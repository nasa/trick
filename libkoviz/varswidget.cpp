#include "varswidget.h"

#ifdef __linux
#include "timeit_linux.h"
#endif

VarsWidget::VarsWidget(const QString &timeName,
                       Runs *runs,
                       const QStringList &unitOverrides,
                       PlotBookModel *plotModel,
                       QItemSelectionModel *plotSelectModel,
                       MonteInputsView *monteInputsView,
                       QWidget *parent) :
    QWidget(parent),
    _timeName(timeName),
    _runs(runs),
    _unitOverrides(unitOverrides),
    _plotModel(plotModel),
    _plotSelectModel(plotSelectModel),
    _monteInputsView(monteInputsView),
    _qpId(0)
{
    // Setup models
    _varsModel = _createVarsModel(_runs);
    _varsFilterModel = new TrickVarSortFilterProxyModel(this);
    _varsFilterModel->setDynamicSortFilter(true);
    _varsFilterModel->setSourceModel(_varsModel);
    QRegExp rx(QString(".*"));
    _varsFilterModel->setFilterRegExp(rx);
    _varsFilterModel->setFilterKeyColumn(0);
    _varsSelectModel = new QItemSelectionModel(_varsFilterModel);
    _varsFilterModel->sort(0,Qt::AscendingOrder);
    connect(_runs, SIGNAL(runsRefreshed()),
            this, SLOT(_runsRefreshed()));

    // Search box
    _gridLayout = new QGridLayout(parent);
    _searchBox = new QLineEdit(parent);
    connect(_searchBox,SIGNAL(textChanged(QString)),
            this,SLOT(_varsSearchBoxTextChanged(QString)));
    _gridLayout->addWidget(_searchBox,0,0);

    // Vars list view
    _listView = new QListView(parent);
    _listView->setDragEnabled(false);
    _listView->setModel(_varsFilterModel);
    _gridLayout->addWidget(_listView,1,0);
    _listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _listView->setSelectionModel(_varsSelectModel);
    _listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _listView->setFocusPolicy(Qt::ClickFocus);
    connect(_varsSelectModel,
         SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
         this,
         SLOT(_varsSelectModelSelectionChanged(QItemSelection,QItemSelection)));
}

VarsWidget::~VarsWidget()
{
    delete _varsSelectModel;
    delete _varsFilterModel;
    delete _varsModel;
}


void VarsWidget::_varsSelectModelSelectionChanged(
                                const QItemSelection &currVarSelection,
                                const QItemSelection &prevVarSelection)
{
    Q_UNUSED(prevVarSelection); // TODO: handle deselection (prevSelection)

    if ( _listView->dragEnabled() ) {
        return;
    }

    if ( currVarSelection.size() == 0 ) return;

    QModelIndexList selIdxs = _varsSelectModel->selection().indexes();

    QModelIndex pageIdx;
    QModelIndex currIdx = _plotSelectModel->currentIndex();
    Qt::KeyboardModifiers keymods = QApplication::keyboardModifiers();
    if ( keymods & Qt::AltModifier && currIdx.isValid() ) {
        // Add variable to last plot on current page
        pageIdx = _plotModel->getIndex(currIdx, "Page");
        QModelIndex plotsIdx = _plotModel->getIndex(pageIdx,"Plots","Page");
        int nplots = _plotModel->rowCount(plotsIdx);
        QModelIndex plotIdx = _plotModel->index(nplots-1,0,plotsIdx);
        QModelIndex curvesIdx = _plotModel->getIndex(plotIdx,
                                                     "Curves", "Plot");
        QModelIndexList currVarIdxs = currVarSelection.indexes();
        foreach (QModelIndex varIdx, currVarIdxs) {
            QString yName = _varsFilterModel->data(varIdx).toString();
            _plotModel->createCurves(curvesIdx,_timeName,yName,_unitOverrides,
                                    _monteInputsView->model(),this);
        }

        // Set y axis label to empty string when adding multivars to plot
        QModelIndex yAxisLabelIdx = _plotModel->getDataIndex(plotIdx,
                                                     "PlotYAxisLabel","Plot");
        _plotModel->setData(yAxisLabelIdx, "");

    } else {

        if ( selIdxs.size() == 1 ) { // Single selection

            QString yName = _varsFilterModel->data(selIdxs.at(0)).toString();
            pageIdx = _findSinglePlotPageWithCurve(yName) ;

            if ( ! pageIdx.isValid() ) {
                // No page w/ single plot of selected var,so create plot of var
                QStandardItem* pageItem = _plotModel->createPageItem();
                _addPlotToPage(pageItem,currVarSelection.indexes().at(0));
                pageIdx = _plotModel->indexFromItem(pageItem);
                _plotSelectModel->setCurrentIndex(pageIdx,
                                                  QItemSelectionModel::Current);
            } else {
                _plotSelectModel->setCurrentIndex(pageIdx,
                                                 QItemSelectionModel::NoUpdate);
            }

        } else {  // Multiple items selected (make pages of 6 plots per page)
            QModelIndex currIdx = _plotSelectModel->currentIndex();
            if ( !currIdx.isValid() ) {
                QStandardItem* pageItem = _plotModel->createPageItem();
                pageIdx = _plotModel->indexFromItem(pageItem);
            } else {
                pageIdx = _plotModel->getIndex(currIdx, "Page");
            }
            QStandardItem* pageItem = _plotModel->itemFromIndex(pageIdx);
            QModelIndexList currVarIdxs = currVarSelection.indexes();
            int rc = currVarIdxs.size();
            QProgressDialog progress("Loading curves...", "Abort", 0, rc, this);
            progress.setWindowModality(Qt::WindowModal);
            progress.setMinimumDuration(500);
#ifdef __linux
            TimeItLinux timer;
            timer.start();
#endif
            int r = 0;
            while ( ! currVarIdxs.isEmpty() ) {
                // Update progress dialog
                progress.setValue(r++);
                if (progress.wasCanceled()) {
                    break;
                }
                QModelIndex varIdx = currVarIdxs.takeFirst();
                int nPlots = _plotModel->plotIdxs(pageIdx).size();
                if ( nPlots == 6 ) {
                    pageItem = _plotModel->createPageItem();
                    pageIdx = _plotModel->indexFromItem(pageItem);
                }
                _addPlotToPage(pageItem,varIdx);
                _plotSelectModel->setCurrentIndex(pageIdx,
                                                  QItemSelectionModel::Current);
#ifdef __linux
                int secs = qRound(timer.stop()/1000000.0);
                div_t d = div(secs,60);
                QString msg = QString("Loaded %1 of %2 curves (%3 min %4 sec)")
                             .arg(r+1).arg(rc).arg(d.quot).arg(d.rem);
                progress.setLabelText(msg);
#endif
            }
            progress.setValue(rc);
        }
    }

    // If live time is set, select curve so live time arrow appears
    QModelIndex liveIdx = _plotModel->getDataIndex(QModelIndex(),
                                                   "LiveCoordTime");
    if ( !_plotModel->data(liveIdx).toString().isEmpty() ) {
        QModelIndex plotsIdx = _plotModel->getIndex(pageIdx,"Plots","Page");
        if ( _plotModel->isChildIndex(plotsIdx,"Plots","Plot")) {
            QModelIndexList plotIdxs = _plotModel->getIndexList(plotsIdx,
                                                                "Plot","Plots");
            QModelIndex plotIdx0 = plotIdxs.at(0);
            if ( _plotModel->isChildIndex(plotIdx0,"Plot","Curves")) {
                QModelIndex curvesIdx = _plotModel->getIndex(plotIdx0,
                                                             "Curves","Plot");
                if (_plotModel->isChildIndex(curvesIdx, "Curves","Curve")) {
                    QModelIndexList curveIdxs = _plotModel->getIndexList(
                                                              curvesIdx,
                                                              "Curve","Curves");
                    QModelIndex curveIdx0 = curveIdxs.at(0);
                    _plotSelectModel->setCurrentIndex(curveIdx0,
                                                 QItemSelectionModel::NoUpdate);
                }
            }
        }
    }
}

void VarsWidget::_runsRefreshed()
{
    _varsFilterModel->setSourceModel(nullptr);
    delete _varsModel;
    _varsModel = _createVarsModel(_runs);
    _varsFilterModel->setSourceModel(_varsModel);
}

void VarsWidget::_varsSearchBoxTextChanged(const QString &rx)
{
    _varsFilterModel->setFilterRegExp(rx);
}

QModelIndex VarsWidget::_findSinglePlotPageWithCurve(const QString& curveYName)
{
    QModelIndex retPageIdx;

    bool isExists = false;
    foreach ( QModelIndex pageIdx, _plotModel->pageIdxs() ) {
        foreach ( QModelIndex plotIdx, _plotModel->plotIdxs(pageIdx) ) {
            QModelIndex curvesIdx = _plotModel->getIndex(plotIdx,
                                                         "Curves", "Plot");
            isExists = true;
            foreach ( QModelIndex curveIdx, _plotModel->curveIdxs(curvesIdx) ) {
                QModelIndex yIdx =  _plotModel->getDataIndex(curveIdx,
                                                         "CurveYName", "Curve");
                QString yName =  _plotModel->data(yIdx).toString();
                isExists = isExists && (yName == curveYName);
            }
            if ( isExists ) {
                retPageIdx = pageIdx;
                break;
            }
            if (isExists) break;
        }
        if (isExists) break;
    }

    return retPageIdx;
}

void VarsWidget::clearSelection()
{
    _varsSelectModel->clear();
}

void VarsWidget::selectAllVars()
{
    _listView->selectAll();
}

void VarsWidget::setDragEnabled(bool isEnabled)
{
    _varsSelectModel->clear();
    if ( isEnabled ) {
        _listView->setSelectionMode(QAbstractItemView::SingleSelection);
    } else {
        _listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
    _listView->setDragEnabled(isEnabled);
}

void VarsWidget::_addPlotToPage(QStandardItem* pageItem,
                                const QModelIndex &varIdx)
{
    QModelIndex pageIdx = _plotModel->indexFromItem(pageItem);
    QModelIndex plotsIdx = _plotModel->getIndex(pageIdx, "Plots", "Page");
    QStandardItem* plotsItem = _plotModel->itemFromIndex(plotsIdx);
    QStandardItem* plotItem = _addChild(plotsItem, "Plot");

    QString yName = _varsFilterModel->data(varIdx).toString();

    int plotId = plotItem->row();
    QString plotName = QString("qp.plot.%0").arg(plotId);

    _addChild(plotItem, "PlotName", plotName);
    _addChild(plotItem, "PlotTitle", "");
    _addChild(plotItem, "PlotMathRect", QRectF());
    _addChild(plotItem, "PlotStartTime", -DBL_MAX);
    _addChild(plotItem, "PlotStopTime",   DBL_MAX);
    _addChild(plotItem, "PlotGrid", true);
    _addChild(plotItem, "PlotRatio", "");
    _addChild(plotItem, "PlotXScale", "linear");
    _addChild(plotItem, "PlotYScale", "linear");
    _addChild(plotItem, "PlotXMinRange", -DBL_MAX);
    _addChild(plotItem, "PlotXMaxRange",  DBL_MAX);
    _addChild(plotItem, "PlotYMinRange", -DBL_MAX);
    _addChild(plotItem, "PlotYMaxRange",  DBL_MAX);
    _addChild(plotItem, "PlotBackgroundColor", "#FFFFFF");
    _addChild(plotItem, "PlotForegroundColor", "#000000");
    int rc = _runs->runPaths().size(); // a curve per run, so, rc == nCurves
    if ( rc == 2 ) {
        QString presentation = _plotModel->getDataString(QModelIndex(),
                                                         "Presentation");
        if ( ! presentation.isEmpty() ) {
            _addChild(plotItem, "PlotPresentation", presentation);
        } else {
            _addChild(plotItem, "PlotPresentation", "compare");
        }
    } else {
        _addChild(plotItem, "PlotPresentation", "compare");
    }
    _addChild(plotItem, "PlotYAxisLabel", yName);
    _addChild(plotItem, "PlotRect", QRect(0,0,0,0));

    QStandardItem *curvesItem = _addChild(plotItem,"Curves");
    QModelIndex curvesIdx = _plotModel->indexFromItem(curvesItem);
    _plotModel->createCurves(curvesIdx,_timeName,yName,_unitOverrides,
                             _monteInputsView->model(),this);

    // Calculate xaxislabel based on curves just added
    QString xAxisLabel(_timeName);
    QModelIndexList curveIdxs = _plotModel->curveIdxs(curvesIdx);
    bool isFirstCurve = true;
    QString label;
    foreach ( QModelIndex curveIdx, curveIdxs ) {
        QString  l = _plotModel->getDataString(curveIdx,
                                               "CurveTimeName","Curve");
        if ( isFirstCurve ) {
            label = l;
            isFirstCurve = false;
        } else {
            if ( l != label ) {
                // Two curves with different timeNames
                label.clear();
                break;
            }
        }
    }
    if ( !label.isEmpty() ) {
        xAxisLabel = label;
    }
    _addChild(plotItem, "PlotXAxisLabel", xAxisLabel);

    // Reset monte carlo input view current idx to signal current changed
    int currRunId = -1;
    if ( _monteInputsView ) {
        currRunId = _monteInputsView->currentRun();
    }

    if ( currRunId >= 0 ) {
        foreach (QModelIndex curveIdx, _plotModel->curveIdxs(curvesIdx)) {
            int curveRunId = _plotModel->getDataInt(curveIdx,
                                                    "CurveRunID","Curve");
            if ( curveRunId == currRunId ) {
                // Reset monte input view's current index which will set
                // plot view's current index (by way of signal/slot connections)
                QModelIndex currIdx = _monteInputsView->currentIndex();
                _monteInputsView->setCurrentIndex(QModelIndex());
                _monteInputsView->setCurrentIndex(currIdx);
                break;
            }
        }
    }
}

QStandardItem* VarsWidget::_addChild(QStandardItem *parentItem,
                             const QString &childTitle,
                             const QVariant& childValue)
{
    return(_plotModel->addChild(parentItem,childTitle,childValue));
}

// Search for yparam with a *single plot* on a page with curve
// This is really a hackish helper for _varsSelectModelSelectionChanged()
void VarsWidget::_selectCurrentRunOnPageItem(QStandardItem* pageItem)
{
    int runId = -1;
    if ( _monteInputsView ) {
        runId = _monteInputsView->currentRun();
    }

    if ( runId >= 0 ) {
        QItemSelection currSel = _plotSelectModel->selection();
        QModelIndex pageIdx = _plotModel->indexFromItem(pageItem);
        foreach ( QModelIndex plotIdx, _plotModel->plotIdxs(pageIdx) ) {
            QModelIndex curvesIdx = _plotModel->getIndex(plotIdx,
                                                         "Curves", "Plot");
            QModelIndex curveIdx = _plotModel->curveIdxs(curvesIdx).at(runId);
            if ( ! currSel.contains(curveIdx) ) {
                QItemSelection curveSel(curveIdx,curveIdx) ;
                _plotSelectModel->select(curveSel,QItemSelectionModel::Select);
            }
        }
    }
}

QStandardItemModel* VarsWidget::_createVarsModel(Runs* runs)
{
    if ( runs == 0 ) return 0;

    QStandardItemModel* varsModel = new QStandardItemModel(0,1);

    QStringList params = runs->params();
    params.sort();
    QStandardItem *rootItem = varsModel->invisibleRootItem();
    foreach (QString param, params) {
        if ( param == "sys.exec.out.time" ) continue;
        QStandardItem *varItem = new QStandardItem(param);
        varItem->setData("VarsModel",Qt::UserRole); // For Drag-n-drop
        rootItem->appendRow(varItem);
    }

    return varsModel;
}

#include "varswidget.h"

VarsWidget::VarsWidget(QStandardItemModel* varsModel,
                       MonteModel *monteModel,
                       PlotBookModel *plotModel,
                       QItemSelectionModel *plotSelectModel,
                       MonteInputsView *monteInputsView,
                       QWidget *parent) :
    QWidget(parent),
    _varsModel(varsModel),
    _monteModel(monteModel),
    _plotModel(plotModel),
    _plotSelectModel(plotSelectModel),
    _monteInputsView(monteInputsView),
    _currQPIdx(0)
{
    // Setup models
    _varsFilterModel = new QSortFilterProxyModel;
    _varsFilterModel->setDynamicSortFilter(true);
    _varsFilterModel->setSourceModel(_varsModel);
    QRegExp rx(QString(".*"));
    _varsFilterModel->setFilterRegExp(rx);
    _varsFilterModel->setFilterKeyColumn(0);
    _varsSelectModel = new QItemSelectionModel(_varsFilterModel);

    // Search box
    _gridLayout = new QGridLayout(parent);
    _searchBox = new QLineEdit(parent);
    connect(_searchBox,SIGNAL(textChanged(QString)),
            this,SLOT(_varsSearchBoxTextChanged(QString)));
    _gridLayout->addWidget(_searchBox,0,0);

    // Vars list view
    _listView = new QListView(parent);
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


void VarsWidget::_varsSelectModelSelectionChanged(
                                const QItemSelection &currVarSelection,
                                const QItemSelection &prevVarSelection)
{
    Q_UNUSED(prevVarSelection); // TODO: handle deselection (prevSelection)

    if ( currVarSelection.size() == 0 ) return;

    QModelIndex pageIdx; // for new or selected qp page
    QModelIndexList selIdxs = _varsSelectModel->selection().indexes();

    if ( selIdxs.size() == 1 ) { // Single selection

        QString yName = _varsFilterModel->data(selIdxs.at(0)).toString();
        pageIdx = _findSinglePlotPageWithCurve(yName) ;

        if ( ! pageIdx.isValid() ) {
            // No page with single plot of selected var, so create plot of var
            QStandardItem* pageItem = _createPageItem();
            _addPlotToPage(pageItem,currVarSelection.indexes().at(0));
            pageIdx = _plotModel->indexFromItem(pageItem);
            _plotSelectModel->setCurrentIndex(pageIdx,
                                              QItemSelectionModel::Current);
            //_selectCurrentRunOnPageItem(pageItem);
        } else {
            _plotSelectModel->setCurrentIndex(pageIdx,
                                              QItemSelectionModel::NoUpdate);
        }

    } else {  // Multiple items selected (make pages of 6 plots per page)
        QModelIndex currIdx = _plotSelectModel->currentIndex();
        QModelIndex pageIdx = _plotModel->getIndex(currIdx, "Page");
        QStandardItem* pageItem = _plotModel->itemFromIndex(pageIdx);
        QModelIndexList currVarIdxs = currVarSelection.indexes();
        while ( ! currVarIdxs.isEmpty() ) {
            QModelIndex varIdx = currVarIdxs.takeFirst();
            int nPlots = _plotModel->plotIdxs(pageIdx).size();
            if ( nPlots == 6 ) {
                pageItem = _createPageItem();
                pageIdx = _plotModel->indexFromItem(pageItem);
            }
            _addPlotToPage(pageItem,varIdx);
            _plotSelectModel->setCurrentIndex(pageIdx,
                                              QItemSelectionModel::Current);
            //_selectCurrentRunOnPageItem(pageItem);
        }
    }
}

void VarsWidget::_varsSearchBoxTextChanged(const QString &rx)
{
    _varsFilterModel->setFilterRegExp(rx);
}

QModelIndex VarsWidget::_findSinglePlotPageWithCurve(const QString& curveName)
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
                isExists = isExists && (yName == curveName);
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

QStandardItem* VarsWidget::_createPageItem()
{
    QModelIndex pagesIdx = _plotModel->getIndex(QModelIndex(), "Pages");
    QStandardItem* pagesItem = _plotModel->itemFromIndex(pagesIdx);
    QStandardItem* pageItem = _addChild(pagesItem, "Page");

    QString title = QString("QP_%0").arg(_currQPIdx++);
    _addChild(pageItem, "PageName", title);
    _addChild(pageItem, "PageTitle", title);
    _addChild(pageItem, "PageStartTime", -DBL_MAX);
    _addChild(pageItem, "PageStopTime",   DBL_MAX);
    _addChild(pageItem, "PageBackgroundColor", "#FFFFFF");
    _addChild(pageItem, "PageForegroundColor", "#000000");
    _addChild(pageItem, "Plots");

    return pageItem;
}

void VarsWidget::_addPlotToPage(QStandardItem* pageItem,
                                const QModelIndex &varIdx)
{
    QModelIndex pageIdx = _plotModel->indexFromItem(pageItem);
    QModelIndex plotsIdx = _plotModel->getIndex(pageIdx, "Plots", "Page");
    QStandardItem* plotsItem = _plotModel->itemFromIndex(plotsIdx);
    QStandardItem* plotItem = _addChild(plotsItem, "Plot");

    QString tName = _monteModel->headerData(0).toString();
    QString xName(tName);
    QString yName = _varsFilterModel->data(varIdx).toString();

    int plotId = plotItem->row();
    QString plotName = QString("QPlot_%0").arg(plotId);

    _addChild(plotItem, "PlotName", plotName);
    _addChild(plotItem, "PlotTitle", "");
    _addChild(plotItem, "PlotMathRect", QRectF());
    int rc = _monteModel->rowCount(); // a curve per run, so, rc == nCurves
    if ( rc == 2 ) {
        _addChild(plotItem, "PlotPresentation", "error");
    } else {
        _addChild(plotItem, "PlotPresentation", "coplot");
    }
    _addChild(plotItem, "PlotXAxisLabel", xName);
    _addChild(plotItem, "PlotYAxisLabel", yName);

    QStandardItem *curvesItem = _addChild(plotItem,"Curves");


    QList<QColor> colors = _plotModel->createCurveColors(rc);
    for ( int r = 0; r < rc; ++r) {

        //
        // Create curves
        //
        TrickCurveModel* curveModel = _monteModel->curve(r,tName,xName,yName);
        if ( !curveModel ) {
            // This should not happen
            // It could be ignored but I'll exit(-1) because I think
            // if this happens it's a programming error, not a user error
            qDebug() << "snap [bad scoobs]: varswidget.cpp";
            exit(-1);
        }

        // Turn off model signals when adding children for significant speedup
        bool block = _plotModel->blockSignals(true);

        QStandardItem *curveItem = _addChild(curvesItem,"Curve");

        QString curveName = QString("Curve_%0").arg(r);
        _addChild(curveItem, "CurveName", curveName);
        _addChild(curveItem, "CurveTime", tName);
        _addChild(curveItem, "CurveTimeUnit", curveModel->t()->unit());
        _addChild(curveItem, "CurveXName", xName);
        _addChild(curveItem, "CurveXUnit", curveModel->t()->unit()); // yes,t
        _addChild(curveItem, "CurveYName", yName);
        _addChild(curveItem, "CurveYUnit", curveModel->y()->unit());
        _addChild(curveItem, "CurveRunID", r);
        _addChild(curveItem, "CurveXScale", 1.0);
        _addChild(curveItem, "CurveXBias", 0.0);
        _addChild(curveItem, "CurveYScale", 1.0);
        _addChild(curveItem, "CurveYBias", 0.0);
        _addChild(curveItem, "CurveColor", colors.at(r).name());
        _addChild(curveItem, "CurveSymbolStyle", "");
        _addChild(curveItem, "CurveSymbolSize", "");
        _addChild(curveItem, "CurveLineStyle", "");
        _addChild(curveItem, "CurveYLabel", "");

        // Turn signals back on before adding curveModel
        block = _plotModel->blockSignals(block);

        // Add actual curve model data
        QVariant v = PtrToQVariant<TrickCurveModel>::convert(curveModel);
        _addChild(curveItem, "CurveData", v);
    }

    // Initialize plot math rect to curves bounding box
    QModelIndex curvesIdx = curvesItem->index();
    QRectF bbox = _plotModel->calcCurvesBBox(curvesIdx);
    QModelIndex plotIdx = plotItem->index();
    QModelIndex plotMathRectIdx = _plotModel->getDataIndex(plotIdx,
                                                           "PlotMathRect",
                                                           "Plot");
    _plotModel->setData(plotMathRectIdx,bbox);
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
    int runId = _currSelectedRun();

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

int VarsWidget::_currSelectedRun()
{
    int runId = -1;
    if ( _monteInputsView ) {
        runId = _monteInputsView->currSelectedRun();
    } else {
        QItemSelection currSel = _plotSelectModel->selection();
        foreach ( QModelIndex idx , currSel.indexes() ) {
            if ( _plotModel->isIndex(idx, "Curve") ) {
                runId = idx.row();
                break;
            }
        }
    }
    return runId;
}

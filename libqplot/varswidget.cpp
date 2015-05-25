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
            _addPlotOfVarToPageItem(pageItem,currVarSelection.indexes().at(0));
            _selectCurrentRunOnPageItem(pageItem);
        } else {
            _plotSelectModel->setCurrentIndex(pageIdx,
                                              QItemSelectionModel::NoUpdate);
        }

    } else {  // Multiple items selected.
        QModelIndex currIdx = _plotSelectModel->currentIndex();
        QModelIndex pageIdx = _plotModel->pageIdx(currIdx);
        QStandardItem* pageItem = _plotModel->itemFromIndex(pageIdx);
        QModelIndexList currVarIdxs = currVarSelection.indexes();
        while ( ! currVarIdxs.isEmpty() ) {
            QModelIndex varIdx = currVarIdxs.takeFirst();
            if ( _plotModel->plotIdxs(pageIdx).size() >= 6 ) {
                pageItem = _createPageItem();
                pageIdx = _plotModel->indexFromItem(pageItem);
                //_plotBookView->setCurrentPage(pageIdx);
            }
            _addPlotOfVarToPageItem(pageItem,varIdx);
            _selectCurrentRunOnPageItem(pageItem);
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
            QModelIndex curvesIdx = _plotModel->curvesIdx(plotIdx);
            isExists = true;
            foreach ( QModelIndex curveIdx, _plotModel->curveIdxs(curvesIdx) ) {
                QModelIndex yIdx =  _plotModel->index(2,0,curveIdx);
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
    QStandardItem* pageItem;
    QStandardItem *rootItem = _plotModel->invisibleRootItem();
    QString pageItemName = QString("QP_%0").arg(_currQPIdx++);
    pageItem = new QStandardItem(pageItemName);
    rootItem->appendRow(pageItem);

    QStandardItem* pageTitleItem = new QStandardItem(pageItemName);
    pageItem->appendRow(pageTitleItem);

    double pageStartTime = -DBL_MAX;
    QString pageStartTimeStr = QString("%1").arg(pageStartTime);
    QStandardItem *pageStartTimeItem = new QStandardItem(pageStartTimeStr);
    pageStartTimeItem->setData(pageStartTime);
    pageItem->appendRow(pageStartTimeItem);

    double pageStopTime = DBL_MAX;
    QString pageStopTimeStr = QString("%1").arg(pageStopTime);
    QStandardItem *pageStopTimeItem = new QStandardItem(pageStopTimeStr);
    pageStopTimeItem->setData(pageStopTime);
    pageItem->appendRow(pageStopTimeItem);

    return pageItem;
}

void VarsWidget::_addPlotOfVarToPageItem(QStandardItem* pageItem,
                                         const QModelIndex &varIdx)
{
    // The -1 is because a pageItem has a pageTitle followed by plotItems
    int nPlots = pageItem->rowCount()-1;

    QString tName = _monteModel->headerData(0).toString();
    QString xName(tName);
    QString yName = _varsFilterModel->data(varIdx).toString();

    QString plotTitle = QString("QPlot_%0").arg(nPlots);
    QStandardItem* plotItem = new QStandardItem(plotTitle);
    pageItem->appendRow(plotItem);

    QStandardItem *xAxisLabelItem = new QStandardItem(xName);
    plotItem->appendRow(xAxisLabelItem);

    QStandardItem *yAxisLabelItem = new QStandardItem(yName);
    plotItem->appendRow(yAxisLabelItem);

    QStandardItem *curvesItem = new QStandardItem("Curves");
    plotItem->appendRow(curvesItem);

    for ( int r = 0; r < _monteModel->rowCount(); ++r) {

        //
        // Create curve
        //
        QString curveName = QString("Curve_%0").arg(r);
        QStandardItem *curveItem = new QStandardItem(curveName);
        curvesItem->appendRow(curveItem);

        QStandardItem *tItem       = new QStandardItem(tName);
        QStandardItem *xItem       = new QStandardItem(xName);
        QStandardItem *yItem       = new QStandardItem(yName);
        QStandardItem *tUnitItem   = new QStandardItem("--");
        QStandardItem *xUnitItem   = new QStandardItem("--");
        QStandardItem *yUnitItem   = new QStandardItem("--");
        QStandardItem *runIDItem   = new QStandardItem(QString("%0").arg(r));
        QStandardItem *curveDataItem   = new QStandardItem("");
        QStandardItem *colorItem   = new QStandardItem("");

        curveItem->appendRow(tItem);
        curveItem->appendRow(xItem);
        curveItem->appendRow(yItem);
        curveItem->appendRow(tUnitItem);
        curveItem->appendRow(xUnitItem);
        curveItem->appendRow(yUnitItem);
        curveItem->appendRow(runIDItem);
        curveItem->appendRow(curveDataItem);
        curveItem->appendRow(colorItem);
    }
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
            QModelIndex curvesIdx;
            for ( int j = 0; j < plotIdx.model()->rowCount(plotIdx); ++j) {
                QModelIndex idx = plotIdx.model()->index(j,0,plotIdx);
                if ( plotIdx.model()->data(idx).toString() == "Curves" ) {
                    curvesIdx = idx;
                    break;
                }
            }
            if ( !curvesIdx.isValid() ) {
                qDebug() << "snap [bad scoobies]: \"Curves\" item not found.";
                exit(-1);
            }
            QModelIndex curveIdx = pageIdx.model()->index(runId,0,curvesIdx);
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
        foreach ( QModelIndex i , currSel.indexes() ) {
            if ( _isCurveIdx(i) ) {
                runId = i.row();
                break;
            }
        }
    }
    return runId;
}

// _plotModel will have a curve on a branch like this:
//           root->page->plot->curves->curvei
//
bool VarsWidget::_isCurveIdx(const QModelIndex &idx) const
{
    if ( idx.model() != _plotModel ) return false;
    if ( idx.model()->data(idx.parent()).toString() != "Curves" ) {
        return false;
    } else {
        return true;
    }
}


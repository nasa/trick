#include "plotbookmodel.h"
#include <float.h>

PlotBookModel::PlotBookModel(QObject *parent) :
    QStandardItemModel(parent)
{
    _initModel();
}

PlotBookModel::PlotBookModel(int rows, int columns, QObject *parent) :
    QStandardItemModel(rows,columns,parent)
{
    _initModel();
}

QModelIndexList PlotBookModel::pageIdxs() const
{
    QModelIndexList idxs;
    QStandardItem* rootItem = this->invisibleRootItem();
    for ( int i = 0; i < rootItem->rowCount(); ++i ) {
        QStandardItem* item = rootItem->child(i);
        QString itemText = item->text();
        if ( itemText == "start" || itemText == "stop" ) continue;
        QModelIndex idx = this->indexFromItem(item);
        idxs.append(idx);
    }
    return idxs;
}

QModelIndexList PlotBookModel::plotIdxs(const QModelIndex &pageIdx) const
{
    QModelIndexList idxs;
    int rc = rowCount(pageIdx);
    // Start at 3 because page title=0, startTime=1, stopTime=2
    for ( int i = 3 ; i < rc; ++i ) {
        idxs.append(index(i,0,pageIdx));
    }

    return idxs;
}

bool PlotBookModel::isPageIdx(const QModelIndex &idx) const
{
    // rows 0,1 are start/stop time
    return (idx.isValid() && !idx.parent().isValid()
            && idx.row() != 0 && idx.row() != 1 ) ;
}

void PlotBookModel::_initModel()
{
    QStandardItem *rootItem = invisibleRootItem();

    QStandardItem *startItem = new QStandardItem("start");
    startItem->setData(-DBL_MAX);
    rootItem->appendRow(startItem);

    QStandardItem *stopItem = new QStandardItem("stop");
    stopItem->setData(DBL_MAX);
    rootItem->appendRow(stopItem);
}

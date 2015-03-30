#include "plotbookmodel.h"

PlotBookModel::PlotBookModel(QObject *parent) :
    QStandardItemModel(parent)
{
}

PlotBookModel::PlotBookModel(int rows, int columns, QObject *parent) :
    QStandardItemModel(rows,columns,parent)
{
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

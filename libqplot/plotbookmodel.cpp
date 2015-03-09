#include "plotbookmodel.h"

PlotBookModel::PlotBookModel(QObject *parent) :
    QStandardItemModel(parent)
{
}

PlotBookModel::PlotBookModel(int rows, int columns, QObject *parent) :
    QStandardItemModel(rows,columns,parent)
{
}

#include "montemodel.h"

MonteModel::MonteModel(Runs *runs, QObject *parent) : QStandardItemModel(parent),
    _runs(runs)
{
    _params = _runs->params();
    _runDirs = _runs->runs();

    foreach ( QString run, _runDirs ) {
        QList<QStandardItem*> items;
        foreach ( QString param, _params ) {
            QString s = run + ":" + param ;
            QStandardItem* item = new QStandardItem(s);
            items.append(item);
        }
        appendRow(items);
    }
}

MonteModel::~MonteModel()
{
    // The client is responsible for cleaning up runs
}

TrickCurveModel *MonteModel::curve(int row,
                                   const QString &tparam,
                                   const QString &xparam,
                                   const QString &yparam) const
{
    return _runs->curve(row,tparam,xparam,yparam);
}

QVariant MonteModel::headerData(int sect, Qt::Orientation orientation,
                               int role) const
{
    if ( sect < 0 ) return QVariant();

    if ( orientation == Qt::Horizontal && sect >= columnCount()) {
        return QVariant();
    }
    if ( orientation == Qt::Vertical && sect >= rowCount()) {
        return QVariant();
    }

    QVariant ret;

    if ( role == Qt::DisplayRole ) {
        if ( orientation == Qt::Horizontal ) {
            ret = _params.at(sect);
        } else {
            ret = _runDirs.at(sect);
        }
    }

    return ret;
}

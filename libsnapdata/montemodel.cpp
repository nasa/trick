#include "montemodel.h"
#include <stdexcept>

MonteModel::MonteModel(Runs *runs, QObject *parent) :
    QAbstractItemModel(parent),
    _runs(runs)
{
    _params = runs->params();
    int col = 0 ;
    foreach ( QString param, _params ) {
        _param2column.insert(param,col);
        col++;
    }
    _runDirs = _runs->runs();
}

MonteModel::~MonteModel()
{
    // TODO!!!! the caller should delete runs passed in
    //delete _runs;
}

TrickCurveModel *MonteModel::curve(const QModelIndex &idx) const
{
    QString yparam = _params.at(idx.column());
    QList<TrickModel*>* models = _runs->models(yparam);
    TrickModel* tm = models->at(idx.row());
    int ycol = tm->paramColumn(yparam) ;
    return new TrickCurveModel(tm,0,0,ycol,yparam);
}

TrickCurveModel *MonteModel::curve(int row, const QString &param) const
{
    QList<TrickModel*>* models = _runs->models(param);
    TrickModel* tm = models->at(row);
    int ycol = tm->paramColumn(param) ;
    return new TrickCurveModel(tm,0,0,ycol,param);
}

int MonteModel::paramColumn(const QString &param)
{
    return _param2column.value(param);
}


QModelIndex MonteModel::parent(const QModelIndex &index) const
{
    if ( !index.isValid() ) {
        return index; // get rid of compiler warning of unused param
    } else {
        QModelIndex rootidx = QModelIndex();
        return rootidx;
    }
}


QModelIndex MonteModel::index(int row, int column,
                                  const QModelIndex &pidx) const
{
    QModelIndex idx = QModelIndex();

    if ( !pidx.isValid() && row < rowCount() && column < columnCount() ) {
        idx = createIndex(row,column);
    }

    return idx;
}

int MonteModel::rowCount(const QModelIndex &pidx) const
{
    if ( ! pidx.isValid() ) {
        return _runDirs.size();
    } else {
        return 0;
    }
}

int MonteModel::columnCount(const QModelIndex &pidx) const
{
    int ncols = 0 ;

    if ( ! pidx.isValid() ) {
        ncols = _runs->params().size();
    }

    return ncols;
}

QVariant MonteModel::data(const QModelIndex &idx, int role) const
{
    QVariant val;


    if ( idx.isValid() ) {
        if ( role == Qt::DisplayRole ) {
            QString run = headerData(idx.row(),
                               Qt::Vertical,
                               Qt::DisplayRole).toString();
            QString param = headerData(idx.column(),
                               Qt::Horizontal,
                               Qt::DisplayRole).toString();
            val = run + ":" + param;
        }
    }

    return val;
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

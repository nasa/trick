#include "montemodel.h"
#include <stdexcept>

QString MonteModel::_err_string;
QTextStream MonteModel::_err_stream(&MonteModel::_err_string);

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

TrickCurveModel *MonteModel::curve(const QModelIndex &xIdx,
                                   const QModelIndex &yIdx,
                                   double xScaleFactor,
                                   double yScaleFactor) const
{
    QString yparam = _params.at(yIdx.column());
    QList<TrickModel*>* models = _runs->models(yparam);
    TrickModel* tm = models->at(yIdx.row());
    int ycol = tm->paramColumn(yparam) ;

    QString xparam = _params.at(xIdx.column());
    int xcol = tm->paramColumn(xparam) ;
    if ( xcol < 0 ) {
        _err_stream << "snap [error]: MonteModel::curve() called with bad "
                    << "xparam:\n" << xparam << "\nMaybe it's a cross plot "
                    << "where x is not in same model as y:.\n"
                    << yparam ;
        throw std::runtime_error(_err_string.toAscii().constData());
    }

    return new TrickCurveModel(tm,0,xcol,ycol,yparam,
                               xScaleFactor,yScaleFactor);
}


TrickCurveModel *MonteModel::curve(int row,
                                   const QString &xparam,
                                   const QString &yparam,
                                   double xScaleFactor,
                                   double yScaleFactor) const
{
    QList<TrickModel*>* models = _runs->models(yparam);
    if ( models == 0 ) {
        _err_stream << "snap [error]: parameter \""
                    << yparam << "\" does not exist in RUN data";
        throw std::runtime_error(_err_string.toAscii().constData());
    }
    TrickModel* tm = models->at(row);
    int ycol = tm->paramColumn(yparam) ;

    int xcol = tm->paramColumn(xparam) ;
    if ( xcol < 0 ) {
        _err_stream << "snap [error]: MonteModel::curve() called with bad "
                    << "xy pairing.   Maybe it's a cross plot "
                    << "where x is not in same model as y:.\n"
                    << "x=" << xparam << "\n"
                    << "y=" << yparam << "\n";
        throw std::runtime_error(_err_string.toAscii().constData());
    }

    return new TrickCurveModel(tm,0,xcol,ycol,yparam,
                               xScaleFactor,yScaleFactor);
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

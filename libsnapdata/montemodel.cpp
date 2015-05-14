#include "montemodel.h"
#include <stdexcept>

QString MonteModel::_err_string;
QTextStream MonteModel::_err_stream(&MonteModel::_err_string);

MonteModel::MonteModel(Runs *runs, QObject *parent) : QStandardItemModel(parent),
    _runs(runs)
{
    _params = runs->params();
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

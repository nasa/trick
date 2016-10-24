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
        _err_stream << "snap [error]: MonteModel::curve(): Could not find "
                    << "(x,y) pair:\n\n"
                    << "        x=" << xparam << "\n"
                    << "        y=" << yparam << "\n\n"
                    << "in the following RUNs:\n\n";
        int i = 0;
        foreach (  QString run, _runs->runs() ) {
            _err_stream << "        " << run << "\n";
            if ( ++i > 10 ) {
                _err_stream << "...<more>";
                break;
            }
        }
        throw std::runtime_error(_err_string.toLatin1().constData());
    }

    return new TrickCurveModel(tm,0,xcol,ycol,yparam,
                               xScaleFactor,yScaleFactor);
}


TrickCurveModel *MonteModel::curve(int row,
                                   const QString &tparam,
                                   const QString &xparam,
                                   const QString &yparam,
                                   double xScaleFactor,
                                   double yScaleFactor) const
{
    QList<TrickModel*>* models = _runs->models(yparam);
    if ( models == 0 ) {
        return 0;
    }

    TrickModel* tm = models->at(row);
    if ( !tm ) {
        return 0;
    }

    int tcol = tm->paramColumn(tparam) ;
    if ( tcol < 0 ) {
        return 0;
        _err_stream << "snap [error]: MonteModel::curve() : "
                       "Could not find:\n\n"
                       "          time param:\n\n                \""
                    << tparam << "\"\n\n          in file:\n\n                "
                    << tm->trkFile();
        throw std::runtime_error(_err_string.toLatin1().constData());
    }
    int ycol = tm->paramColumn(yparam) ;
    QString xp = xparam;
    if ( xp.isEmpty() ) {
        xp = tparam;
    }
    int xcol = tm->paramColumn(xp) ;
    if ( xcol < 0 ) {
        _err_stream << "snap [error]: MonteModel::curve(): Could not find "
                    << "(x,y) pair:\n\n"
                    << "        x=" << xp << "\n"
                    << "        y=" << yparam << "\n\n"
                    << "in the following RUNs:\n\n";
        int i = 0;
        foreach (  QString run, _runs->runs() ) {
            _err_stream << "        " << run << "\n";
            if ( ++i > 10 ) {
                _err_stream << "...<more>";
                break;
            }
        }
        throw std::runtime_error(_err_string.toLatin1().constData());
    }

    return new TrickCurveModel(tm,tcol,xcol,ycol,yparam,
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

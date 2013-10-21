#include "montemodel.h"
#include <stdexcept>

MonteModel::MonteModel(const QString &monteDir,
                       QObject *parent):
    QAbstractItemModel(parent)
{
    _monte = new Monte(monteDir);
    _params = _monte->params();
    int col = 0 ;
    foreach ( QString param, _params ) {
        _param2column.insert(param,col);
        col++;
    }
    _runs = _monte->runs();
}

MonteModel::~MonteModel()
{
    delete _monte;
}

TrickCurveModel *MonteModel::curve(const QModelIndex &idx) const
{
    QString yparam = _params.at(idx.column());
    QList<TrickModel*>* models = _monte->models(yparam);
    TrickModel* tm = models->at(idx.row());
    int ycol = tm->paramColumn(yparam) ;
    for ( ; ycol < tm->columnCount(); ++ycol) {
        if ( tm->headerData(ycol,Qt::Horizontal,Param::Name) == yparam ) {
            break;
        }
    }
    int d = tm->paramColumn(yparam);
    if ( d != ycol ) {
        qDebug() << "die!";
        exit(-1);
    }
    return new TrickCurveModel(tm,0,0,ycol,yparam);
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
        return _runs.size();
    } else {
        return 0;
    }
}

int MonteModel::columnCount(const QModelIndex &pidx) const
{
    int ncols = 0 ;

    if ( ! pidx.isValid() ) {
        ncols = _monte->params().size();
    }

    return ncols;
}

QVariant MonteModel::data(const QModelIndex &idx, int role) const
{
    QVariant val;


    if ( idx.isValid() ) {
        if ( role == Qt::DisplayRole ) {
            val = QString("MonteModel->data() not implemented");
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
            ret = _runs.at(sect);
        }
    }

    return ret;
}

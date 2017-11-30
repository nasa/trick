#include "curvemodel.h"

CurveModel::CurveModel(DataModel *datamodel,
                       int tcol, int xcol, int ycol,
                       QObject *parent) :
    QAbstractTableModel(parent),
    _datamodel(datamodel),
    _tcol(tcol),
    _xcol(xcol),
    _ycol(ycol)
{
}

int CurveModel::rowCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() && _datamodel ) {
        return _datamodel->rowCount();
    } else {
        return 0;
    }
}

int CurveModel::columnCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() && _datamodel ) {
        return 3;
    } else {
        return 0;
    }
}

// TODO CurveModel::data() --- for now, return empty QVariant
QVariant CurveModel::data (const QModelIndex & index, int role ) const

{
    Q_UNUSED(index);
    Q_UNUSED(role);
    QVariant v;
    return v;
}

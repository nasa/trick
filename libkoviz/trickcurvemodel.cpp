#include "trickcurvemodel.h"
#include <stdio.h>
#include <stdexcept>

TrickCurveModel::TrickCurveModel(TrickModel* trickmodel,
                                int tcol, int xcol, int ycol,
                                QObject *parent) :
    QAbstractTableModel(parent),
    _trickmodel(trickmodel),
    _tcol(tcol),
    _xcol(xcol),
    _ycol(ycol)
{
    _t = _trickmodel->param(tcol);
    _x = _trickmodel->param(xcol);
    _y = _trickmodel->param(ycol);
}

// Destructor does not delete _trickModel
TrickCurveModel::~TrickCurveModel()
{
}

TrickModelIterator TrickCurveModel::begin() const
{
    return TrickModelIterator(0,_trickmodel,_tcol,_xcol,_ycol);
}

TrickModelIterator TrickCurveModel::end() const
{
    return TrickModelIterator(_trickmodel->rowCount(),
                              _trickmodel,_tcol,_xcol,_ycol);
}


int TrickCurveModel::rowCount(const QModelIndex &pidx) const
{
    if ( ! pidx.isValid() ) {
        return _trickmodel->rowCount();
    } else {
        return 0;
    }
}

int TrickCurveModel::columnCount(const QModelIndex &pidx) const
{
    if ( ! pidx.isValid() ) {
        return 3;
    } else {
        return 0;
    }
}

QVariant TrickCurveModel::data (const QModelIndex & index, int role ) const

{
    return _trickmodel->data(index,role);
}

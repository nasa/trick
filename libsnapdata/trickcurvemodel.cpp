#include "trickcurvemodel.h"
#include <stdio.h>
#include <stdexcept>

TrickCurveModel::TrickCurveModel(TrickModel* trickmodel,
                                int tcol, int xcol, int ycol,
                                const QString &tableName,
                                double xScaleFactor, double yScaleFactor,
                                QObject *parent) :
    QAbstractTableModel(parent),
    _trickmodel(trickmodel),
    _tcol(tcol),
    _xcol(xcol),
    _ycol(ycol),
    _tableName(tableName),
    _xScaleFactor(xScaleFactor),
    _yScaleFactor(yScaleFactor)
{

    Qt::Orientation h = Qt::Horizontal;

    _t.setName(_trickmodel->headerData(tcol,h,Param::Name).toString());
    _t.setUnit(_trickmodel->headerData(tcol,h,Param::Unit).toString());
    _t.setType(_trickmodel->headerData(tcol,h,Param::Type).toInt());
    _t.setSize(_trickmodel->headerData(tcol,h,Param::Size).toInt());

    _x.setName(_trickmodel->headerData(xcol,h,Param::Name).toString());
    _x.setUnit(_trickmodel->headerData(xcol,h,Param::Unit).toString());
    _x.setType(_trickmodel->headerData(xcol,h,Param::Type).toInt());
    _x.setSize(_trickmodel->headerData(xcol,h,Param::Size).toInt());

    _y.setName(_trickmodel->headerData(ycol,h,Param::Name).toString());
    _y.setUnit(_trickmodel->headerData(ycol,h,Param::Unit).toString());
    _y.setType(_trickmodel->headerData(ycol,h,Param::Type).toInt());
    _y.setSize(_trickmodel->headerData(ycol,h,Param::Size).toInt());
}

// Destructor does not delete _trickModel
TrickCurveModel::~TrickCurveModel()
{
}

TrickModelIterator TrickCurveModel::begin() const
{
    return TrickModelIterator(0,_trickmodel,_tcol,_xcol,_ycol,
                              _xScaleFactor,_yScaleFactor);
}

TrickModelIterator TrickCurveModel::end() const
{
    return TrickModelIterator(_trickmodel->rowCount(),
                              _trickmodel,_tcol,_xcol,_ycol,
                              _xScaleFactor,_yScaleFactor);
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

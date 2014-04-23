#include "trickcurvemodel.h"
#include <stdio.h>
#include <stdexcept>

TrickCurveModel::TrickCurveModel(TrickModel* trickmodel,
                                int tcol, int xcol, int ycol,
                                const QString &tableName,
                                double xScaleFactor, double yScaleFactor,
                                QObject *parent) :
    SnapTable(tableName,parent),
    _trickmodel(trickmodel),
    _tcol(tcol),
    _xcol(xcol),
    _ycol(ycol),
    _xScaleFactor(xScaleFactor),
    _yScaleFactor(yScaleFactor)
{
    QList<int> cols;
    cols << tcol << xcol << ycol ;
    int c = 0 ;
    foreach ( int col, cols ) {

        Role* role = _createColumnRole();
        _col_headers.insert(c,new QVariant);
        _col_roles.insert(c,role);

        QVariant name = _trickmodel->headerData(col,Qt::Horizontal,Param::Name);
        QVariant unit = _trickmodel->headerData(col,Qt::Horizontal,Param::Unit);
        QVariant type = _trickmodel->headerData(col,Qt::Horizontal,Param::Type);
        QVariant size = _trickmodel->headerData(col,Qt::Horizontal,Param::Size);
        setHeaderData(c,Qt::Horizontal,name,Qt::DisplayRole);
        setHeaderData(c,Qt::Horizontal,name,Qt::EditRole);
        setHeaderData(c,Qt::Horizontal,name,Param::Name);
        setHeaderData(c,Qt::Horizontal,unit,Param::Unit);
        setHeaderData(c,Qt::Horizontal,type,Param::Type);
        setHeaderData(c,Qt::Horizontal,size,Param::Size);

        c++;
    }
}

TrickCurveModel::~TrickCurveModel()
{
    // nothing to do since model is not owned by TrickCurveModel
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


// read only for now
bool TrickCurveModel::insertRows(int row, int count, const QModelIndex &pidx)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(pidx);
    return false;
}

// read only for now
bool TrickCurveModel::removeRows(int row, int count, const QModelIndex &pidx)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(pidx);
    return false;
}

// read only for now
bool TrickCurveModel::insertColumns(int column, int count,
                                   const QModelIndex &pidx)
{
    Q_UNUSED(column);
    Q_UNUSED(count);
    Q_UNUSED(pidx);
    return false;
}

// read only for now
bool TrickCurveModel::removeColumns(int column, int count, const QModelIndex &pidx)
{
    Q_UNUSED(column);
    Q_UNUSED(count);
    Q_UNUSED(pidx);
    return false;
}

Role* TrickCurveModel::_createColumnRole()
{
    Param* param = new Param;
    return param;
}


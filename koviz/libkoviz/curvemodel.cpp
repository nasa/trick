#include "curvemodel.h"

CurveModel::CurveModel() :
    _real(0),
    _imag(0),
    _datamodel(0),
    _t(0),
    _x(0),
    _y(0)
{}

CurveModel::CurveModel(DataModel *datamodel,
                       int tcol, int xcol, int ycol,
                       QObject *parent) :
    QAbstractTableModel(parent),
    _real(0),
    _imag(0),
    _datamodel(datamodel),
    _tcol(tcol),
    _xcol(xcol),
    _ycol(ycol),
    _t(new CurveModelParameter),
    _x(new CurveModelParameter),
    _y(new CurveModelParameter)
{
    _t->setName(_datamodel->param(_tcol)->name());
    _t->setUnit(_datamodel->param(_tcol)->unit());
    _x->setName(_datamodel->param(_xcol)->name());
    _x->setUnit(_datamodel->param(_xcol)->unit());
    _y->setName(_datamodel->param(_ycol)->name());
    _y->setUnit(_datamodel->param(_ycol)->unit());
}

CurveModel::~CurveModel()
{
    delete _t;
    delete _x;
    delete _y;

    if ( _real ) {
        free(_real);
        _real = 0;
    }
    if ( _imag ) {
        free(_imag);
        _imag = 0;
    }

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

QString CurveModel::fileName() const
{
    if ( !_datamodel ) {
        return "";
    }
    return _datamodel->fileName();
}

QString CurveModel::runPath() const
{
    if ( !_datamodel ) {
        return "";
    }
    return _datamodel->runPath();
}

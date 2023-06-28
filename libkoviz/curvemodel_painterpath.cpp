#include "curvemodel_painterpath.h"

CurveModelPainterPath::CurveModelPainterPath(QPainterPath *painterPath,
                                   const QString& xName, const QString& xUnit,
                                   const QString& yName, const QString& yUnit) :
    _painterPath(painterPath),
    _ncols(3),
    _nrows(0),
    _t(new CurveModelParameter),
    _x(new CurveModelParameter),
    _y(new CurveModelParameter),
    _iteratorTimeIndex(0)
{
    if ( xUnit != "s" ) {
        fprintf(stderr,"koviz [bad scoobs]: CurveModelPainterPath given curve "
                       "with xunit=%s.  X must be in seconds.\n",
                xUnit.toLatin1().constData());
        exit(-1);
    }

    _t->setName(xName);
    _t->setUnit(xUnit);
    _x->setName(xName);
    _x->setUnit(xUnit);
    _y->setName(yName);
    _y->setUnit(yUnit);

    _iteratorTimeIndex = new PainterPathModelIterator(this);
}

// See ~CurveModel() too
CurveModelPainterPath::~CurveModelPainterPath()
{
    if ( _iteratorTimeIndex ) {
        delete _iteratorTimeIndex;
        _iteratorTimeIndex = 0;
    }

    if ( _t ) delete _t;
    if ( _x ) delete _x;
    if ( _y ) delete _y;
}

ModelIterator* CurveModelPainterPath::begin() const
{
    return new PainterPathModelIterator(this);
}

int CurveModelPainterPath::indexAtTime(double time)
{
    return _idxAtTimeBinarySearch(_iteratorTimeIndex,0,rowCount()-1,time);
}

int CurveModelPainterPath::_idxAtTimeBinarySearch (PainterPathModelIterator* it,
                                       int low, int high, double time)
{
        if (high <= 0 ) {
                return 0;
        }
        if (low >= high) {
                // Time not found, choose closest near high
                double t1 = it->at(high-1)->t();
                double t2 = it->at(high)->t();
                double t3 = t2;
                it = it->at(high+1);
                if ( !it->isDone() ) {
                    t3 = it->at(high+1)->t();
                }

                int i;
                if ( qAbs(time-t1) < qAbs(time-t2) ) {
                    if ( qAbs(time-t1) < qAbs(time-t3) ) {
                        i = high-1;
                    } else {
                        if ( !it->isDone() ) {
                            i = high+1;
                        } else {
                            i = high;
                        }
                    }
                } else {
                    if ( qAbs(time-t2) < qAbs(time-t3) ) {
                        i = high;
                    } else {
                        if ( !it->isDone() ) {
                            i = high+1;
                        } else {
                            i = high;
                        }
                    }
                }
                return i;
        } else {
                int mid = (low + high)/2;
                if (time == it->at(mid)->t()) {
                        return mid;
                } else if ( time < it->at(mid)->t() ) {
                        return _idxAtTimeBinarySearch(it,
                                                      low, mid-1, time);
                } else {
                        return _idxAtTimeBinarySearch(it,
                                                      mid+1, high, time);
                }
        }
}

int CurveModelPainterPath::rowCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() ) {
        return _painterPath->elementCount();
    } else {
        return 0;
    }
}

int CurveModelPainterPath::columnCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() ) {
        return 3;
    } else {
        return 0;
    }
}

// For now, return empty QVariant
QVariant CurveModelPainterPath::data (const QModelIndex & index, int role ) const

{
    Q_UNUSED(index);
    Q_UNUSED(role);
    QVariant v;
    return v;
}

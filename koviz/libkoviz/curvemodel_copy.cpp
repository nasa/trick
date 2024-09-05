#include "curvemodel_copy.h"

CurveModelCopy::CurveModelCopy(CurveModel *curveModel) :
    _ncols(3),
    _nrows(0),
    _t(new CurveModelParameter),
    _x(new CurveModelParameter),
    _y(new CurveModelParameter),
    _iteratorTimeIndex(0)
{
    _iteratorTimeIndex = new CopyModelIterator(this);

    _fileName = curveModel->fileName();
    _t->setName(curveModel->t()->name());
    _t->setUnit(curveModel->t()->unit());
    _x->setName(curveModel->x()->name());
    _x->setUnit(curveModel->x()->unit());
    _y->setName(curveModel->y()->name());
    _y->setUnit(curveModel->y()->unit());

    _init(curveModel);
}

CurveModelCopy::~CurveModelCopy()
{
    // See: ~CurveModel()
}

ModelIterator* CurveModelCopy::begin() const
{
    return new CopyModelIterator(this);
}

int CurveModelCopy::indexAtTime(double time)
{
    return _idxAtTimeBinarySearch(_iteratorTimeIndex,0,rowCount()-1,time);
}

int CurveModelCopy::_idxAtTimeBinarySearch (CopyModelIterator* it,
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
                        return _idxAtTimeBinarySearch(it, mid+1, high, time);
                }
        }
}

int CurveModelCopy::rowCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() ) {
        return _nrows;
    } else {
        return 0;
    }
}

int CurveModelCopy::columnCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() ) {
        return 3;
    } else {
        return 0;
    }
}

// TODO CurveModelCopy::data() --- for now, return empty QVariant
QVariant CurveModelCopy::data (const QModelIndex & index, int role ) const

{
    Q_UNUSED(index);
    Q_UNUSED(role);
    QVariant v;
    return v;
}

void CurveModelCopy::_init(CurveModel* curveModel)
{
    curveModel->map();
    ModelIterator* it = curveModel->begin();

    _nrows = curveModel->rowCount();
    _data = (double*)malloc(_nrows*_ncols*sizeof(double));

    for (int i = 0; i < _nrows; ++i ) {
        _data[i*_ncols+0] = it->at(i)->t();
        _data[i*_ncols+1] = it->at(i)->x();
        _data[i*_ncols+2] = it->at(i)->y();
    }

    delete it;
    curveModel->unmap();
}

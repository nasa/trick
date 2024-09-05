#include "curvemodel_integ.h"

CurveModelIntegral::CurveModelIntegral(CurveModel *curveModel,
                                       double initial_value) :
    _ncols(3),
    _nrows(0),
    _t(new CurveModelParameter),
    _x(new CurveModelParameter),
    _y(new CurveModelParameter),
    _iteratorTimeIndex(0)
{
    if ( curveModel->x()->unit() != "s" ) {
        fprintf(stderr,"koviz [bad scoobs]: CurveModelIntegral given curve "
                       "with xunit=%s.  It must be in seconds.\n",
                curveModel->x()->unit().toLatin1().constData());
        exit(-1);
    }

    _iteratorTimeIndex = new IntegralModelIterator(this);

    _fileName = curveModel->fileName();
    _t->setName(curveModel->t()->name());
    _t->setUnit(curveModel->t()->unit());
    _x->setName(curveModel->x()->name());
    _x->setUnit(curveModel->x()->unit());

    QString yName = curveModel->y()->name();
    QChar integSymbol(8747);
    QString iYName;
    if (  yName.startsWith(integSymbol) ) {
        iYName = QString("%1%2").arg(integSymbol).arg(yName);
    } else {
        iYName = QString("%1(%2)").arg(integSymbol).arg(yName);
    }
    _y->setName(iYName);

    QString integUnit = Unit::integral(curveModel->y()->unit());
    _y->setUnit(integUnit);

    _init(curveModel,initial_value);
}

CurveModelIntegral::~CurveModelIntegral()
{
    // See: ~CurveModel()
}

ModelIterator* CurveModelIntegral::begin() const
{
    return new IntegralModelIterator(this);
}

int CurveModelIntegral::indexAtTime(double time)
{
    return _idxAtTimeBinarySearch(_iteratorTimeIndex,0,rowCount()-1,time);
}

int CurveModelIntegral::_idxAtTimeBinarySearch (IntegralModelIterator* it,
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

int CurveModelIntegral::rowCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() ) {
        return _nrows;
    } else {
        return 0;
    }
}

int CurveModelIntegral::columnCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() ) {
        return 3;
    } else {
        return 0;
    }
}

// TODO CurveModel::data() --- for now, return empty QVariant
QVariant CurveModelIntegral::data (const QModelIndex & index, int role ) const

{
    Q_UNUSED(index);
    Q_UNUSED(role);
    QVariant v;
    return v;
}

// Use trapezoids to estimate integral
void CurveModelIntegral::_init(CurveModel* curveModel, double initial_value)
{
    curveModel->map();
    ModelIterator* it = curveModel->begin();

    _nrows = curveModel->rowCount();
    _data = (double*)malloc(_nrows*_ncols*sizeof(double));

    if ( _nrows == 0 ) {
        // Nothing to do, empty
    } else if ( _nrows == 1 ) {
        _data[0*_ncols+0] = it->at(0)->t();
        _data[0*_ncols+1] = it->at(0)->x();
        _data[0*_ncols+2] = initial_value;
    } else if ( _nrows == 2 ) {
        double x0 = it->at(0)->x();
        double x1 = it->at(1)->x();
        double y0 = it->at(0)->y();
        double y1 = it->at(1)->y();
        double dx = x1-x0;
        double area = (y0+y1)*dx/2.0;
        _data[0*_ncols+0] = it->at(0)->t();
        _data[0*_ncols+1] = it->at(0)->x();
        _data[0*_ncols+2] = initial_value;
        _data[1*_ncols+0] = it->at(1)->t();
        _data[1*_ncols+1] = it->at(1)->x();
        _data[1*_ncols+2] = area;
    } else if ( _nrows == 3 ) {
        double x0 = it->at(0)->x();
        double y0 = it->at(0)->y();
        double x1 = it->at(1)->x();
        double y1 = it->at(1)->y();
        double x2 = it->at(2)->x();
        double y2 = it->at(2)->y();
        double a0 = (y0+y1)*(x1-x0)/2.0;
        double a1 = (y1+y2)*(x2-x1)/2.0;
        _data[0*_ncols+0] = it->at(0)->t();
        _data[0*_ncols+1] = it->at(0)->x();
        _data[0*_ncols+2] = initial_value;
        _data[1*_ncols+0] = it->at(1)->t();
        _data[1*_ncols+1] = it->at(1)->x();
        _data[1*_ncols+2] = a0;
        _data[2*_ncols+0] = it->at(2)->t();
        _data[2*_ncols+1] = it->at(2)->x();
        _data[2*_ncols+2] = a1;
    } else {
        for (int i = 0; i < _nrows; ++i ) {
            _data[i*_ncols+0] = it->at(i)->t();
            _data[i*_ncols+1] = it->at(i)->x();
            if ( i == 0 ) {
                _data[i*_ncols+2] = initial_value;
            } else {
                double x0 = it->at(i-1)->x();
                double y0 = it->at(i-1)->y();
                double x1 = it->at(i)->x();
                double y1 = it->at(i)->y();
                for (int j = i-1; j >= 0; --j) {
                    x0 = it->at(j)->x();
                    y0 = it->at(j)->y();
                    if ( !std::isnan(x0) && !std::isnan(y0) && x0 != x1 ) {
                        break;
                    }
                }
                double area = (y0+y1)*(x1-x0)/2.0;
                if ( x1 == x0 ) {
                    area = 0.0;
                }
                _data[i*_ncols+2] = _data[(i-1)*_ncols+2] + area;
            }
        }
    }

    delete it;
    curveModel->unmap();
}

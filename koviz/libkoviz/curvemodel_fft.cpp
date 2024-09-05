#include "curvemodel_fft.h"

CurveModelFFT::CurveModelFFT(CurveModel *curveModel,
                             double xb, double xs,
                             double begX, double endX) :
    _begX(begX),
    _endX(endX),
    _xb(xb),
    _xs(xs),
    _ncols(3),
    _nrows(0),
    _t(new CurveModelParameter),
    _x(new CurveModelParameter),
    _y(new CurveModelParameter),
    _iteratorTimeIndex(0)
{
    if ( curveModel->x()->unit() != "s" ) {
        fprintf(stderr,"koviz [bad scoobs]: CurveModelFFT given curve with "
                       "xunit=%s.  It must be in seconds.\n",
                curveModel->x()->unit().toLatin1().constData());
        exit(-1);
    }

    _iteratorTimeIndex = new FFTModelIterator(this);

    _fileName = curveModel->fileName();
    _t->setName("frequency");
    _t->setUnit("Hz");
    _x->setName("frequency");
    _x->setUnit("Hz");
    _y->setName(curveModel->y()->name());
    _y->setUnit(curveModel->y()->unit());

    _init(curveModel);
}

CurveModelFFT::~CurveModelFFT()
{
    // See: ~CurveModel()
}

ModelIterator* CurveModelFFT::begin() const
{
    return new FFTModelIterator(this);
}

int CurveModelFFT::indexAtTime(double time)
{
    return _idxAtTimeBinarySearch(_iteratorTimeIndex,0,rowCount()-1,time);
}

int CurveModelFFT::_idxAtTimeBinarySearch (FFTModelIterator* it,
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

int CurveModelFFT::rowCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() ) {
        return _nrows;
    } else {
        return 0;
    }
}

int CurveModelFFT::columnCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() ) {
        return 3;
    } else {
        return 0;
    }
}

// TODO CurveModel::data() --- for now, return empty QVariant
QVariant CurveModelFFT::data (const QModelIndex & index, int role ) const

{
    Q_UNUSED(index);
    Q_UNUSED(role);
    QVariant v;
    return v;
}

void CurveModelFFT::_init(CurveModel* curveModel)
{
    curveModel->map();

    int i = 0;
    int i0 = -1;
    int N = 0;
    ModelIterator* it = curveModel->begin();
    while ( !it->isDone() ) {
        if ( it->x()*_xs+_xb < _begX ) {
            it->next();
            ++i;
            continue;
        }
        if ( i0 == -1 ) {
            i0 = i;
        }
        if ( it->x()*_xs+_xb > _endX ) {
            break;
        }
        ++N;
        it->next();
    }

    if ( i0 == -1 || N < 2 ) {
        return;
    }

    double dt = 0;
    it = it->at(i0);
    double t = it->x()*_xs+_xb;
    it->next();
    while ( !it->isDone() ) {
        dt = it->x()*_xs+_xb - t;
        if ( dt > 0 ) {
            break;
        }
        it->next();
    }
    if ( dt == 0 ) {
        return;
    }

    _real = (double*)malloc(N*sizeof(double));
    _imag = (double*)malloc(N*sizeof(double));

    it = it->at(i0);
    double goodVal = 0.0;
    while ( !it->isDone() ) {
        if ( std::isnan(it->y()) ) {
            it->next();
            continue;
        }
        goodVal = it->y();
        break;
    }

    double ys = 1.0;
    double yb = 0.0;
    i = 0;
    it = it->at(i0);
    while ( !it->isDone() && i < N ) {
        _real[i] = it->y()*ys+yb;
        if ( std::isnan(_real[i]) ) {
            _real[i] = goodVal;
        }
        _imag[i] = 0.0;
        goodVal = _real[i];
        it->next();
        ++i;
    }
    delete it;

    Fft_transform(_real,_imag,N);

    _nrows = N;
    _data = (double*)malloc(_nrows*_ncols*sizeof(double));

    for ( i = 0 ; i < _nrows; ++i ) {
        double f = (1/dt)*i/N;
        double m = qSqrt(_real[i]*_real[i]+_imag[i]*_imag[i]);
        _data[i*_ncols+0] = f;
        _data[i*_ncols+1] = f;
        _data[i*_ncols+2] = m;
    }

    curveModel->unmap();
}

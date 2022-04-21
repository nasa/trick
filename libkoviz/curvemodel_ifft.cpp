#include "curvemodel_ifft.h"

CurveModelIFFT::CurveModelIFFT(CurveModel *curveModel,
                             double begTime, double endTime) :
    _begTime(begTime),
    _endTime(endTime),
    _ncols(3),
    _nrows(0),
    _t(new CurveModelParameter),
    _x(new CurveModelParameter),
    _y(new CurveModelParameter),
    _iteratorTimeIndex(0)
{
    if ( curveModel->x()->unit() != "Hz" ) {
        fprintf(stderr,"koviz [bad scoobs]: CurveModelIFFT given curve with "
                       "xunit=%s.  It must be in Hz for ifft.\n",
                curveModel->x()->unit().toLatin1().constData());
        exit(-1);
    }

    _fileName = curveModel->fileName();
    _t->setName("time");
    _t->setUnit("s");
    _x->setName("time");
    _x->setUnit("s");
    _y->setName(curveModel->y()->name());
    _y->setUnit(curveModel->y()->unit());

    _iteratorTimeIndex = new IFFTModelIterator(this);

    if ( curveModel->_real != 0 && curveModel->_imag != 0 ) {
        _init(curveModel);
    } else {
        // IFFT is empty - so nothing to do
    }
}

// See ~CurveModel() too
CurveModelIFFT::~CurveModelIFFT()
{
    if ( _iteratorTimeIndex ) {
        delete _iteratorTimeIndex;
        _iteratorTimeIndex = 0;
    }
}

ModelIterator* CurveModelIFFT::begin() const
{
    return new IFFTModelIterator(this);
}

int CurveModelIFFT::indexAtTime(double time)
{
    return _idxAtTimeBinarySearch(_iteratorTimeIndex,0,rowCount()-1,time);
}

int CurveModelIFFT::_idxAtTimeBinarySearch (IFFTModelIterator* it,
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

int CurveModelIFFT::rowCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() ) {
        return _nrows;
    } else {
        return 0;
    }
}

int CurveModelIFFT::columnCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() ) {
        return 3;
    } else {
        return 0;
    }
}

// TODO CurveModel::data() --- for now, return empty QVariant
QVariant CurveModelIFFT::data (const QModelIndex & index, int role ) const

{
    Q_UNUSED(index);
    Q_UNUSED(role);
    QVariant v;
    return v;
}

void CurveModelIFFT::_init(CurveModel* curveModel)
{
    curveModel->map();

    int N = curveModel->rowCount();
    if ( N < 2 ) {
        fprintf(stderr, "koviz [bad scoobs]: Attempting to take inverse FFT of"
                "data with less than 2 points\n");
        exit(-1);
    }
    _nrows = N;

    _real = (double*)malloc(N*sizeof(double));
    for ( int i = 0; i < N; ++i ) {
        _real[i] = curveModel->_real[i];
    }
    _imag = (double*)malloc(N*sizeof(double));
    for ( int i = 0; i < N; ++i ) {
        _imag[i] = curveModel->_imag[i];
    }

    Fft_inverseTransform(_real,_imag,N);

    _data = (double*)malloc(_nrows*_ncols*sizeof(double));

    ModelIterator* it = curveModel->begin();
    double dt = 1/(it->at(1)->x()*N);
    delete it;

    for ( int i = 0 ; i < _nrows; ++i ) {
        _data[i*_ncols+0] = dt*i;
        _data[i*_ncols+1] = dt*i;
        _data[i*_ncols+2] = _real[i]/N;
    }

    // Clear cache
    free(_real);
    free(_imag);
    _real = 0;
    _imag = 0;

    curveModel->unmap();
}

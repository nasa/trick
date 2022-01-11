#include "curvemodel_fft.h"

CurveModelFFT::CurveModelFFT(CurveModel *curveModel,
                             double begTime, double endTime) :
    _begTime(begTime),
    _endTime(endTime),
    _ncols(3),
    _nrows(0),
    _t(new CurveModelParameter),
    _x(new CurveModelParameter),
    _y(new CurveModelParameter)
{
    if ( curveModel->x()->unit() != "s" ) {
        fprintf(stderr,"koviz [bad scoobs]: CurveModelFFT given curve with "
                       "xunit=%s.  It must be in seconds.\n",
                curveModel->x()->unit().toLatin1().constData());
        exit(-1);
    }

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
    fprintf(stderr, "CurveModelFFT::indexAtTime() not implemented ("
                    "time=%g)\n", time);
    exit(-1);
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

    int N = 0;
    int i0 = curveModel->indexAtTime(_begTime);
    ModelIterator* it = curveModel->begin();
    it = it->at(i0);
    while ( !it->isDone() ) {
        if ( it->t() > _endTime ) {
            break;
        }
        ++N;
        it->next();
    }

    _real = (double*)malloc(N*sizeof(double));
    _imag = (double*)malloc(N*sizeof(double));

    double dt = 0;
    it = it->at(i0);
    double t = it->t();
    it->next();
    while ( !it->isDone() ) {
        dt = it->t() - t;
        if ( dt > 0 ) {
            break;
        }
        it->next();
    }
    if ( dt == 0 ) {
        fprintf(stderr, "koviz [error]: dt=0.0, cannot take fft\n");
        exit(-1);
    }

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
    int i = 0;
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

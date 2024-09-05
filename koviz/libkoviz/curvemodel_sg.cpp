#include "curvemodel_sg.h"

CurveModelSG::CurveModelSG(CurveModel *curveModel,int window,int degree) :
    _window(window),
    _degree(degree),
    _ncols(3),
    _nrows(0),
    _t(new CurveModelParameter),
    _x(new CurveModelParameter),
    _y(new CurveModelParameter),
    _iteratorTimeIndex(0)
{
    if ( curveModel->x()->unit() != "s" ) {
        fprintf(stderr,"koviz [bad scoobs]: CurveModelSG given curve with "
                       "xunit=%s.  It must be in seconds.\n",
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

    _iteratorTimeIndex = new SGModelIterator(this);

    _init(curveModel);
}

// See ~CurveModel() too
CurveModelSG::~CurveModelSG()
{
    if ( _iteratorTimeIndex ) {
        delete _iteratorTimeIndex;
        _iteratorTimeIndex = 0;
    }
}

ModelIterator* CurveModelSG::begin() const
{
    return new SGModelIterator(this);
}

int CurveModelSG::indexAtTime(double time)
{
    return _idxAtTimeBinarySearch(_iteratorTimeIndex,0,rowCount()-1,time);
}

int CurveModelSG::_idxAtTimeBinarySearch (SGModelIterator* it,
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

int CurveModelSG::rowCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() ) {
        return _nrows;
    } else {
        return 0;
    }
}

int CurveModelSG::columnCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() ) {
        return 3;
    } else {
        return 0;
    }
}

// TODO CurveModel::data() --- for now, return empty QVariant
QVariant CurveModelSG::data (const QModelIndex & index, int role ) const

{
    Q_UNUSED(index);
    Q_UNUSED(role);
    QVariant v;
    return v;
}

void CurveModelSG::_init(CurveModel* curveModel)
{
    curveModel->map();

    // Calculate N and dt
    int N = 0;
    double dt = 0.0;
    bool isFirst = true;
    double lastTime = 0.0;
    double lastDt = 0.0;
    ModelIterator* it = curveModel->begin();
    double beginTime = it->t();
    while ( !it->isDone() ) {
        if ( isFirst ) {
            isFirst = false;
        } else {
            dt = it->t() - lastTime;
            if ( lastDt > 0.0 && dt > 0.0 && qAbs(dt-lastDt) > 1.0e-9 ) {
                fprintf(stderr, "koviz [bad scoobs]: S-Golay filter expects"
                        " uniform sampling frequency.  Data has variable dt.  "
                        "Bailing!\n");
                exit(-1);
            } else if ( dt == 0.0 ) {
                fprintf(stderr, "koviz [error]: S-Golay filter expects "
                        "uniform sampling frequency.  Data has two values with "
                        "same time stamp.  Bailing!\n");
                exit(-1);
            }
            lastDt = dt;
        }
        lastTime = it->t();
        ++N;
        it->next();
    }

    if ( curveModel->_real == 0 ) {
        fprintf(stderr, "koviz [bad scoobs]: CurveModelSG::_init() should "
               "never encounter a curveModel->_real==0x0.  _real should be "
               "allocated prior to this for performance reasons.  _real is a "
               "cache for holding unchanging data from sg window to window.\n");
        exit(-1);
    }
    _real = curveModel->_real;

    _nrows = N;
    double *buf = (double*)malloc(_nrows*sizeof(double));
    for (int i = 0; i < N; ++i) {
        buf[i] = _real[i];
    }
    buf = calc_sgsmooth(_nrows, buf, _window, _degree);
    _data = (double*)malloc(_nrows*_ncols*sizeof(double));
    for ( int i = 0; i < N; ++i ) {
        _data[i*_ncols+0] = beginTime+dt*i;
        _data[i*_ncols+1] = beginTime+dt*i;
        _data[i*_ncols+2] = buf[i];
    }
    free(buf);

    delete it;
    curveModel->unmap();
}

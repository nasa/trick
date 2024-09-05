#include "curvemodel_deriv.h"

CurveModelDerivative::CurveModelDerivative(CurveModel *curveModel) :
    _ncols(3),
    _nrows(0),
    _t(new CurveModelParameter),
    _x(new CurveModelParameter),
    _y(new CurveModelParameter),
    _iteratorTimeIndex(0)
{
    if ( curveModel->x()->unit() != "s" ) {
        fprintf(stderr,"koviz [bad scoobs]: CurveModelDerivative given curve "
                       "with xunit=%s.  It must be in seconds.\n",
                curveModel->x()->unit().toLatin1().constData());
        exit(-1);
    }

    _iteratorTimeIndex = new DerivativeModelIterator(this);

    _fileName = curveModel->fileName();
    _t->setName(curveModel->t()->name());
    _t->setUnit(curveModel->t()->unit());
    _x->setName(curveModel->x()->name());
    _x->setUnit(curveModel->x()->unit());
    QString yName = "d'(" + curveModel->y()->name() + ")";
    _y->setName(yName);

    QString dUnit = Unit::derivative(curveModel->y()->unit());
    _y->setUnit(dUnit);

    _init(curveModel);
}

CurveModelDerivative::~CurveModelDerivative()
{
    // See: ~CurveModel()
}

ModelIterator* CurveModelDerivative::begin() const
{
    return new DerivativeModelIterator(this);
}

int CurveModelDerivative::indexAtTime(double time)
{
    return _idxAtTimeBinarySearch(_iteratorTimeIndex,0,rowCount()-1,time);
}

int CurveModelDerivative::_idxAtTimeBinarySearch (DerivativeModelIterator* it,
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

int CurveModelDerivative::rowCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() ) {
        return _nrows;
    } else {
        return 0;
    }
}

int CurveModelDerivative::columnCount(const QModelIndex &pidx) const
{
    if ( !pidx.isValid() ) {
        return 3;
    } else {
        return 0;
    }
}

// TODO CurveModel::data() --- for now, return empty QVariant
QVariant CurveModelDerivative::data (const QModelIndex & index, int role ) const

{
    Q_UNUSED(index);
    Q_UNUSED(role);
    QVariant v;
    return v;
}

void CurveModelDerivative::_init(CurveModel* curveModel)
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
        _data[0*_ncols+2] = 0.0;
    } else if ( _nrows == 2 ) {
        double x0 = it->at(0)->x();
        double x1 = it->at(1)->x();
        double y0 = it->at(0)->y();
        double y1 = it->at(1)->y();
        double m = (y1-y0)/(x1-x0);
        if ( x1 == x0 ) {
            m = 0;
        }
        _data[0*_ncols+0] = it->at(0)->t();
        _data[0*_ncols+1] = it->at(0)->x();
        _data[0*_ncols+2] = m;
        _data[1*_ncols+0] = it->at(1)->t();
        _data[1*_ncols+1] = it->at(1)->x();
        _data[1*_ncols+2] = m;
    } else if ( _nrows == 3 ) {
        double x0 = it->at(0)->x();
        double y0 = it->at(0)->y();
        double x1 = it->at(1)->x();
        double y1 = it->at(1)->y();
        double x2 = it->at(2)->x();
        double y2 = it->at(2)->y();
        double m1 = (y1-y0)/(x1-x0);
        double m2 = (y2-y1)/(x2-x1);
        if ( x1 == x0 ) {
            m1 = 0;
        }
        if ( x2 == x1 ) {
            m2 = 0;
        }
        _data[0*_ncols+0] = it->at(0)->t();
        _data[0*_ncols+1] = it->at(0)->x();
        _data[0*_ncols+2] = m1;
        _data[1*_ncols+0] = it->at(1)->t();
        _data[1*_ncols+1] = it->at(1)->x();
        _data[1*_ncols+2] = (m1+m2)/2;
        _data[2*_ncols+0] = it->at(2)->t();
        _data[2*_ncols+1] = it->at(2)->x();
        _data[2*_ncols+2] = m2;
    } else {
        for (int i = 0; i < _nrows; ++i ) {
            _data[i*_ncols+0] = it->at(i)->t();
            _data[i*_ncols+1] = it->at(i)->x();
            if ( i == 0 ) {
                // Initial point
                double x0 = it->at(0)->x();
                double y0 = it->at(0)->y();
                double x1 = it->at(1)->x();
                double y1 = it->at(1)->y();
                double x2 = it->at(2)->x();
                double y2 = it->at(2)->y();
                double x3 = it->at(3)->x();
                double y3 = it->at(3)->y();

                int j;
                for (j = 1; j < _nrows; ++j) {
                    it = it->at(j);
                    if ( !it->isDone() ) {
                        x1 = it->at(j)->x();
                        y1 = it->at(j)->y();
                        if ( !std::isnan(x1) && !std::isnan(y1) && x0 != x1 ) {
                            break;
                        }
                    }
                }

                for (j = j+1; j < _nrows; ++j) {
                    it = it->at(j);
                    if ( !it->isDone() ) {
                        x2 = it->at(j)->x();
                        y2 = it->at(j)->y();
                        if ( !std::isnan(x2) && !std::isnan(y2) && x1 != x2 ) {
                            break;
                        }
                    }
                }

                for (j = j+1; j < _nrows; ++j) {
                    it = it->at(j);
                    if ( !it->isDone() ) {
                        x3 = it->at(j)->x();
                        y3 = it->at(j)->y();
                        if ( !std::isnan(x3) && !std::isnan(y3) && x2 != x3 ) {
                            break;
                        }
                    }
                }

                if ( !std::isnan(x0) && !std::isnan(y0) &&
                     !std::isnan(x1) && !std::isnan(y1) &&
                     !std::isnan(x2) && !std::isnan(y2) &&
                     !std::isnan(x3) && !std::isnan(y3) ) {

                    double m0 = (y1-y0)/(x1-x0);
                    double m1 = (y2-y1)/(x2-x1);
                    double m2 = (y3-y2)/(x3-x2);

                    double v1 = (m0+m1)/2.0;
                    double v2 = (m1+m2)/2.0;

                    double m = (v2-v1)/(x2-x1);

                    _data[i*_ncols+2] = m*(x0-x1)+v1;
                } else {
                    double m = (y1-y0)/(x1-x0);
                    if ( x0 == x1 ) {
                        m = 0.0;
                    }
                    _data[i*_ncols+2] = m;
                }


            } else if ( i == _nrows-1 ) {
                // Last point
                double x0 = it->at(i-3)->x();
                double y0 = it->at(i-3)->y();
                double x1 = it->at(i-2)->x();
                double y1 = it->at(i-2)->y();
                double x2 = it->at(i-1)->x();
                double y2 = it->at(i-1)->y();
                double x3 = it->at(i)->x();
                double y3 = it->at(i)->y();

                int j;
                for (j = i-1; j >= 0; --j) {
                    x2 = it->at(j)->x();
                    y2 = it->at(j)->y();
                    if ( !std::isnan(x2) && !std::isnan(y2) && x2 != x3 ) {
                        break;
                    }
                }

                for (j = j-1; j >= 0; --j) {
                    x1 = it->at(j)->x();
                    y1 = it->at(j)->y();
                    if ( !std::isnan(x1) && !std::isnan(y1) && x1 != x2 ) {
                        break;
                    }
                }

                for (j = j-1; j >= 0; --j) {
                    x0 = it->at(j)->x();
                    y0 = it->at(j)->y();
                    if ( !std::isnan(x0) && !std::isnan(y0) && x0 != x1 ) {
                        break;
                    }
                }

                if ( !std::isnan(x0) && !std::isnan(y0) &&
                     !std::isnan(x1) && !std::isnan(y1) &&
                     !std::isnan(x2) && !std::isnan(y2) &&
                     !std::isnan(x3) && !std::isnan(y3) ) {

                    double m0 = (y1-y0)/(x1-x0);
                    double m1 = (y2-y1)/(x2-x1);
                    double m2 = (y3-y2)/(x3-x2);

                    double v1 = (m0+m1)/2.0;
                    double v2 = (m1+m2)/2.0;

                    double m = (v2-v1)/(x2-x1);

                    _data[i*_ncols+2] = m*(x3-x2)+v2;
                } else {
                    double m = (y3-y2)/(x3-x2);
                    if ( x3 == x2 ) {
                        m = 0.0;
                    }
                    _data[i*_ncols+2] = m;
                }

            } else {
                // All other points (avg slope before and after)
                double x0 = it->at(i-1)->x();
                double x1 = it->at(i)->x();
                double x2 = it->at(i+1)->x();
                double y0 = it->at(i-1)->y();
                double y1 = it->at(i)->y();
                double y2 = it->at(i+1)->y();
                for (int j = i-1; j >= 0; --j) {
                    x0 = it->at(j)->x();
                    y0 = it->at(j)->y();
                    if ( !std::isnan(x0) && !std::isnan(y0) && x0 != x1 ) {
                        break;
                    }
                }
                for (int j = i+1; j < _nrows; ++j) {
                    it = it->at(j);
                    if ( !it->isDone() ) {
                        x2 = it->at(j)->x();
                        y2 = it->at(j)->y();
                        if ( !std::isnan(x2) && !std::isnan(y2) && x1 != x2 ) {
                            break;
                        }
                    }
                }
                double m0 = (y1-y0)/(x1-x0);
                if ( x1 == x0 ) {
                    m0 = 0.0;
                }
                double m1 = (y2-y1)/(x2-x1);
                if ( x1 == x2 ) {
                    m1 = 0.0;
                }
                double m = (m0+m1)/2.0;
                if ( std::isnan(m0) && !std::isnan(m1) ) {
                    m = m1;
                } else if ( !std::isnan(m0) && std::isnan(m1) ) {
                    m = m0;
                }
                _data[i*_ncols+2] = m;
            }
        }
    }

    delete it;
    curveModel->unmap();
}

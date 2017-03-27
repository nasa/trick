#include "timestamps.h"
#include "roundoff.h"

int TimeStamps::_lastIdx(0);

static bool doubleLessThan(double a, double b)
{
    return a-b < 1.0e-9;
}


// TODO: Fix this dumb way
// If xs = (0.01, 0.010000010) this takes a long time
TimeStamps::TimeStamps(QList<double> freqs, double stopTime, QObject *parent)
  : QObject(parent)
{
    _lastIdx = 0;

    QList<double> ufreqs(freqs);
    usort(ufreqs);
    double lcm = leastCommonMultiple(ufreqs);

    foreach ( double f, freqs ) {
        QList<double> ts;
        double k = 1.0;
        while ( 1 ) {
            double t = k*f;
            ROUNDOFF(t,t);
            if ( t > (lcm+1.0e-9) ) {
                break;
            }
            ts << t;
            k = k + 1.0;
            ROUNDOFF(k,k);
        }
        _timeStampsToLCM = merge(_timeStampsToLCM,ts);
    }

    double zero = 0.0;
    ROUNDOFF(zero,zero);
    _timeStamps << zero;

    while ( 1 ) {
        bool isFinished = false;
        double lastTime = _timeStamps.last();
        foreach ( double t, _timeStampsToLCM ) {
            double nt = t + lastTime;
            ROUNDOFF(nt,nt);
            if ( nt > stopTime+1.0e-9 ) {
                isFinished = true;
                break;
            }
            _timeStamps << nt;
        }
        if ( isFinished ) {
            break;
        }
    }
}

double TimeStamps::at(int i )
{
    if ( i >= 0 && i < _timeStamps.size() ) {
        return _timeStamps.at(i);
    } else if ( i >= _timeStamps.size() ) {
        return _timeStamps.last();
    } else if ( i < 0 ) {
        return _timeStamps.at(0);
    }

    return 0.0;
}


// TODO: Fix this dumb way
// If xs = (0.01, 0.010000010) this takes a long time
double TimeStamps::leastCommonMultiple(const QList<double>& xs)
{
    double lcm = 0.0;

    if ( xs.size() == 0 ) {
        return 0.0;
    }
    if ( xs.size() == 1 ) {
        return xs.at(0);
    }

    QList<double> ks;
    foreach ( double x, xs ) {
        Q_UNUSED(x);
        ks << 1.0;
    }

    while ( 1 ) {

        int i = 0;
        double max = -1.0e20;
        foreach ( double k, ks ) {
            double x = xs.at(i);
            if ( k*x > max ) {
                max = k*x;
            }
            ++i;
        }

        bool isDone = true;
        for ( int i = 0 ; i < ks.size(); ++i ) {
            double k = ks.at(i);
            double x = xs.at(i);
            if ( max - k*x > 1.0e-9 ) {
                isDone = false;
                k = k + 1.0;
                ROUNDOFF(k,k);
                ks[i] = k;
            }
        }

        if ( isDone ) {
            lcm = max;
            ROUNDOFF(lcm,lcm);
            break;
        }
    }


    return lcm;
}

QList<double> TimeStamps::merge(const QList<double>& listA,
                                const QList<double>& listB)
{
    QList<double> list = listA + listB;
    usort(list);
    return list;
}

void TimeStamps::insert(double t, QList<double> &list)
{
    if ( list.isEmpty() ) {
        list.append(t);
        return;
    }

    if ( list.count() == 1 ) {
        double t0 = list.at(0);
        if ( t < t0-1.0e-9 ) {
            list.prepend(t);
        } else if ( t > t0+1.0e-9 ) {
            list.append(t);
        }
        return;
    }

    int i = idxAtTime(list,t);
    double ti = list.at(i);
    if ( qAbs(t-ti) > 1.0e-9 ) {
        // Time is not in list, so insert
        if ( i == 0 ) {
            double t0 = list.at(0);
            if ( t < t0-1.0e-9 ) {
                list.prepend(t);
            } else {
                list.insert(1,t);  // list size >= 2 (from checks above)
            }
        } else {
            list.insert(i+1,t);
        }
    } else {
        // Nothing to insert since it is already inserted
    }
}

// Returns -1 if time below list
// Returns list.count() if time >= last time
int TimeStamps::idxAtTime(const QList<double> &list, double time)
{
    if ( list.isEmpty() ) return -1;
    int rc = list.size();
    if ( rc > 0 && list.at(rc-1) < time-1.0e-9 ) {
        return rc-1;
    }
    if ( _lastIdx+1 < rc ) {
        if ( qAbs(list.at(_lastIdx+1)-time) < 1.0e-9 ) {
            ++_lastIdx;
            return _lastIdx;
        }
    }

    _lastIdx = _idxAtTimeBinarySearch(list,0,rc,time);

    return _lastIdx;
}

int TimeStamps::_idxAtTimeBinarySearch(const QList<double>& list,
                                       int low, int high, double time)
{
        if (high <= 0 ) {
                return 0;
        }
        if (low >= high) {
                return ( list.at(high) > time ) ? high-1 : high;
        } else {
                int mid = (low + high)/2;
                if (time == list.at(mid)) {
                        return mid;
                } else if ( time < list.at(mid) ) {
                        return _idxAtTimeBinarySearch(list,
                                                      low, mid-1, time);
                } else {
                        return _idxAtTimeBinarySearch(list,
                                                      mid+1, high, time);
                }
        }
}

void TimeStamps::usort(QList<double>& list)
{
    qSort(list.begin(),list.end(),doubleLessThan);
    int rc = list.count();
    for ( int i = rc-2; i >= 0; --i ) {
        double a = list.at(i);
        double b = list.at(i+1);
        if ( qAbs(b-a) < 1.0e-9 ) {
            list.removeAt(i+1);
        }
    }
}

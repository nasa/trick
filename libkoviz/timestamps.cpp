#include "timestamps.h"
#include "roundoff.h"

int TimeStamps::_lastIdx(0);
double TimeStamps::epsilon(5.391245e-44);  // Planck time is arbitrary

static bool doubleLessThan(double a, double b)
{
    return a-b < TimeStamps::epsilon;
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
        if ( t < t0-TimeStamps::epsilon ) {
            list.prepend(t);
        } else if ( t > t0+TimeStamps::epsilon ) {
            list.append(t);
        }
        return;
    }

    int i = idxAtTime(list,t);
    double ti = list.at(i);
    if ( qAbs(t-ti) > TimeStamps::epsilon ) {
        // Time is not in list, so insert
        if ( i == 0 ) {
            double t0 = list.at(0);
            if ( t < t0-TimeStamps::epsilon ) {
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
    if ( rc > 0 && list.at(rc-1) < time-TimeStamps::epsilon ) {
        return rc-1;
    }
    if ( _lastIdx+1 < rc ) {
        if ( qAbs(list.at(_lastIdx+1)-time) < TimeStamps::epsilon ) {
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
    std::sort(list.begin(),list.end(),doubleLessThan);
    int rc = list.count();
    for ( int i = rc-2; i >= 0; --i ) {
        double a = list.at(i);
        double b = list.at(i+1);
        if ( qAbs(b-a) < TimeStamps::epsilon ) {
            list.removeAt(i+1);
        }
    }
}

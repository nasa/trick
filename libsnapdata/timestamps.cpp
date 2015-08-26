#include "timestamps.h"
#include "roundoff.h"

static bool doubleLessThan(double a, double b)
{
    return a-b < 1.0e-9;
}


// TODO: Fix this dumb way
// If xs = (0.01, 0.010000010) this takes a long time
TimeStamps::TimeStamps(QList<double> freqs, double stopTime, QObject *parent)
  : QObject(parent)
{
    QList<double> ufreqs = uniquify(freqs);
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
    list = uniquify(list);
    qSort(list.begin(),list.end(),doubleLessThan);
    return list;
}

QList<double> TimeStamps::uniquify(const QList<double>& list)
{
    QList<double> ulist;
    foreach ( double v, list ) {
        bool isEqual = false;
        foreach ( double u, ulist ) {
            if ( qAbs(v-u) < 1.0e-9 ) {
                isEqual = true;
                break;
            }
        }
        if ( !isEqual ) {
            ulist << v;
        }
    }
    return ulist;
}

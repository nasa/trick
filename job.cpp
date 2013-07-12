#include "job.h"

#include <QRegExp>
#include <stdio.h>
#include <cmath>

#include "utils.h"

bool jobAvgTimeGreaterThan(Job* a,Job* b)
{
    return a->avg_runtime() > b->avg_runtime();
}

bool jobMaxTimeGreaterThan(Job* a,Job* b)
{
    return a->max_runtime() > b->max_runtime();
}


QString Job::sim_object_name() const
{
    QString simobj;

    int idx = 0;
    if ( _job_name.contains(QString("##")) ) {
        idx = _job_name.indexOf(QChar('#'));
    } else {
        idx = _job_name.indexOf(QChar('.'));
    }
    if ( idx > 0 ) {
        simobj = _job_name.mid(0,idx);
    }

    return simobj;
}

inline void Job::_do_stats()
{
    if ( _is_stats ) {
        return ;
    } else {
        _is_stats  = true;
    }

    if ( _npoints == 0 || _timestamps == 0 || _runtime == 0 ) {
        fprintf(stderr,"Job::_do_stats() called without setting:");
        fprintf(stderr," npoints or timestamps or runtime");
        fprintf(stderr," which come from TrickBinaryRiver\n");
        exit(-1);
    }

    long freq;
    QMap<long,int> map_freq;
    long last_nonzero_timestamp = 0 ;

    long sum_rt = 0 ;
    long max_rt = 0 ;
    double sum_vv = 0.0;
    for ( int tidx = 0 ; tidx < _npoints ; tidx++ ) {
        double time = _timestamps[tidx];
        if ( time < 1.0 ) {
            continue;
        }
        long rt = (long)_runtime[tidx];

        if ( tidx > 0 && rt > 0 ) {
            freq = round_10((long)(time*1000000.0) - last_nonzero_timestamp);
            long freq_cnt ;
            if ( map_freq.contains(freq) ) {
                freq_cnt = map_freq.value(freq)+1;
            } else {
                freq_cnt = 0;
            }
            map_freq.insert(freq,freq_cnt);
            last_nonzero_timestamp = (long)(time*1000000.0);
        }

        if ( rt > max_rt ) {
            max_rt = rt;
            _max_timestamp = time;
        }
        sum_rt += rt;

        _avg_runtime = ((double)sum_rt/(double)_npoints);
        sum_vv += (rt-_avg_runtime)*(rt-_avg_runtime); // for stddev
    }

    _avg_runtime = (sum_rt/_npoints)/1000000.0;
    _max_runtime = (max_rt)/1000000.0;
    _stddev_runtime = sqrt(sum_vv/_npoints)/1000000.0 ;

    // Could be multiple frequencies - choose mode
    int max_cnt = 0 ;
    _freq = 0;
    foreach ( long freq, map_freq.keys() ) {
        int cnt = map_freq.value(freq);
        if ( cnt > max_cnt ) {
            _freq = freq/1000000.0;
            max_cnt = cnt;
        }
    }
}

double Job::avg_runtime()
{
    _do_stats();
    return _avg_runtime;
}

double Job::max_runtime()
{
    _do_stats();
    return _max_runtime;
}

double Job::max_timestamp()
{
    _do_stats();
    return _max_timestamp;
}

double Job::stddev_runtime()
{
    _do_stats();
    return _stddev_runtime;
}

double Job::freq()
{
    _do_stats();
    return _freq;
}

// Parse long logname and set job members accordingly
// An example logname:
// JOB_schedbus.SimBus##read_ALDS15_ObcsRouter_C1.1828.00(read_simbus_0.100)
Job::Job(BoundedTrickBinaryRiver *river, const char* log_jobname) :
     _is_stats(false)
{
    _npoints = river->getNumPoints();
    _timestamps = river->getTimeStamps();
    _runtime = river->getVals(log_jobname);

    QString qname(log_jobname);
    qname.replace("::","##");
    qname.replace(QRegExp("^JOB_"),"");
    int idx1 = qname.lastIndexOf (QChar('('));
    int idx2 = qname.lastIndexOf (QChar(')'));
    int idx3 = qname.lastIndexOf (QChar('_'));
    if ( (idx3 > idx1) && qname.at(idx3+1).isDigit() ) {
        // frequency specified e.g. (read_simbus_0.100)
        _job_class = qname.mid(idx1+1,idx3-idx1-1);
        _freq = qname.mid(idx3+1,idx2-idx3-1).toDouble();
    } else {
        // frequency not specified e.g. (derivative)
        _job_class = qname.mid(idx1+1,idx2-idx1-1);
        _freq = -1.0;
    }

    // e.g. 1828.00 from example name
    int idx4 = qname.lastIndexOf(QChar('.'),idx1);
    int idx5 = qname.lastIndexOf(QChar('.'),idx4-1);
    _job_num = qname.mid(idx5+1,idx1-idx5-1);

    // child/thread id
    _thread_id = 0 ;
    QString stid;
    int idx6;
    for ( idx6 = idx5-1 ; idx6 > 0 ; idx6-- ) {
        if ( isdigit(qname.at(idx6).toAscii()) ) {
            stid.prepend(qname.at(idx6));
        } else {
            if ( qname.at(idx6) == 'C' && qname.at(idx6-1) == '_' ) {
                _thread_id = stid.toInt();
                idx6--;
            } else {
                idx6++;
            }
            break;
        }
    }

    _job_name = qname.mid(0,idx6);
}

QString Job::id() const
{
    char buf[256];                     // freq rouned to 3 decimal places
    QString logname = "JOB_";
    logname += _job_name;
    logname = logname.replace("::","##");

     // hope it is thread_id and not processor id (TODO)
    if ( _thread_id != 0 ) {
        logname += "_C";
        QString tid = QString("%1").arg(_thread_id);
        logname += tid;
    }

    logname += ".";
    logname += _job_num;
    logname += "(";
    logname += _job_class;
    if ( _freq > -0.5 ) {
        // Some jobs have no freqency spec in the id
        logname += "_";
        sprintf(buf,"%.3lf",_freq);
        logname += QString(buf);
    }
    logname += ")";

    return logname;
}


#include "job.h"

#include <QRegExp>
#include <stdio.h>
#include <cmath>
#include <QtCore/qmath.h>

#include "utils.h"

bool jobAvgTimeGreaterThan(Job* a,Job* b)
{
    return a->avg_runtime() > b->avg_runtime();
}

bool jobMaxTimeGreaterThan(Job* a,Job* b)
{
    return a->max_runtime() > b->max_runtime();
}

QString Job::_err_string;
QTextStream Job::_err_stream(&Job::_err_string);

QString Job::sim_object_name() const
{
    QString simobj;

    int idx = 0;
    if ( _job_name.contains(QString("##")) ) {
        idx = _job_name.indexOf(QChar('#'));
    } else if ( _job_name.startsWith("frame_userjobs_C") ) {
        simobj = "frame_log";
    } else {
        idx = _job_name.indexOf(QChar('.'));
    }
    if ( idx > 0 ) {
        simobj = _job_name.mid(0,idx);
    }

    qDebug() << "job=" << _job_name << "so=" << simobj;
    return simobj;
}

inline void Job::_do_stats()
{
    if ( _is_stats ) {
        return ;
    } else {
        _is_stats  = true;
    }

    if ( ! _curve ) {
        // TODO: throw exception
        fprintf(stderr,"snap [error]: Job::_do_stats() called without setting:");
        fprintf(stderr," curve\n");
        exit(-1);
    }

    long freq;
    QMap<long,int> map_freq;
    long last_nonzero_timestamp = 0 ;

    long sum_squares = 0 ;
    long sum_rt = 0 ;
    long max_rt = 0 ;
    TrickModelIterator it = _curve->begin();
    const TrickModelIterator e = _curve->end();
    int cnt = 0;
    while (it != e) {

        double time = it.t();
        long rt = (long)it.x();

        if ( time < 1.0 && rt > 2000000.0) {
            // Throw out bad points at start of sim
            continue;
        }

        if ( cnt > 0 && rt > 0 ) {
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

        sum_squares += rt*rt;
        sum_rt += rt;

        ++cnt;
        ++it;
    }

    double ss = (double)sum_squares;
    double s = (double)sum_rt;
    double n = (double)cnt;

    _max_runtime = (max_rt)/1000000.0;
    _avg_runtime = (s/n)/1000000.0;
    _stddev_runtime = qSqrt(ss/n - s*s/(n*n))/1000000.0 ;

    if ( _npoints > 1 ) {
        int max_cnt = 0 ;
        _freq = 0;
        // Could be multiple frequencies - choose mode
        foreach ( long freq, map_freq.keys() ) {
            int cnt = map_freq.value(freq);
            if ( cnt > max_cnt ) {
                _freq = freq/1000000.0;
                max_cnt = cnt;
            }
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

void Job::_parseJobId(const QString &jobId)
{
    QString name(jobId);

    name.replace("::","##");
    name.replace(QRegExp("^JOB_"),"");
    int idx1 = name.lastIndexOf (QChar('('));
    int idx2 = name.lastIndexOf (QChar(')'));
    int idx3 = name.lastIndexOf (QChar('_'));
    if ( (idx3 > idx1) && name.at(idx3+1).isDigit() ) {
        // frequency specified e.g. (read_simbus_0.100)
        _job_class = name.mid(idx1+1,idx3-idx1-1);
        _freq = name.mid(idx3+1,idx2-idx3-1).toDouble();
    } else {
        // frequency not specified e.g. (derivative)
        _job_class = name.mid(idx1+1,idx2-idx1-1);
        _freq = -1.0;
    }

    // e.g. 1828.00 from example name
    int idx4 = name.lastIndexOf(QChar('.'),idx1);
    int idx5 = name.lastIndexOf(QChar('.'),idx4-1);
    _job_num = name.mid(idx5+1,idx1-idx5-1);

    //
    // child/thread id + job name
    //
    // For Trick 13, the job frame_userjobs_CX.frame_sched_time has id X
    //
    if ( jobId.startsWith("frame_userjobs_C") &&
         jobId.endsWith("frame_sched_time") ) {

        name = name.remove(".frame_sched_time");
        _job_name = name.trimmed();
        QString strThreadId = name.remove(QString("frame_userjobs_C"));
        bool ok = false;
        _thread_id = strThreadId.toDouble(&ok);
        if ( !ok ) {
            _err_stream << "snap [bad scoobies]: Couldn't determine thread_id "
                        << "from jobId \"" << jobId << "\"";
            throw std::runtime_error(_err_string.toAscii().constData());
        }

    } else {

        _thread_id = 0 ;
        QString stid;
        int idx6;
        for ( idx6 = idx5-1 ; idx6 > 0 ; idx6-- ) {
            if ( isdigit(name.at(idx6).toAscii()) ) {
                stid.prepend(name.at(idx6));
            } else {
                if ( name.at(idx6) == 'C' && name.at(idx6-1) == '_' ) {
                    _thread_id = stid.toInt();
                    idx6--;
                } else {
                    idx6++;
                }
                break;
            }
        }

        _job_name = name.mid(0,idx6);
    }

}

double Job::freq()
{
    if ( _curve ) {
        _do_stats();
    }
    return _freq;
}

// Parse long logname and set job members accordingly
// An example logname:
// JOB_schedbus.SimBus##read_ALDS15_ObcsRouter_C1.1828.00(read_simbus_0.100)

Job::Job(TrickCurveModel* curve) :
     _curve(curve),_npoints(0),
     _is_stats(false)
{
    if ( !curve ) {
        return;
    }

    _npoints = curve->rowCount();
    _log_name = curve->tableName();

    _parseJobId(_log_name);
}

Job::Job(const QString &jobId) :
     _curve(0),_npoints(0),
     _log_name(jobId),
     _is_stats(false)
{
    _parseJobId(_log_name);
}

QString Job::log_name() const
{
    return _log_name;
}


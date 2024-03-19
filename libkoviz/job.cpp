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
    } else if ( _job_name.startsWith("trick_frame_userjobs_C") ||
                _job_name.startsWith("frame_userjobs_C") ||
                _job_name.startsWith("snap_userjobs_C") ) {
        simobj = "frame_log";
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

    if ( ! _curve ) {
        // TODO: throw exception
        fprintf(stderr,"koviz [error]: Job::_do_stats() called without setting:");
        fprintf(stderr," curve\n");
        exit(-1);
    }

    double freq = 0.0;
    QMap<double,int> map_freq;
    double last_nonzero_timestamp = 0.0 ;

    double sum_squares = 0 ;
    double sum_rt = 0 ;
    double max_rt = 0 ;
    ModelIterator* it = _curve->begin();
    int cnt = 0;
    while ( !it->isDone() ) {

        double time = it->t();
        double rt = it->y();

        if ( rt < 0 ) {
            rt =  0.0;
        }

        if ( cnt > 0 && rt > 0 ) {
            freq = time - last_nonzero_timestamp;
            int freq_cnt = 0;
            if ( map_freq.contains(freq) ) {
                freq_cnt = map_freq.value(freq)+1;
            }
            map_freq.insert(freq,freq_cnt);
            last_nonzero_timestamp = time;
        }

        if ( rt > max_rt ) {
            max_rt = rt;
            _max_timestamp = time;
        }

        sum_squares += rt*rt;
        sum_rt += rt;

        ++cnt;
        it->next();
    }
    delete it;

    double ss = sum_squares;
    double s = sum_rt;
    double n = (double)cnt;

    _max_runtime = max_rt;
    _avg_runtime = s/n;
    _stddev_runtime = qSqrt(ss/n - s*s/(n*n)) ;

    //
    // (re)Calculate job frequency
    //
    if ( _npoints > 1 ) {
        int max_cnt = 0 ;
        double savedFreq = _freq;
        _freq = 0;
        // Could be multiple frequencies - choose mode
        foreach ( double freq, map_freq.keys() ) {
            int cnt = map_freq.value(freq);
            if ( cnt > max_cnt ) {
                _freq = freq;
                max_cnt = cnt;
            }
        }

        if ( _job_name == "trick_sys.sched.advance_sim_time" && _freq == 0 ) {
            // With Orlando's RUN I found that I couldn't trust
            // the advance_sim_time value, HOWEVER!, with Thomas' RUN
            // I found that if the above fails to find a frequency
            // to go ahead and fall back on the freq as specified in the
            // S_job_execution
            _freq = savedFreq;
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

    _isFrameTimerJob = false;
    if ( (
         jobId.startsWith("trick_frame_userjobs_C") ||
         jobId.startsWith("frame_userjobs_C") ||
         jobId.startsWith("snap_userjobs_C")) &&
         jobId.endsWith("frame_time") ) {
        _isFrameTimerJob = true;
    }

    //
    // child/thread id + job name
    //
    // For Trick 13, the job frame|snap_userjobs_CX.frame_sched_time has id X
    //
    if ( _isFrameTimerJob ) {

        name = name.remove(".frame_time");
        _job_name = name.trimmed();

        QString strThreadId;
        if ( _job_name.startsWith("snap") ) {
             strThreadId = name.remove(QString("snap_userjobs_C"));
        } else if ( _job_name.startsWith("frame") ) {
            // Trick13
             strThreadId = name.remove(QString("frame_userjobs_C"));
        } else if ( _job_name.startsWith("trick") ) {
            // Trick17
            strThreadId = name.remove(QString("trick_frame_userjobs_C"));
        }
        bool ok = false;
        _thread_id = strThreadId.toDouble(&ok);
        if ( !ok ) {
            _err_stream << "koviz [bad scoobies]: Couldn't determine thread_id "
                        << "from jobId \"" << jobId << "\"";
            throw std::runtime_error(_err_string.toLatin1().constData());
        }

    } else {

        _thread_id = 0 ;
        QString stid;
        int idx6;
        for ( idx6 = idx5-1 ; idx6 > 0 ; idx6-- ) {
            if ( isdigit(name.at(idx6).toLatin1()) ) {
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

Job::Job(CurveModel* curve) :
     _curve(curve),_npoints(0),_isFrameTimerJob(false),
     _is_stats(false)
{
    if ( !curve ) {
        return;
    }

    _npoints = curve->rowCount();
    _log_name = curve->y()->name();

    _parseJobId(_log_name);
}

Job::Job(const QString &jobId) :
     _curve(0),_npoints(0),_isFrameTimerJob(false),
     _log_name(jobId),
     _is_stats(false)
{
    _parseJobId(_log_name);
}

QString Job::job_id() const
{
    return _log_name;
}


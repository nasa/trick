#include "thread.h"

#include <cmath>

QString Thread::_err_string;
QTextStream Thread::_err_stream(&Thread::_err_string);

static bool intLessThan(int a, int b)
{
    return a < b;
}

Thread::Thread(const QString &runDir) :
    _runDir(runDir), _threadId(-1), _sJobExecThreadInfo(runDir),
    _avg_runtime(0),_avg_load(0), _tidx_max_runtime(0),
    _max_runtime(0), _max_load(0),_stdev(0),_freq(0.0),
    _num_overruns(0),_runtimeCurve(0)
{
}

Thread::~Thread()
{
    if ( _runtimeCurve ) {
        delete _runtimeCurve;
    }
}

void Thread::addJob(Job* job)
{
    if ( !_jobs.isEmpty() && _threadId != job->thread_id() ) {
        _err_stream << "snap [bad scoobies]: Thread::addJob() called with "
                    << "job with threadId that doesn't match other jobs. "
                    << "Conflicing jobs are:\n    "  << _jobs.at(0)->job_name()
                    << "\nand\n    " << job->job_name() ;
        throw std::runtime_error(_err_string.toAscii().constData());
    }

    if ( _jobs.isEmpty() ) {
        _threadId = job->thread_id();
        _sJobExecThreadInfo.setThreadId(_threadId);
    }

    _jobs.append(job);
}

void Thread::_do_stats()
{
    if ( _jobs.size() == 0 ) {
        return;
    }


    qSort(_jobs.begin(),_jobs.end(),jobAvgTimeGreaterThan);

    if ( _sJobExecThreadInfo.hasInfo() ) {
        _freq = _sJobExecThreadInfo.frequency();
    } else {
        _freq = _calcFrequency();
    }

    if ( _threadId == 0 && _freq == 0.0 ) {
        QString msg;
        msg += "snap [error]: couldn't find job";
        msg += " trick_sys.sched.advance_sim_time.";
        msg += " Cannot determine thread0's frequency.";
        throw std::runtime_error(msg.toAscii().constData());
    }

    //
    // Count number of frames
    //
    Job* job0 = _jobs.at(0);
    TrickCurveModel* curve = job0->curve();
    TrickModelIterator it = curve->begin();
    const TrickModelIterator e = curve->end();
    int frameCnt = 0;
    if ( _freq < 0.000001 ) {
        frameCnt = curve->rowCount();
    } else {
        double tnext = it.t() + _freq;
        frameCnt = 1;
        while (it != e) {
            if (it.t()+1.0e-6 > tnext) {
                tnext += _freq;
                ++frameCnt ;
            }
            ++it;
        }
    }

    //
    // Create table with frameCnt number of rows
    //
    QString tableName = QString("Thread %1 Runtime").arg(_threadId);
    _runtimeCurve = new SnapTable(tableName);
    _runtimeCurve->insertColumns(0,2);
    _runtimeCurve->setHeaderData(0,Qt::Horizontal,QString("sys.exec.out.time"));
    _runtimeCurve->setHeaderData(1,Qt::Horizontal,QString("ThreadRunTime"));
    _runtimeCurve->insertRows(0,frameCnt);

    //
    // Calc frame runtimes (sum of jobs runtimes per frame), num overruns etc.
    //
    bool is_frame_change = true;
    double sum_time = 0.0;
    double frame_time = 0.0;
    _max_runtime = 0.0;
    int last_frameidx = 0 ;
    int tidx = 0 ;
    int rowCount = 0 ;
    TrickModelIterator it2;

    it = curve->begin();
    double tnext = it.t() + _freq;

    _num_overruns = 0;
    while (it != e) {

        if ( _freq < 0.000001 ) {
            is_frame_change = true;
        } else if ( it.t()+1.0e-6 > tnext ) {
            tnext += _freq;
            if ( frame_time/1000000.0 > _freq ) {
                _num_overruns++;
            }
            is_frame_change = true;
        }

        if ( is_frame_change ) {
            if ( frame_time > _max_runtime ) {
                _max_runtime = frame_time;
                _tidx_max_runtime = last_frameidx;
            }
            double ft = frame_time/1000000.0;
            _frameidx2runtime[last_frameidx] = ft;
            QModelIndex timeIdx = _runtimeCurve->index(rowCount,0);
            QModelIndex valIdx = _runtimeCurve->index(rowCount,1);
            ++rowCount;
            _runtimeCurve->setData(timeIdx,it.t());
            _runtimeCurve->setData(valIdx,ft);
            sum_time += frame_time;
            frame_time = 0.0;
            last_frameidx = tidx;
            is_frame_change = false;
        }

        foreach ( Job* job, _jobs ) {
            if ( job->job_name().startsWith("frame_userjobs_C") ) {
                // For Trick 13
                // Do not use child frame scheduling time for frame time sum.
                // Snap reports the sum of the userjobs, not the frame
                // scheduling time since the frame scheduling time includes
                // executive overhead (e.g. the frame logging itself).
                continue;
            }
            it2 = job->curve()->begin();
            double ft = it2[tidx].x();
            if ( ft < 0 ) {
                ft = 0.0;
            }
            frame_time += ft;
        }

        tidx++;
        ++it;
    }

    _max_runtime /= 1000000.0;
    _avg_runtime = sum_time/(double)this->numFrames()/1000000.0;
    if ( _freq > 0.0000001 ) {
        _avg_load = 100.0*_avg_runtime/_freq;
        _max_load = 100.0*_max_runtime/_freq;
    }

    // Stddev
    foreach ( int tidx, _frameidx2runtime.keys() ) {
        double rt = _frameidx2runtime[tidx];
        double vv = (_avg_runtime-rt)*(_avg_runtime-rt);
        _stdev += vv;
    }
    _stdev = sqrt(_stdev/(double)this->numFrames());

}



// Guess frequency (cycle time) of thread
//
// If Trick 13, use S_job_execution if possible
// Else Use this
//
// Guess the thread freq by:
//   thread0:
//              It's the same as sim frame time which
//              can be determined by the job:
//                       trick_sys.sched.advance_sim_time
//   threads1-N:
//              Guess that the thread freq is the same freq of the job
//              with max cycle time
double Thread::_calcFrequency()
{
    double freq;

    freq = -1.0e20;
    foreach ( Job* job, _jobs ) {
        if ( _threadId == 0 ) {
            if ( job->job_name() == "trick_sys.sched.advance_sim_time" ) {
                freq = job->freq();
                break;
            }
        } else {
            if ( job->freq() < 0.000001 ) continue;
            if ( job->freq() > freq ) {
                freq = job->freq();
            }
        }
    }
    if (freq == -1.0e20) {
        freq = 0.0;
    }

    return freq;
}

double Thread::runtime(int tidx) const
{
    double rt = -1.0;

    for ( int ii = tidx; ii >= 0 ; --ii) {
        if ( _frameidx2runtime.contains(ii) ) {
            rt = _frameidx2runtime[ii];
            break;
        }
    }

    return rt;
}

double Thread::runtime(double timestamp) const
{
    int tidx = _jobs.at(0)->curve()->indexAtTime(timestamp);
    double rt = runtime(tidx);
    return rt;
}

int Thread::numFrames() const
{
    return _frameidx2runtime.keys().size();
}

double Thread::avgJobRuntime(Job *job) const
{
    double rt = 0.0;
    int nFrames = numFrames();
    if ( nFrames > 0 ) {
        rt = job->avg_runtime()*job->npoints()/nFrames;
    }
    return rt;
}

//
// Returns a percent average load of job on thread
// A load of 0.0 can mean it's negligible
// If there's only one job on a thread, load is 100% if there's any load
//
double Thread::avgJobLoad(Job *job) const
{
    double load = 0.0;

    if ( _avg_runtime > 0.000001 ) {
        if ( _jobs.length() == 1 ) {
            // Fix round off error.
            // If the job has an average above zero
            // and the thread has a single job
            // this job took 100%, so force it to 100.0
            load = 100.0;
        } else {
            load = 100.0*avgJobRuntime(job)/_avg_runtime;
        }
    }

    return load;

}

Threads::Threads(const QString &runDir, const QList<Job*>& jobs) : _jobs(jobs)
{
    foreach ( Job* job, _jobs ) {

        int tid = job->thread_id();
        if ( ! _ids.contains(tid) ) {
            _ids.append(tid);
            Thread* thread = new Thread(runDir);
            _threads.insert(tid,thread);
        }

        Thread* thread = _threads.value(tid);
        thread->addJob(job);
    }

    qSort(_ids.begin(),_ids.end(),intLessThan);

    foreach ( Thread* thread, _threads.values() ) {
        thread->_do_stats();
    }

}

Threads::~Threads()
{
    foreach ( Thread* thread, _threads.values() ) {
        delete thread;
    }
}

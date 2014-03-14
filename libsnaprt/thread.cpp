#include "thread.h"

#include <cmath>

QString Thread::_err_string;
QTextStream Thread::_err_stream(&Thread::_err_string);

static bool intLessThan(int a, int b)
{
    return a < b;
}

Thread::Thread(const QString &runDir) :
    _runDir(runDir), _threadId(-1),
    avg_runtime(0),avg_load(0), tidx_max_runtime(0),
    max_runtime(0), max_load(0),stdev(0),freq(0.0),
    num_overruns(0)
{
}

Thread::~Thread()
{
    //delete _sJobExecThreadInfo;
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
        _sJobExecThreadInfo = new SJobExecThreadInfo(_runDir,_threadId);
    }

    _jobs.append(job);
}

void Thread::_do_stats()
{
    if ( _jobs.size() == 0 ) {
        return;
    }

    qSort(_jobs.begin(),_jobs.end(),jobAvgTimeGreaterThan);

    if ( _sJobExecThreadInfo->hasInfo() ) {
        freq = _sJobExecThreadInfo->frequency();
    } else {
        freq = _calcFrequency();
    }

    if ( _threadId == 0 && freq == 0.0 ) {
        QString msg;
        msg += "snap [error]: couldn't find job";
        msg += " trick_sys.sched.advance_sim_time.";
        msg += " Cannot determine thread0's frequency.";
        throw std::runtime_error(msg.toAscii().constData());
    }

    Job* job0 = _jobs.at(0);
    TrickCurveModel* curve = job0->curve();
    TrickModelIterator it = curve->begin();
    const TrickModelIterator e = curve->end();
    double tnext = it.t() + freq;
    double sum_time = 0.0;
    double frame_time = 0.0;
    max_runtime = 0.0;
    int last_frameidx = 0 ;
    bool is_frame_change = true;
    int tidx = 0 ;
    TrickModelIterator it2;

    while (it != e) {

        if ( freq < 0.000001 ) {
            is_frame_change = true;
        } else if ( it.t()+1.0e-6 > tnext ) {
            tnext += freq;
            if ( frame_time/1000000.0 > freq ) {
                num_overruns++;
            }
            is_frame_change = true;
        }

        if ( is_frame_change ) {
            if ( frame_time > max_runtime ) {
                max_runtime = frame_time;
                tidx_max_runtime = last_frameidx;
            }
            _frameidx2runtime[last_frameidx] = frame_time/1000000.0;
            sum_time += frame_time;
            frame_time = 0.0;
            last_frameidx = tidx;
            is_frame_change = false;
        }

        foreach ( Job* job, _jobs ) {
            it2 = job->curve()->begin();
            frame_time += it2[tidx].x();
        }

        tidx++;
        ++it;
    }

    max_runtime /= 1000000.0;
    avg_runtime = sum_time/(double)this->nframes()/1000000.0;
    if ( freq > 0.0000001 ) {
        avg_load = 100.0*avg_runtime/freq;
        max_load = 100.0*max_runtime/freq;
    }

    // Stddev
    foreach ( int tidx, _frameidx2runtime.keys() ) {
        double rt = _frameidx2runtime[tidx];
        double vv = (avg_runtime-rt)*(avg_runtime-rt);
        stdev += vv;
    }
    stdev = sqrt(stdev/(double)this->nframes());

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
    double fq;

    fq = -1.0e20;
    foreach ( Job* job, _jobs ) {
        if ( _threadId == 0 ) {
            if ( job->job_name() == "trick_sys.sched.advance_sim_time" ) {
                fq = job->freq();
                break;
            }
        } else {
            if ( job->freq() < 0.000001 ) continue;
            if ( job->freq() > fq ) {
                fq = job->freq();
            }
        }
    }
    if (fq == -1.0e20) {
        fq = 0.0;
    }

    return fq;
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

int Thread::nframes() const
{
    return _frameidx2runtime.keys().size();
}

double Thread::avg_job_runtime(Job *job) const
{
    double rt = 0.0;
    int nFrames = nframes();
    if ( nFrames > 0 ) {
        rt = job->avg_runtime()*job->npoints()/nframes();
    }
    return rt;
}

//
// Returns a percent average load of job on thread
// A load of 0.0 can mean it's negligible
// If there's only one job on a thread, load is 100% if there's any load
//
double Thread::avg_job_load(Job *job) const
{
    double load = 0.0;

    if ( avg_runtime > 0.000001 ) {
        if ( _jobs.length() == 1 ) {
            // Fix round off error.
            // If the job has an average above zero
            // and the thread has a single job
            // this job took 100%, so force it to 100.0
            load = 100.0;
        } else {
            load = 100.0*avg_job_runtime(job)/avg_runtime;
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

Thread Threads::get(int id) const
{
    return *(_threads[id]);
}

QList<Thread> Threads::list() const
{
    QList<Thread> list ;

    foreach ( Thread* thread, _threads.values() ) {
        list.append(*thread);
    }

    return list;
}

QList<int> Threads::ids() const
{
    return _ids;
}

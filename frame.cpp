#include "frame.h"

QString Frame::frame_time_name = "real_time.rt_sync.frame_sched_time";
QString Frame::overrun_time_name = "real_time.rt_sync.frame_overrun_time";

bool frameTopJobsGreaterThan(const QPair<double,Job*>& a,
                           const QPair<double,Job*>& b)
{
    return a.first > b.first;
}

bool frameTimeGreaterThan(const Frame& a,const Frame& b)
{
    return a.frame_time() > b.frame_time();
}


Frame::Frame(QList<Job *> *jobs,
             int timeidx,  double timestamp,
             double frame_time, double overrun_time) :
    _jobs(jobs),
    _tidx(timeidx),
    _timestamp(timestamp),
    _frame_time(frame_time),
    _overrun_time(overrun_time),
    _jobloadindex(0.0)
{
    if ( overrun_time > 0.0 ) {
        _is_overrun = true;
    }
}

double Frame::jobloadindex()
{
    if ( _topjobs.empty() ) {
        _calc_topjobs();
    }
    return _jobloadindex;
}

QList<QPair<double, Job *> >* Frame::topjobs()
{
    if ( _topjobs.empty() ) {
        _calc_topjobs();
    }
    return &_topjobs;
}


void Frame::_calc_topjobs()
{
    const int count = 10; // top ten for now

    double jcnt = 0.0 ;
    foreach ( Job* job, *_jobs ) {

        double rt = job->runtime()[_tidx]/1000000.0;
        if ( rt > job->avg_runtime()+1.50*job->stddev_runtime() ) {
            // Job Load Index
            jcnt += 1.0;
        }

        // List of top jobs with most runtime for the frame
        int len = _topjobs.length();
        if ( len < count ) {
            _topjobs.append(qMakePair(rt,job));
        } else {
            if ( len == count ) {
                qSort(_topjobs.begin(), _topjobs.end(), frameTopJobsGreaterThan);
            }
            QPair<double,Job*> ljob = _topjobs.last();
            double lrt = ljob.second->runtime()[_tidx]/1000000.0;
            if ( rt > lrt ) {
                _topjobs.replace(len-1,qMakePair(rt,job));
                qSort(_topjobs.begin(), _topjobs.end(), frameTopJobsGreaterThan);
            }
        }
    }
    _jobloadindex = 100.0*jcnt/(double)_jobs->size();
    qSort(_topjobs.begin(),_topjobs.end(),frameTopJobsGreaterThan);
}

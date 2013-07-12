/*
 * Snap! Crackle! Pop!
 */

#include <stdio.h>
#include <ctype.h>
#include <cmath>
#include <QDir>
#include <QFile>
#include <stdexcept>

#include "snap.h"

bool frameTopJobsGreaterThan(const QPair<double,Job*>& a,
                           const QPair<double,Job*>& b)
{
    return a.first > b.first;
}

bool topThreadGreaterThan(const QPair<double,Thread>& a,
                         const QPair<double,Thread>& b)
{
    if ( a.second.freq > 0.000001 && b.second.freq > 0.000001 ) {
        return (a.first/a.second.freq) > (b.first/b.second.freq);
    } else {
        return (a.first > b.first);
    }
}

Snap::Snap(const QString &irundir, double istart, double istop) :
    _err_stream(&_err_string),
    _rundir(irundir), _start(istart),_stop(istop), _is_realtime(false),
    _frame_avg(0.0),_frame_stddev(0),_curr_sort_method(NoSort),
    _river_userjobs(0), _river_frame(0), _river_trickjobs(0),
    _num_overruns(0), _threads(0),_sim_objects(0)
{
    _process_rivers();        // _jobs list and _frames created
    qSort(_jobs.begin(), _jobs.end(), jobAvgTimeGreaterThan);
    _curr_sort_method = SortByJobAvgTime;
    _calc_frame_avg();        // _frame_avg calculated
    _calc_frame_stddev();     // _frame_stddev calculated

    _threads = new Threads(_jobs);

    _sim_objects = new SimObjects(_jobs);
}

Snap::~Snap()
{
    foreach ( Job* job, _jobs ) {
        delete job;
    }

    if (_river_userjobs ) delete _river_userjobs;
    if (_river_frame ) delete _river_frame;
    if (_river_trickjobs ) delete _river_trickjobs;

    if ( _threads ) delete _threads;
    if ( _sim_objects ) delete _sim_objects;
}

QList<Job *>* Snap::jobs(SortBy sort_method)
{
    if ( _curr_sort_method != sort_method ) {
        if ( sort_method == SortByJobAvgTime ) {
            qSort(_jobs.begin(), _jobs.end(), jobAvgTimeGreaterThan);
            _curr_sort_method = SortByJobAvgTime;
        } else if ( sort_method == SortByJobMaxTime ) {
            qSort(_jobs.begin(), _jobs.end(), jobMaxTimeGreaterThan);
            _curr_sort_method = SortByJobMaxTime;
        }
    }

    return &_jobs;
}

BoundedTrickBinaryRiver *Snap::_open_river(const QString &rundir,
                                             const QString &logfilename,
                                            double start, double stop)
{
    BoundedTrickBinaryRiver* river;

    QDir dir(rundir);
    if ( ! dir.exists() ) {
        _err_stream << "snap [error]: couldn't find run directory: "
                    << rundir << "\n";
        throw std::invalid_argument(_err_string.toAscii().constData());
    }

    QString trk = rundir + QString("/") + logfilename;
    QFile file(trk);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        _err_stream << "snap [error]: couldn't read file: " << trk << "\n";
        throw std::invalid_argument(_err_string.toAscii().constData());
    } else {
        file.close();
    }

    //
    // If trk file is less than 1000 bytes, it can't be legit
    //
    if ( file.size() < 1000 ) {
        _err_stream << "snap [error]: "
                    << "suspicious filesize of "
                    << file.size()
                    << " for file \""
                    << trk
                    << "\""
                    << "  - bailing like Rob Bailey!!!";
        throw std::invalid_argument(_err_string.toAscii().constData());
    }

    try {
        river = new BoundedTrickBinaryRiver(trk.toAscii().data(),
                                         start,stop);
    }
    catch (std::range_error &e) {
        _err_stream << e.what();
        _err_stream << "snap [error]: -start or -stop options have bad vals\n";
        throw std::range_error(_err_string.toAscii().constData());
    }


    return river;
}

void Snap::_process_rivers()
{
    _river_trickjobs = _open_river(_rundir,
                                   QString("log_trickjobs.trk"),
                                   _start, _stop);
    _river_userjobs = _open_river(_rundir,
                                   QString("log_userjobs.trk"),
                                   _start, _stop);
    _river_frame = _open_river(_rundir,
                                   QString("log_frame.trk"),
                                   _start, _stop);

    _process_job_river(_river_trickjobs);
    _process_job_river(_river_userjobs);

    _frames = _process_frame_river(_river_frame);
    qSort(_frames.begin(),_frames.end(),frameTimeGreaterThan);
}


// TODO!!! For now, just make a string of "rates"... probably should take
//         mode instead and return a single number
QString Snap::frame_rate() const
{
    QString frame_rates;

    QList<long> rates;
    double* timestamps = _river_frame->getTimeStamps();
    long ltime = (long)(timestamps[0]*1000000);
    int npoints = _river_frame->getNumPoints();
    for ( int ii = 1 ; ii < npoints ; ii++ ) {
        long tt = (long)(timestamps[ii]*1000000);
        long dt = round_10(tt-ltime);
        if ( ! rates.contains(dt) ) {
            // Do fuzzy compare since doubles screw things up
            bool is_equal = false;
            foreach ( long rate, rates ) {
                if ( qAbs(dt-rate) < 2 ) {
                    is_equal = true;
                    break;
                }
            }
            if ( is_equal == false ) {
                rates.append(dt);
            }
        }

        ltime = tt;
    }
    foreach ( long rate, rates ) {
        double drate = rate/1000000.0;
        QString str =  QString("%1,").arg(drate);
        frame_rates.append(str);
    }
    frame_rates.chop(1); // cut comma

    return frame_rates;
}

QString Snap::thread_listing() const
{
    QString listing;

    QString str;

    int nthreads = _threads->size();
    int ii = 0 ;
    int ltid = 0;
    foreach ( int tid, _threads->ids() ) {
        if ( ii != 0 && ii%16 == 0 ) {
            listing += QString("\n");
            listing += str.sprintf("%20s   "," ");
        }
        if ( ii > 0 && tid != ltid+1) {
            listing += QString("MISSING,");
        }
        listing += str.sprintf("%d", tid);
        if ( ii < nthreads-1 ) {
            listing += QString(",");
        }
        ltid = tid;
        ++ii;
    }

    return listing;
}

void Snap::_calc_frame_avg()
{
    double sum = 0.0 ;
    foreach ( Frame frame, _frames ) {
        sum += frame.frame_time;
    }

    _frame_avg = sum/_frames.length() ;
}

void Snap::_calc_frame_stddev()
{
    if ( _frame_avg == 0.0 ) {
        _calc_frame_avg();
    }

    double sum = 0.0 ;
    foreach ( Frame frame, _frames ) {
        double frametime =  frame.frame_time;
        double vv = (frametime-_frame_avg)*(frametime-_frame_avg);
        sum += vv;
    }

    _frame_stddev = sqrt(sum/_frames.length()) ;
}

//
// logfilename e.g. log_trickjobs.trk
//
bool Snap::_process_job_river( BoundedTrickBinaryRiver* river )
{
    bool ret = true;

    std::vector<LOG_PARAM> params = river->getParamList();
    for ( unsigned int ii = 1 ; ii < params.size(); ++ii ) {

        LOG_PARAM param = params.at(ii);

        Job* job = new Job(river,const_cast<char*>(param.data.name.c_str()));

        QString job_id = job->id();

        if (  _id_to_job.contains(job_id) ) {
            // Since job already created and most likely has more info
            // e.g. parsing S_job_execution has more info e.g. phase
            delete job;
            job = _id_to_job.value(job_id);
        } else {
            _id_to_job[job_id] = job;
            _jobs.append(job);
        }

    }

    return ret;
}

QList<Frame> Snap::_process_frame_river(BoundedTrickBinaryRiver* river)
{
    QList<Frame> frames;

    _is_realtime = false;

    int npoints = river->getNumPoints();
    if ( npoints == 0 ) {
        _err_stream << "snap [error]: file \"" << river->getFileName()
                    << "\" has no points";
        throw std::invalid_argument(_err_string.toAscii().constData());
    }

    std::vector<LOG_PARAM> params = river->getParamList();

    double* timestamps = river->getTimeStamps();
    double* frame_times = 0 ;
    double* overrun_times = 0 ;
    int cnt = 0 ;
    for ( unsigned int ii = 1 ; ii < params.size(); ++ii ) {

        LOG_PARAM param = params.at(ii);

        QString qparam(param.data.name.c_str());

        if ( qparam ==  Frame::frame_time_name ) {
            frame_times = river->getVals(const_cast<char*>
                                             (param.data.name.c_str()));
            cnt++;
        } else if ( qparam == Frame::overrun_time_name ) {
            overrun_times = river->getVals(const_cast<char*>
                                             (param.data.name.c_str()));
            cnt++;
        }
        if ( cnt == 2 ) {
            break;
        }
    }
    if ( frame_times == 0 || overrun_times == 0 ) {
        QString param  = ( frame_times == 0 ) ?
                        Frame::frame_time_name : Frame::overrun_time_name ;
        // Shouldn't happen unless trick renames that param
        _err_stream << "snap [error]: Couldn't find parameter "
                << param
                << "in file \""
                << river->getFileName()
                << "\"";
        throw std::invalid_argument(_err_string.toAscii().constData());
    }

    _num_overruns = 0;
    for ( int tidx = 0 ; tidx < npoints ; ++tidx ) {
        Frame frame;
        double tt = timestamps[tidx];
        frame.timeidx = tidx;
        frame.timestamp = tt;
        frame.frame_time = frame_times[tidx]/1000000.0;
        frame.overrun_time = overrun_times[tidx]/1000000.0;
        if ( frame.overrun_time > 0.0 ) {
            _num_overruns++;
        }
        if ( !_is_realtime && frame_times[tidx] != 0 ) {
            _is_realtime = true;
        }

        // Job load percentage (a lot of jobs running hot)
        double jcnt = 0.0 ;
        foreach ( Job* job, _jobs ) {
            double rt = job->runtime()[tidx]/1000000.0;
            if ( rt > job->avg_runtime()+1.50*job->stddev_runtime() ) {
                jcnt += 1.0;
            }

            // List of top jobs with most runtime for the frame
            int len = frame.topjobs.length() ;
            if ( len < 10 ) {
                frame.topjobs.append(qMakePair(rt,job));
            } else {
                QPair<double,Job*> ljob = frame.topjobs.last();
                double lrt = ljob.second->runtime()[tidx]/1000000.0;
                if ( rt > lrt ) {
                    frame.topjobs.replace(len-1,qMakePair(rt,job));
                    qSort(frame.topjobs.begin(),
                          frame.topjobs.end(),
                          frameTopJobsGreaterThan);
                }
            }
        }
        frame.jobloadindex = 100.0*jcnt/(double)num_jobs();
        qSort(frame.topjobs.begin(),frame.topjobs.end(),frameTopJobsGreaterThan);
        frames.append(frame);
    }

    qSort(frames.begin(), frames.end(), frameTimeGreaterThan);


    return frames;
}

SnapReport::SnapReport(Snap &snap) : _snap(snap)
{
}

QString SnapReport::report()
{
    QString rpt;
    QString str;

    int cnt = 0;
    int max_cnt = 10;

    if ( ! _snap.is_realtime() ) {
        rpt.append("Oh Snap!!! - The following sim run had realtime disabled!\n");
        rpt += str.sprintf("          %s\n",_snap.rundir().toAscii().constData());
        rpt.append("Try again!\n");
        return rpt;
    }

    QString divider("------------------------------------------------\n");
    QString endsection("\n\n");

    rpt.append(
"************************************************************************\n"
"*                            Snap! Results                             *\n"
"************************************************************************\n\n");

    rpt += str.sprintf("%20s = %s\n", "Run directory ",
                                     _snap.rundir().toAscii().constData());
    rpt += str.sprintf("%20s = %8.3lf\n", "Start time ", _snap.start());
    rpt += str.sprintf("%20s = %8.3lf\n", "Stop time ",_snap.stop());
    rpt += str.sprintf("%20s = %d\n", "Num jobs", _snap.num_jobs());
    rpt += str.sprintf("%20s = %d\n", "Num frames",_snap.num_frames());
    rpt += str.sprintf("%20s = %d\n", "Num overruns",_snap.num_overruns());
    rpt += str.sprintf("%20s = %.2lf%%\n", "Percentage overruns",
                                          _snap.percent_overruns());
    rpt += str.sprintf("%20s = %s\n", "Frame rate(s)",
                                      _snap.frame_rate().toAscii().constData());
    rpt += str.sprintf("%20s = %.6lf\n", "Frame avg",_snap.frame_avg());
    rpt += str.sprintf("%20s = %.6lf\n","Frame stddev",_snap.frame_stddev());
    rpt += str.sprintf("%20s = %d\n", "Num threads",_snap.num_threads());
    rpt += str.sprintf("%20s = %s\n","Thread list",
                                   _snap.thread_listing().toAscii().constData());
    rpt += endsection;

    //
    // Spike Summary
    //
    const QList<Frame>* frames = _snap.frames();
    rpt += divider;
    rpt += QString("Top Spikes\n\n");
    rpt += str.sprintf("    %15s %15s %15s\n", "Time", "Spike", "JobLoadIndex%");
    cnt = 0 ;
    foreach ( Frame frame, *frames ) {
        if ( ++cnt > max_cnt ) break;
        double tt = frame.timestamp;
        double ft = frame.frame_time;
        rpt += str.sprintf("    %15.6lf %15.6lf %14.0lf%%\n",
                          tt ,ft, frame.jobloadindex);

    }
    rpt += endsection;

    //
    // Thread Summary
    //
    rpt += divider;
    rpt += QString("Thread Time Summary\n\n");
    rpt += str.sprintf("    %10s %10s %15s %15s %15s %15s %15s %15s\n",
            "Thread", "NumJobs", "Freq", "NumOverruns", "ThreadAvg", "AvgLoad%",
            "ThreadMax", "MaxLoad%");

    QList<Thread> threads = _snap.threads()->list() ;
    foreach ( Thread thread, threads ) {

        // Hard code num overruns for thread 0 :(
        //
        // Do this because the thread overrun calc
        // sums the job times on a thread, but the trick exec
        // overhead will not show up in that calc
        if ( thread.thread_id == 0 ) {
            thread.num_overruns = _snap.num_overruns();
        }

        rpt += str.sprintf("    %10d %10d %15.6lf %15d %15.6lf "
                          "%14.0lf%% %15.6lf %14.0lf%%\n",
                 thread.thread_id,thread.jobs.length(),thread.freq,
                 thread.num_overruns, thread.avg_runtime,
                 thread.avg_load, thread.max_runtime, thread.max_load);
    }
    rpt += endsection;

    //
    // Top Jobs Per Thread
    //
    rpt += divider;
    rpt += QString("Top Jobs by Thread\n\n");
    rpt += str.sprintf("    %8s %8s %15s %15s %10s%%     %-50s\n",
           "Thread", "NumJobs", "ThreadAvg", "JobAvgTime", "Percent", "JobName");
    foreach ( Thread thread, threads ) {

        rpt += str.sprintf("    %8d %8d %15.6lf ",
                   thread.thread_id,thread.jobs.length(),thread.avg_runtime) ;

        if ( thread.avg_runtime < 0.000005 && thread.jobs.length() > 1 ) {
            rpt += str.sprintf("%15s  %10s     %-49s\n", "--","--","--");
            continue;
        }

        double sum = 0;
        for ( int ii = 0; ii < 5; ++ii) {
            Job* job = thread.jobs.at(ii) ;
            double load = thread.avg_job_load(job);
            sum += load;
            if ( (load < 0.01 || (sum > 98.0 && ii > 0)) &&
                  thread.jobs.length() > 1 ) {
                break;
            } else {
                if ( ii > 0 ) {
                    rpt += str.sprintf("    %8s %8s %15s ", "","","");
                }
                rpt += str.sprintf("%15.6lf ",thread.avg_job_runtime(job));
                if ( thread.avg_runtime > 0.0000001 ) {
                    rpt += str.sprintf("%10.0lf%%", load);
                } else {
                    rpt += str.sprintf(" %10s", "--");
                }
                rpt += str.sprintf("     %-50s\n",
                                   job->job_name().toAscii().constData());
            }
            if ( ii == thread.jobs.length()-1 ) {
                break;
            }
        }
    }
    rpt += endsection;

    //
    // Job Time Avgs
    //
    rpt += divider;
    rpt += str.sprintf("Top Job Avg Times\n\n");
    rpt += str.sprintf("    %15s %6s %15s %15s    %-40s\n",
            "JobAvg", "Thread", "ThreadAvgTime", "JobFreq", "JobName");

    QList<Job*>* jobs = _snap.jobs(Snap::SortByJobAvgTime);
    cnt = 0 ;
    foreach ( Job* job, *jobs ) {

        if ( ++cnt > max_cnt ) break;

        rpt += str.sprintf("    %15.6lf %6d %15.6lf %15.6lf    %-40s\n",
                   job->avg_runtime(),
                   job->thread_id(),
                   _snap.threads()->get(job->thread_id()).avg_runtime,
                   job->freq(),
                   job->job_name().toAscii().constData() );

    }
    rpt += endsection;

    //
    // Job Time Maxes
    //
    rpt += divider;
    rpt += str.sprintf("Top Job Max Times\n\n");
    rpt += str.sprintf("    %15s %15s %6s %15s %15s    %-40s\n",
                   "JobMax", "SimTime","Thread", "ThreadTime",
                   "JobFreq","JobName");
    jobs = _snap.jobs(Snap::SortByJobMaxTime);
    cnt = 0 ;
    foreach ( Job* job, *jobs ) {

        if ( ++cnt > max_cnt ) break;

        rpt += str.sprintf("    %15.6lf %15.6lf %6d %15.6lf %15.6lf    %-40s\n",
                job->max_runtime(),
                job->max_timestamp(),
                job->thread_id(),
                _snap.threads()->get(job->thread_id()).
                                     runtime(job->max_timestamp()),
                job->freq(),
                job->job_name().toAscii().constData() );

    }
    rpt += endsection;

    //
    // Top 10 Sim Objects
    //
    QList<SimObject> simobjects = _snap.simobjects();
    if ( simobjects.length() > 100 ) {
        max_cnt = simobjects.length()/10;
    } else if ( simobjects.length() < 10 ) {
        max_cnt = simobjects.length();
    }

    // Format string to take length of simobject into account
    QString format;
    int maxlen = 0 ;
    cnt = 0 ;
    foreach ( SimObject sobject, simobjects ) {
        if ( ++cnt > max_cnt ) break;
        int len = sobject.name().length();
        if ( len > maxlen ) maxlen = len;
    }

    rpt += divider;
    rpt += QString("Top Sim Object Avg Times\n\n");
    format.sprintf("    %%%ds %%15s %%10s\n",maxlen);
    rpt += str.sprintf(TXT(format),"SimObject","AvgTime", "NumJobs");
    format.sprintf("    %%%ds %%15.6lf %%10d\n",maxlen);
    cnt = 0 ;
    foreach ( SimObject sobject, simobjects ) {
        if ( ++cnt > max_cnt ) break;
        rpt += str.sprintf(TXT(format),TXT(sobject.name()),
                           sobject.avg_runtime(),
                           sobject.jobs().length());
    }
    max_cnt = 10;
    rpt += endsection;

    //
    // Spike to Job Correlation
    //
    rpt += divider;
    rpt += QString("Top Spikes Job Breakdown\n\n");
    cnt = 0 ;
    foreach ( Frame frame, *frames ) {

        if ( ++cnt > max_cnt ) break;

        double tt = frame.timestamp;
        double ft = frame.frame_time;

        rpt += str.sprintf("Spike at time %.4lf of %g\n", tt ,ft);
        rpt += str.sprintf( "    %6s %15s %15s %15s    %-50s\n",
                        "Thread",
                        "ThreadTime",
                        "JobTime",
                        "JobFreq",
                        "JobName");

        int cnt2 = 0 ;
        double total = 0 ;
        for ( int ii = 0; ii < frame.topjobs.length(); ++ii) {

            QPair<double,Job*> topjob = frame.topjobs.at(ii);

            double rt =  topjob.first;
            Job* job = topjob.second;

            int tidx = frame.timeidx;
            double delta = 1.0e-3;
            if ( rt < delta ) {
                continue;
            }

            rpt += str.sprintf(
                    "    %6d %15.6lf %15.6lf %15.6lf    %-50s\n",
                    job->thread_id(),
                    _snap.threads()->get(job->thread_id()).runtime(tidx),
                    rt,
                    job->freq(),
                    job->job_name().toAscii().constData()) ;


            // limit printout of jobs
            if (job->job_name() != QString("trick_sys.sched.advance_sim_time")) {
                total += rt;
            }
            if ( total > 0.75*ft && ++cnt2 > 4 ) {
                break;
            }

        }
        rpt += endsection;
    }

    //
    // Spike to Thread Correlation
    //
    rpt += divider;
    rpt += QString("Top Spike Thread Breakdown\n\n");
    cnt = 0 ;
    foreach ( Frame frame, *frames ) {

        if ( ++cnt > max_cnt ) break;

        int tidx =  frame.timeidx;
        double tt = frame.timestamp;
        double ft = frame.frame_time;

        QList<QPair<double, Thread> > topthreads;
        foreach ( Thread thread, threads ) {
            topthreads.append(qMakePair(thread.runtime(tidx),thread));
        }
        qSort(topthreads.begin(),topthreads.end(),topThreadGreaterThan);

        rpt += str.sprintf("Spike at time %g of %g\n",tt,ft);
        rpt += str.sprintf("    %6s %15s %15s\n", "Thread", "ThreadFreq",
                                            "ThreadTime");
        for ( int ii = 0 ; ii < topthreads.length(); ++ii ) {
            QPair<double,Thread> topthread = topthreads.at(ii);
            if ( topthread.first < 1.0e-3 ) {
                break;
            }
            rpt += str.sprintf("    %6d %15.6lf %15.6lf\n",
                               topthread.second.thread_id,
                               topthread.second.freq,
                               topthread.first);
        }
        rpt += endsection;
    }

    return rpt;

}

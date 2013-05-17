#include "snap.h"

#include <stdio.h>
#include <ctype.h>
#include <cmath>
#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QDebug>

#include <stdio.h>

bool threadTimeGreaterThan(const QPair<int,long> &a,
                            const QPair<int,long> &b)
{
    return a.second > b.second;
}

bool jobTimeGreaterThan(const QPair<Job*,long> &a,
                         const QPair<Job*,long> &b)
{
    return a.second > b.second;
}

bool doubleTimeGreaterThan(const QPair<Job*,double> &a,
                            const QPair<Job*,double> &b)
{
    return a.second > b.second;
}

bool simObjTimeGreaterThan(const QPair<QString,double> &a,
                         const QPair<QString,double> &b)
{
    return a.second > b.second;
}

BoundedTrickBinaryRiver::BoundedTrickBinaryRiver(
        char *filename,  double start, double stop) :
    TrickBinaryRiver(filename),
    _start(start),
    _stop(stop),
    _npoints(0)
{
    int np = TrickBinaryRiver::getNumPoints();
    double *ts = TrickBinaryRiver::getTimeStamps();
    bool is_first = true;
    for ( int ii = 0; ii < np; ++ii) {
        double tt = ts[ii];
        if ( tt < start - 0.000001 ) {
            continue;
        }
        if ( tt > stop + 0.000001 ) {
            break;
        }

        if ( is_first ) {
            _timestamps = &(ts[ii]);
            std::vector<LOG_PARAM> params = TrickBinaryRiver::getParamList() ;
            for ( uint jj = 0; jj < params.size(); ++jj) {
                LOG_PARAM param = params[jj];
                double* vals = TrickBinaryRiver::getVals(
                            const_cast<char*>(param.data.name.c_str()));
                _vals[const_cast<char*>(param.data.name.c_str())] =  &(vals[ii]);

            }
            is_first = false ;
        }
        _npoints++;
    }
}

Jobs::Jobs(const QString &rundir, double start, double stop) :
    _start(start),_stop(stop)
{
    //_parse_s_job_execution(rundir);
    _parse_log_jobs(rundir,
                    QString("log_trickjobs.trk"),
                    &_river_trickjobs,
                    start, stop);
    _parse_log_jobs(rundir,
                    QString("log_userjobs.trk"),
                    &_river_userjobs,
                    start, stop);

    QList<QPair<double, long> > ovs = _parse_log_frame(rundir, start, stop);

    //
    // Main Summary
    //
    fprintf(stderr,"\n");
    fprintf(stderr,
"************************************************************************\n"
"*                            Snap! Results                             *\n"
"************************************************************************\n\n");

    fprintf(stderr,"%20s = %s\n",
            "Run directory", rundir.toAscii().constData());
    fprintf(stderr,"%20s = %8.3lf\n", "Start time ",
            _river_frame->getTimeStamps()[0]);
    fprintf(stderr,"%20s = %8.3lf\n", "Stop time ",
            _river_frame->getTimeStamps()[_river_frame->getNumPoints()-1]);
    fprintf(stderr,"%20s = %d\n", "Num jobs", _jobs.length());
    fprintf(stderr,"%20s = %d\n", "Num frames",ovs.length());

    //
    // Frame rate(s) (could be multiple e.g. 0.0120, 0.0125)
    //
    QList<long> rates;
    double* timestamps = _river_frame->getTimeStamps();
    long ltime = (long)(timestamps[0]*1000000);
    int npoints = _river_frame->getNumPoints();
    for ( int ii = 1 ; ii < npoints ; ii++ ) {
        long tt = (long)(timestamps[ii]*1000000);
        long dt = tt-ltime;
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
    QString buf;
    foreach ( long rate, rates ) {
        double drate = rate/1000000.0;
        QString str =  QString("%1,").arg(drate);
        buf.append(str);
    }
    buf.chop(1); // cut comma
    fprintf(stderr,"%20s = %s\n", "Frame rate(s)",buf.toAscii().constData());

    //
    // Avg frame exec time
    //
    double sum = 0 ;
    for ( int ii = 0 ; ii < ovs.length() ; ++ii ) {
        QPair<double,long> ov = ovs.at(ii);
        double ot = ov.second;
        sum += ot;
    }
    double avg = sum/ovs.length() ;
    fprintf(stderr,"%20s = %.6lf\n", "Frame avg",avg/1000000);

    //
    // Std dev for frame exec time
    //
    sum = 0 ;
    for ( int ii = 0 ; ii < ovs.length() ; ++ii ) {
        QPair<double,long> ov = ovs.at(ii);
        double ot = ov.second;
        double vv = (ot-avg)*(ot-avg);
        sum += vv;
    }
    double stddev = sqrt(sum/ovs.length()) ;
    fprintf(stderr,"%20s = %.6lf\n","Frame stddev",stddev/1000000);

    //
    // Thread summary
    //
    QSet<int> threads = _thread_list();
    fprintf(stderr,"%20s = %d\n", "Num threads",threads.size());
    fprintf(stderr,"%20s = ","Thread list");
    int ii = 0 ;
    int ltid = 0;
    foreach ( int tid, threads ) {
        if ( ii != 0 && ii%16 == 0 ) {
            fprintf(stderr,"\n");
            fprintf(stderr,"%20s   "," ");
        }
        if ( ii > 0 && tid != ltid+1) {
            fprintf(stderr,"MISSING,");
        }
        fprintf(stderr,"%d", tid);
        if ( ii < threads.size()-1 ) {
            fprintf(stderr,",");
        }
        ltid = tid;
        ++ii;
    }
    fprintf(stderr,"\n");

    //
    // End of main summary
    //
    fprintf(stderr,"\n");

    //
    // Stddev Spikes (a lot of jobs running hot)
    //
    QMap<int,double> time2hotjobpercent;
    QMap<int,double> time2spicyhotjobpercent;
    double nj = (double)_jobs.size();
    for ( int ii = 0; ii < npoints; ++ii) {
        double hcnt = 0.0 ;
        double shcnt = 0.0 ;
        foreach ( Job* job, _jobs ) {
            double rt = job->runtime[ii]/1000000;
            if ( rt > job->avg_runtime()+job->stddev_runtime() ) {
                hcnt += 1.0;
                if ( rt > job->avg_runtime()+2*job->stddev_runtime() ) {
                    shcnt += 1.0;
                }
            }
        }
        time2hotjobpercent.insert(ii,100.0*hcnt/nj);
        time2spicyhotjobpercent.insert(ii,100.0*shcnt/nj);
    }

    //
    // Spike Summary
    //
    fprintf(stderr,"------------------------------------------------\n");
    fprintf(stderr,"Top 10 Spikes\n\n");
    fprintf(stderr,"    %15s %15s %15s %15s\n",
            "Time", "Spike", "HotJob%", "SpicyHotJob%");

    for ( int ii = 0 ; ii < 10 ; ++ii ) {
        QPair<double,long> ov = ovs.at(ii);
        double tt = ov.first;
        double ot = ov.second;
        int tidx = _river_frame->getIndexAtTime(&tt);
        fprintf(stderr,"     %15.6lf %15.6lf %14.0lf%% %14.0lf%%\n",
                tt ,ot/1000000.0,
                time2hotjobpercent.value(tidx),
                time2spicyhotjobpercent.value(tidx));

    }
    fprintf(stderr,"\n\n");

    //
    // Thread Avgs/Stddev
    //
    fprintf(stderr,
    "-----------------------------------------------------------------\n");
    fprintf(stderr,"Thread Time Summary\n\n");
    fprintf(stderr,"    %10s %15s %15s %15s\n",
            "Thread", "AvgTime", "StdDev", "Max");

    QList<ThreadStat> stats = _thread_stats();
    foreach ( ThreadStat stat, stats ) {
        fprintf(stderr,"    %10d %15.6lf %15.6lf %15.6lf\n",
                stat.thread_id, stat.avg, stat.stdev, stat.max ) ;
    }
    fprintf(stderr,"\n\n");

    //
    // Get and sort stats
    //
    QList<QPair<Job*,double> > avgtimes;
    QList<QPair<Job*,double> > maxtimes;
    QList<QPair<Job*,double> > stddevtimes;
    foreach ( Job* job, _jobs )  {
        avgtimes.append(qMakePair(job,job->avg_runtime()));
        maxtimes.append(qMakePair(job,job->max_runtime()));
        stddevtimes.append(qMakePair(job,job->stddev_runtime()));
    }
    qSort(avgtimes.begin(), avgtimes.end(), doubleTimeGreaterThan);
    qSort(maxtimes.begin(), maxtimes.end(), doubleTimeGreaterThan);
    qSort(stddevtimes.begin(), stddevtimes.end(), doubleTimeGreaterThan);

    //
    // Job Time Avgs
    //
    fprintf(stderr,"------------------------------------------------\n");
    int njobs = 10;
    fprintf(stderr,"Top %d Job Avg Times\n\n", njobs);
    fprintf(stderr,"    %6s %15s %15s    %-40s\n",
            "Thread", "AvgTime", "Stddev", "Job");

    for ( int ii = 0 ; ii <  njobs; ii++ ) {
        QPair<Job*,double> avgtime = avgtimes.at(ii);
        Job* job = avgtime.first;
        fprintf(stderr,"    %6d %15.6lf %15.6lf     %-40s\n",
                job->thread_id(), avgtime.second,
                job->stddev_runtime(),
                job->job_name().toAscii().constData() );

    }
    fprintf(stderr,"\n\n");

    //
    // Job Time Stddev (most erratic)
    //
    fprintf(stderr,"------------------------------------------------\n");
    fprintf(stderr,"Top %d Job Stddev Times\n\n", njobs);
    fprintf(stderr,"    %6s %15s %15s    %-40s\n",
            "Thread","Stddev", "AvgTime","Job");

    for ( int ii = 0 ; ii <  njobs; ii++ ) {
        QPair<Job*,double> stddevtime = stddevtimes.at(ii);
        Job* job = stddevtime.first;
        fprintf(stderr,"    %6d %15.6lf %15.6lf     %-40s\n",
                job->thread_id(),
                job->stddev_runtime(),
                job->avg_runtime(),
                job->job_name().toAscii().constData() );

    }
    fprintf(stderr,"\n\n");


    //
    // Job Time Maxes
    //
    fprintf(stderr,"------------------------------------------------\n");
    fprintf(stderr,"Top %d Job Max Times\n\n", njobs);
    fprintf(stderr,"    %6s %10s     %-40s\n", "Thread", "MaxTime", "Job");
    for ( int ii = 0 ; ii <  njobs; ii++ ) {
        QPair<Job*,double> maxtime = maxtimes.at(ii);
        Job* job = maxtime.first;
        fprintf(stderr,"    %6d %10.6lf     %-40s\n",
                job->thread_id(), maxtime.second,
                job->job_name().toAscii().constData() );

    }
    fprintf(stderr,"\n\n");

    // Get map of sim objects to jobs
    QMap<QString,QList<Job*>* > simobj2jobs;
    foreach ( Job* job, _jobs ) {
        QString sobj = job->sim_object();
        QList<Job*>* jobs ;
        if ( simobj2jobs.contains(sobj) ) {
            jobs = simobj2jobs.value(sobj);
        } else {
            jobs = new QList<Job*>();
            simobj2jobs.insert(sobj,jobs);
        }
        jobs->append(job);
    }

    //
    // SimObject Time Avgs
    //
    fprintf(stderr,"------------------------------------------------\n");
    QMap<QString,double> simobj2avgtime;
    for ( int tidx = 0 ; tidx < npoints ; tidx++ ) {
        foreach ( QString simobj, simobj2jobs.keys() ) {
            QList<Job*>* jobs = simobj2jobs.value(simobj) ;
            double sum_simobj_rt = 0.0;
            foreach ( Job* job, *jobs ) {
                long rt = (long)job->runtime[tidx];
                sum_simobj_rt += rt;
            }
            if ( simobj2avgtime.contains(simobj) ) {
                simobj2avgtime[simobj] += sum_simobj_rt;
            } else {
                simobj2avgtime[simobj] = sum_simobj_rt;
            }
        }
    }
    foreach ( QString simobj, simobj2jobs.keys() ) {
        double avg_rt = simobj2avgtime.value(simobj)/npoints;
        simobj2avgtime[simobj] = avg_rt/1000000.0;
    }

    QList<QPair<QString,double> > simobj_avgtimes;
    foreach ( QString simobj, simobj2jobs.keys() ) {
        simobj_avgtimes.append(qMakePair(simobj,
                                          simobj2avgtime.value(simobj)));
    }
    qSort(simobj_avgtimes.begin(), simobj_avgtimes.end(),
          simObjTimeGreaterThan);

    // Number of sim objects to report
    int count = 10 ;
    if ( simobj_avgtimes.length() > 100 ) {
        count = simobj_avgtimes.length()/10;
    } else if ( simobj_avgtimes.length() < 10 ) {
        count = simobj_avgtimes.length();
    }

    // Format string to take length of simobject into account
    char format[256];
    int maxlen = 0 ;
    for ( int ii = 0; ii < count; ++ii) {
        QString sobj = simobj_avgtimes.at(ii).first;
        if ( sobj.length() > maxlen ) maxlen = sobj.length();
    }

    // Report
    fprintf(stderr,"Top Sim Object Avg Times (%d of %d simobjects)\n\n",
            count,simobj_avgtimes.length());
    sprintf(format,"    %%%ds %%15s\n",maxlen);
    fprintf(stderr,format,"SimObject","AvgTime");
    sprintf(format,"    %%%ds %%15.6lf\n",maxlen);
    for ( int ii = 0; ii < count; ++ii) {
        QString sobj = simobj_avgtimes.at(ii).first;
        double avgtime = simobj_avgtimes.at(ii).second;
        fprintf(stderr,format,sobj.toAscii().constData(),avgtime);
    }
    fprintf(stderr,"\n\n");

    foreach ( QString sobj, simobj2jobs.keys() ) {
        QList<Job*>* jobs = simobj2jobs.value(sobj);
        delete jobs;
    }

    //
    // Spike to Job Correlation
    //
    fprintf(stderr,"------------------------------------------------\n");
    fprintf(stderr,"Top 10 Spike Job Breakdown\n\n");
    for ( int ii = 0 ; ii < 10 ; ++ii ) {
        QPair<double,long> ov = ovs.at(ii);
        double tt = ov.first;
        double ot = ov.second;

        fprintf(stderr,"Spike at time %g of %g\n", tt ,ot/1000000);
        QList<QPair<Job*,long> > snaps = _jobtimes(tt);
        int tidx = _river_userjobs->getIndexAtTime(&tt);
        fprintf(stderr, "    %6s %15s     %-50s\n",
                        "Thread",
                        "Time",
                        "Job");
        double total = 0 ;

        for ( int jj = 0 ; jj < 10 ; ++jj ) {
            QPair<Job*,long> snap = snaps.at(jj);
            Job* job = snap.first;
            double t =  job->runtime[tidx]/1000000;
            double delta = 1.0e-3;
            if ( t < delta ) {
                continue;
            }

            char format[64];
            strcpy(format,"    %6d %15.3lf     %-50s\n");

            fprintf(stderr,format,
                    job->thread_id(),t,
                    job->job_name().toAscii().constData()) ;


            // limit printout of jobs
            if ( job->job_name() !=
                 QString("trick_sys.sched.advance_sim_time")) {
                total += t;
            }
            if ( total > 0.75*ot/1000000 && jj > 4 ) {
                break;
            }

        }
        fprintf(stderr,"\n");
    }

    //
    // Spike to Thread Correlation
    //
    fprintf(stderr,"------------------------------------------------\n");
    fprintf(stderr,"Top 10 Spike Thread Breakdown\n\n");
    for ( int ii = 0 ; ii < 10 ; ++ii ) {
        QPair<double,long> ov = ovs.at(ii);
        double tt = ov.first;
        double ot = ov.second;

        fprintf(stderr,"Spike at time %g of %g\n", tt,ot/1000000);
        fprintf(stderr, "    %6s %15s\n", "Thread", "Time");
        QList<QPair<int,long> > ttimes = _threadtimes(tt);
        for ( int ii = 0 ; ii < ttimes.length(); ++ii ) {
            QPair<int,long> ttime = ttimes.at(ii);
            double t = ttime.second/1000000.0;
            if ( t < 1.0e-3 ) {
                break;
            }
            fprintf(stderr,"    %6d %15.3lf\n", ttime.first ,t);
        }
        fprintf(stderr, "\n");
    }


}

Jobs::~Jobs()
{
    foreach ( Job* job, _jobs ) {
        delete job;
    }

    delete _river_userjobs;
}

QString Job::sim_object() const
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

    if ( npoints == 0 || timestamps == 0 || runtime == 0 ) {
        fprintf(stderr,"Job::_do_stats() called without setting:");
        fprintf(stderr," npoints or timestamps or runtime");
        fprintf(stderr," which come from TrickBinaryRiver\n");
        exit(-1);
    }


    long sum_rt = 0 ;
    long max_rt = 0 ;
    double sum_vv = 0.0;
    for ( int tidx = 0 ; tidx < npoints ; tidx++ ) {
        double time = timestamps[tidx];
        if ( time < 1.0 ) {
            continue;
        }
        long rt = (long)runtime[tidx];
        if ( rt > max_rt ) max_rt = rt;
        sum_rt += rt;

        _avg_runtime = ((double)sum_rt/(double)npoints);
        sum_vv += (rt-_avg_runtime)*(rt-_avg_runtime); // for stddev
    }

    _avg_runtime = (sum_rt/npoints)/1000000.0;
    _max_runtime = (max_rt)/1000000.0;
    _stddev_runtime = sqrt(sum_vv/npoints)/1000000.0 ;
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

double Job::stddev_runtime()
{
    _do_stats();
    return _stddev_runtime;
}

// Return list of thread_id => (avg,stddev) of exec time
QList<ThreadStat> Jobs::_thread_stats() const
{
    QList<ThreadStat> stats;

    QList<QList<QPair<int,long> > >* frametimes
            = new QList<QList<QPair<int,long> > >();

    int npoints = _river_frame->getNumPoints();
    QMap<int,long> sumtime;
    QMap<int,long> maxes;
    int count = 0 ;
    for ( int tidx = 0; tidx < npoints; ++tidx) {

        QMap<int,long> frames;
        foreach ( Job* job, _jobs ) {
            long rt = (long)(job->runtime[tidx]);
            int thread = job->thread_id();
            frames[thread] += rt;
            sumtime[thread] += rt;
        }

        // Save off frame time
        QList<QPair<int,long> > list_threads;
        foreach ( int thread, frames.keys() ) {
            list_threads.append(qMakePair(thread,frames.value(thread)));
        }
        frametimes->append(list_threads);

        // maxes
        if ( maxes.size() == 0 ) {
            foreach ( int thread, frames.keys() ) {
                maxes.insert(thread,frames.value(thread));
            }
        } else {
            foreach ( int thread, maxes.keys() ) {
                long max = maxes.value(thread);
                long frame = frames.value(thread);
                if ( frame > max ) {
                    maxes.insert(thread,frame);
                }
            }
        }

        count++;
    }

    // Calculate standard deviation
    QMap<int,double> stddevs;
    foreach ( int thread, sumtime.keys() ) {
        stddevs.insert(thread,0.0);
    }
    for ( int ii = 0; ii < frametimes->length(); ++ii) {
        QList<QPair<int,long> >  frametime = frametimes->at(ii);
        for ( int jj = 0; jj < frametime.length(); ++jj) {
            QPair<int,long>  threadtime = frametime.at(jj);
            int thread = threadtime.first;
            double rt = threadtime.second/1000000.0;
            double avg = (sumtime.value(thread)/count)/1000000.0;
            double vv = (avg-rt)*(avg-rt);
            stddevs[thread] += vv;
        }
    }
    foreach ( int thread, stddevs.keys() ) {
        stddevs[thread] = sqrt(stddevs.value(thread)/count);
    }

    foreach ( int thread, sumtime.keys() ) {
        ThreadStat stat;
        stat.thread_id = thread;
        stat.avg = (sumtime.value(thread)/count)/1000000.0;
        stat.max = (maxes.value(thread))/1000000.0;
        stat.stdev = stddevs.value(thread);
        stats.append(stat);
    }


    delete frametimes;

    return stats;
}

QList<QPair<int, long> > Jobs::_threadtimes(double t) const
{
    QList<QPair<int,long> > runtimes;
    QMap<int,long> sumtime;
    int tidx = _river_userjobs->getIndexAtTime(&t);
    foreach ( Job* job, _jobs ) {
        long rt = (long)(job->runtime[tidx]);
        int thread = job->thread_id();
        sumtime[thread] += rt;
    }
    foreach ( int thread, sumtime.keys() ) {
        runtimes.append(qMakePair(thread,sumtime.value(thread)));
    }
    qSort(runtimes.begin(), runtimes.end(), threadTimeGreaterThan);
    return runtimes;
}

QList<QPair<Job*, long> > Jobs::_jobtimes(double t) const
{
    QList<QPair<Job*,long> > runtimes;
    int tidx = _river_userjobs->getIndexAtTime(&t);
    foreach ( Job* job, _jobs ) {
        long rt = (long)job->runtime[tidx];
        runtimes.append(qMakePair(job,rt));
    }
    qSort(runtimes.begin(), runtimes.end(), jobTimeGreaterThan);
    return runtimes;
}

Job::Job(const QString& job_name,  // e.g. simbus.SimBus::read_ObcsRouter
         const QString& job_num,   // e.g. 1828.00
         int thread_id,            // e.g. 1
         int processor_id,         // e.g. 1
         double freq,              // e.g. 0.100
         double start,             // e.g. 0.0
         double stop,              // e.g. 1.0e37
         const QString& job_class, // e.g.scheduled
         bool is_enabled,          // e.g. 1
         int phase)                // e.g. 60000
    : npoints(0), timestamps(0), runtime(0),
      _job_name(job_name),_job_num(job_num), _thread_id(thread_id),
      _processor_id(processor_id),_freq(freq),_start(start),
      _stop(stop),_job_class(job_class),_is_enabled(is_enabled),
      _phase(phase),_is_stats(false)
{
}

// Parse long logname and set job members accordingly
// An example logname:
// JOB_schedbus.SimBus##read_ALDS15_ObcsRouter_C1.1828.00(read_simbus_0.100)
Job::Job(const QString& log_jobname) :
    _start(0),_stop(1.0e37),_is_enabled(true),_phase(60000),
    _is_stats(false)
{
    QString qname(log_jobname);

    qname.replace("::","##");
    qname.replace(QRegExp("^JOB_"),"");
    int idx1 = qname.lastIndexOf (QChar('('));
    int idx2 = qname.lastIndexOf (QChar(')'));
    int idx3 = qname.lastIndexOf (QChar('_'));
    if ( idx3 > idx1 && isdigit(qname.at(idx3+1).isDigit()) ) {
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

bool Jobs::_parse_s_job_execution(const QString &rundir)
{
    bool ret = true;

    QDir dir(rundir);
    if ( ! dir.exists() ) {
        qDebug() << "couldn't find run directory: " << rundir;
        ret = false;
        return(ret);
    }

    QString fname = rundir + QString("/S_job_execution");
    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "couldn't read file: " << fname;
        ret = false;
        return(ret);
    }

    QTextStream in(&file);
    int thread_id = -2;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if ( line.startsWith("=====") )  {
            thread_id = -1;
            continue;
        }
        if ( line.startsWith("Integration Loop") )  {
            thread_id = 0;
            continue;
        }
        if ( line.startsWith("Thread") )  {
            QStringList list = line.split(QRegExp("\\W+"));
            thread_id = list[1].toInt() ;
            continue;
        }
        if ( thread_id < 0 ) continue ;

        QStringList list =  line.split("|") ;
        if ( list.length() != 9 ) continue ;

        QString job_name; QString job_num; double freq; double start;
        double stop; QString job_class; bool is_enabled; int phase;
        int processor_id;

        for ( int ii = 0 ; ii < list.size(); ++ii) {
            QString str = list.at(ii).trimmed();
            switch (ii) {
            case 0: is_enabled = str.toInt() ? true : false; break;
            case 1: processor_id = str.toInt(); break;
            case 2: job_class = str; break;
            case 3: phase = str.toInt(); break;
            case 4: start = str.toDouble(); break;
            case 5: freq = str.toDouble(); break;
            case 6: stop = str.toDouble(); break;
            case 7: job_num = str; break;
            case 8: job_name = str; break;
            };
        }

        Job* job = new Job(job_name, job_num, thread_id, processor_id,
                            freq, start, stop, job_class, is_enabled,phase);

        _id_to_job[job->id()] = job;
        _jobs.append(job);
    }

    return ret;
}


//
// logfilename e.g. log_trickjobs.trk
//
bool Jobs::_parse_log_jobs(const QString &rundir,
                           const QString& logfilename,
                           TrickBinaryRiver** river,
                           double start, double stop)
{
    bool ret = true;

    QDir dir(rundir);
    if ( ! dir.exists() ) {
        qDebug() << "couldn't find run directory: " << rundir;
        exit(-1); // hard exit for now
        ret = false;
        return(ret);
    }

    QString trk = rundir + QString("/") + logfilename;
    QFile file(trk);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "couldn't read file: " << trk;
        exit(-1); // hard exit for now
        ret = false;
        return(ret);
    } else {
        file.close();
    }

    *river = new BoundedTrickBinaryRiver(trk.toAscii().data(),
                                         start,stop);
    std::vector<LOG_PARAM> params = (*river)->getParamList();
    for ( unsigned int ii = 1 ; ii < params.size(); ++ii ) {

        LOG_PARAM param = params.at(ii);

        Job* job = new Job(const_cast<char*>(param.data.name.c_str()));

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

        job->npoints = (*river)->getNumPoints();
        job->timestamps = (*river)->getTimeStamps();
        job->runtime = (*river)->getVals(const_cast<char*>
                                       (param.data.name.c_str()));
    }

    return ret;
}

bool dlGreaterThan(const QPair<double,long> &a,
                    const QPair<double,long> &b)
{
    return a.second > b.second;
}


QList<QPair<double, long> >
    Jobs::_parse_log_frame(const QString &rundir,double start, double stop)
{
    QList<QPair<double, long> > overruns;

    QDir dir(rundir);
    if ( ! dir.exists() ) {
        qDebug() << "couldn't find run directory: " << rundir;
        return(overruns);
    }

    QString trk = rundir + QString("/log_frame.trk");
    QFile file(trk);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "couldn't read file: " << trk;
        exit(-1);
        return(overruns);
    } else {
        file.close();
    }

    _river_frame = new BoundedTrickBinaryRiver(trk.toAscii().data(),
                                               start, stop);
    std::vector<LOG_PARAM> params = _river_frame->getParamList();
    int npoints = _river_frame->getNumPoints();
    double* timestamps = _river_frame->getTimeStamps();
    double* overrun = 0 ;
    QString param_overrun("real_time.rt_sync.frame_sched_time");
    for ( unsigned int ii = 1 ; ii < params.size(); ++ii ) {

        LOG_PARAM param = params.at(ii);

        QString qparam(param.data.name.c_str());
        if ( qparam ==  param_overrun ) {
            overrun = _river_frame->getVals(const_cast<char*>
                                             (param.data.name.c_str()));
            break;
        }
    }
    if ( overrun == 0 ) {
        // Shouldn't happen unless trick renames that param
        fprintf(stderr,"snap [error]: Couldn't find parameter "
                        "%s in log_frame.trk\n",
                param_overrun.toAscii().data());
        exit(-1);
    }

    for ( int ii = 0 ; ii < npoints ; ++ii ) {
        double tt = timestamps[ii];
        long ot = (long) overrun[ii];
        overruns.append(qMakePair(tt,ot));
    }

    qSort(overruns.begin(), overruns.end(), dlGreaterThan);

    return overruns;
}

QSet<int> Jobs::_thread_list()
{
    QSet<int> threads;
    foreach ( Job* job, _jobs ) {
        threads.insert(job->thread_id());
    }

    return threads;
}


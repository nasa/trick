#include "snap.h"


#include <stdio.h>
#include <ctype.h>
#include <cmath>
#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QDebug>

#include <stdio.h>

bool intLessThan(int a, int b)
{
    return a < b;
}

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

bool avgJobTimeGreaterThan(Job* a,Job* b)
{
    return a->avg_runtime() > b->avg_runtime();
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

bool long_int_GreaterThan(const QPair<long,int> &a,
                           const QPair<long,int> &b)
{
    return a.second > b.second;
}

static long _round_10(long a)
{
    long r = a;

    if ( a % 10 != 0 ) {
        for ( int ii = 1; ii < 100; ++ii) {
            r = a + ii;
            if ( r % 10 == 0 ) break;
            r = a - ii;
            if ( r % 10 == 0 ) break;
        }
    }

    return r;
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

BoundedTrickBinaryRiver *Jobs::_open_river(const QString &rundir,
                                             const QString &logfilename,
                                            double start, double stop)
{
    BoundedTrickBinaryRiver* river;

    QDir dir(rundir);
    if ( ! dir.exists() ) {
        qDebug() << "couldn't find run directory: " << rundir;
        exit(-1); // hard exit for now
    }

    QString trk = rundir + QString("/") + logfilename;
    QFile file(trk);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "couldn't read file: " << trk;
        exit(-1); // hard exit for now
    } else {
        file.close();
    }

    river = new BoundedTrickBinaryRiver(trk.toAscii().data(),
                                         start,stop);

    return river;
}

Jobs::Jobs(const QString &rundir, double start, double stop) :
    _start(start),_stop(stop)
{
    _river_trickjobs = _open_river(rundir,
                                   QString("log_trickjobs.trk"),
                                   start, stop);
    _river_userjobs = _open_river(rundir,
                                   QString("log_userjobs.trk"),
                                   start, stop);
    _river_frame = _open_river(rundir,
                                   QString("log_frame.trk"),
                                   start, stop);

    _process_job_river(_river_trickjobs);
    _process_job_river(_river_userjobs);

    QList<QPair<double, long> > ovs = _process_frame_river(_river_frame);

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
        long dt = _round_10(tt-ltime);
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
    QMap<int,QMap<int,long> >  threadtimes = _threadtimes() ;
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
        fprintf(stderr,"    %15.6lf %15.6lf %14.0lf%% %14.0lf%%\n",
                tt ,ot/1000000.0,
                time2hotjobpercent.value(tidx),
                time2spicyhotjobpercent.value(tidx));

    }
    fprintf(stderr,"\n\n");

    //
    // Thread Summary
    //
    fprintf(stderr,
    "-----------------------------------------------------------------\n");
    fprintf(stderr,"Thread Time Summary\n\n");
    fprintf(stderr,"    %10s %10s %15s %15s %15s %15s %15s\n",
            "Thread", "NumJobs", "Freq", "ThreadAvg", "AvgLoad%",
            "ThreadMax", "MaxLoad%");

    QMap<int,ThreadStat> threadstats = _thread_stats(threadtimes);
    QList<int> threadids = threadstats.keys();
    qSort(threadids.begin(), threadids.end(),intLessThan);
    foreach ( int threadid, threadids ) {
        ThreadStat stat = threadstats[threadid];
        double avg_percent = 0.0;
        double max_percent = 0.0;
        if ( stat.freq > 0.0000001 ) {
            avg_percent = 100.0*stat.avg/stat.freq;
            max_percent = 100.0*stat.max/stat.freq;
        }
        fprintf(stderr,"    %10d %10d %15.6lf %15.6lf "
                       "%14.0lf%% %15.6lf %14.0lf%%\n",
                 threadid,stat.hotjobs.length(),stat.freq,stat.avg,
                 avg_percent, stat.max, max_percent);
    }
    fprintf(stderr,"\n\n");

    //
    // Top Jobs Per Thread
    //
    fprintf(stderr,
    "-----------------------------------------------------------------\n");
    fprintf(stderr,"Top Jobs by Thread\n\n");
    fprintf(stderr,"    %8s %8s %15s %15s %10s%%     %-50s\n",
            "Thread", "NumJobs", "ThreadAvg", "JobAvgTime", "Percent", "JobName");
    foreach ( int threadid, threadids ) {
        ThreadStat stat = threadstats[threadid];
        fprintf(stderr,"    %8d %8d %15.6lf ",
                threadid,stat.hotjobs.length(),stat.avg) ;
        if ( stat.avg < 0.000005 && stat.hotjobs.length() > 1 ) {
            fprintf(stderr,"%15s  %10s     %-49s\n", "--","--","--");
            continue;
        }
        double sum = 0;
        for ( int ii = 0; ii < 5; ++ii) {
            Job* job = stat.hotjobs.at(ii) ;
            double percentage = 0.0;
            if ( stat.avg > 0.0000001 ) {
                if ( stat.hotjobs.length() == 1 ) {
                    // Fix round off error.
                    // If the job has an average above zero
                    // and the thread has a single job
                    // this job took 100%, so force it
                    percentage = 100.0;
                } else {
                    percentage = 100.0*job->avg_runtime()/stat.avg;
                }
            }
            sum += percentage;
            if ( (percentage < 1.0 || (sum > 98.0 && ii > 0)) &&
                  stat.hotjobs.length() > 1 ) {
                break;
            } else {
                if ( ii > 0 ) {
                    fprintf(stderr,"    %8s %8s %15s ", "","","");
                }
                fprintf(stderr,"%15.6lf ",job->avg_runtime());
                if ( stat.avg > 0.0000001 ) {
                    fprintf(stderr,"%10.0lf%%", percentage);
                } else {
                    fprintf(stderr," %10s", "--");
                }
                fprintf(stderr,"     %-50s\n", job->job_name().toAscii().data());
            }
            if ( ii == stat.hotjobs.length()-1 ) {
                break;
            }
        }
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
    fprintf(stderr,"    %15s %6s %15s %15s %15s    %-40s\n",
            "JobAvg", "Thread", "ThreadAvgTime",
            "JobStddev", "JobFreq", "JobName");

    for ( int ii = 0 ; ii <  njobs; ii++ ) {
        QPair<Job*,double> avgtime = avgtimes.at(ii);
        Job* job = avgtime.first;
        fprintf(stderr,"    %15.6lf %6d %15.6lf %15.6lf %15.6lf    %-40s\n",
                avgtime.second,
                job->thread_id(),
                threadstats[job->thread_id()].avg,
                job->stddev_runtime(),
                job->freq(),
                job->job_name().toAscii().constData() );

    }
    fprintf(stderr,"\n\n");

    //
    // Job Time Stddev (most erratic)
    //
    fprintf(stderr,"------------------------------------------------\n");
    fprintf(stderr,"Top %d Job Stddev Times\n\n", njobs);
    fprintf(stderr,"    %15s %6s %15s %15s %15s    %-40s\n",
            "JobStddev", "Thread","ThreadStddev", "JobAvgTime",
            "JobFreq","JobName");

    for ( int ii = 0 ; ii <  njobs; ii++ ) {
        QPair<Job*,double> stddevtime = stddevtimes.at(ii);
        Job* job = stddevtime.first;
        fprintf(stderr,"    %15.6lf %6d %15.6lf %15.6lf %15.6lf    %-40s\n",
                job->stddev_runtime(),
                job->thread_id(),
                threadstats[job->thread_id()].stdev,
                job->avg_runtime(),
                job->freq(),
                job->job_name().toAscii().constData() );

    }
    fprintf(stderr,"\n\n");


    //
    // Job Time Maxes
    //
    fprintf(stderr,"------------------------------------------------\n");
    fprintf(stderr,"Top %d Job Max Times\n\n", njobs);
    fprintf(stderr,"    %15s %15s %6s %15s %15s    %-40s\n",
                   "JobMax", "SimTime","Thread", "ThreadTime",
                   "JobFreq","JobName");
    for ( int ii = 0 ; ii <  njobs; ii++ ) {
        QPair<Job*,double> maxtime = maxtimes.at(ii);
        Job* job = maxtime.first;
        int tidx = job->max_timeidx();
        fprintf(stderr,"    %15.6lf %15.6lf %6d %15.6lf %15.6lf    %-40s\n",
                maxtime.second,
                _river_frame->getTimeStamps()[tidx],
                job->thread_id(),
                threadtimes[tidx].value(job->thread_id())/1000000.0,
                job->freq(),
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

        fprintf(stderr,"Spike at time %.4lf of %g\n", tt ,ot/1000000);
        QList<QPair<Job*,long> > snaps = _jobtimes(tt);
        int tidx = _river_userjobs->getIndexAtTime(&tt);
        fprintf(stderr, "    %6s %15s %15s %15s    %-50s\n",
                        "Thread",
                        "ThreadTime",
                        "JobTime",
                        "JobFreq",
                        "JobName");
        double total = 0 ;

        for ( int jj = 0 ; jj < 10 ; ++jj ) {
            QPair<Job*,long> snap = snaps.at(jj);
            Job* job = snap.first;
            double rt =  job->runtime[tidx]/1000000;
            double delta = 1.0e-3;
            if ( rt < delta ) {
                continue;
            }

            fprintf(stderr,
                    "    %6d %15.6lf %15.6lf %15.6lf    %-50s\n",
                    job->thread_id(),
                    threadtimes[tidx].value(job->thread_id())/1000000.0,
                    rt,
                    job->freq(),
                    job->job_name().toAscii().constData()) ;


            // limit printout of jobs
            if ( job->job_name() !=
                 QString("trick_sys.sched.advance_sim_time")) {
                total += rt;
            }
            if ( total > 0.75*ot/1000000.0 && jj > 4 ) {
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
        int tidx = _river_frame->getIndexAtTime(&(ov.first));
        double ot = ov.second;

        QList<QPair<int,long> > ttimes;
        QMap<int,long> threadid2runtime = threadtimes.value(tidx);
        QList<int> threadids = threadid2runtime.keys();
        foreach ( int threadid, threadids ) {
            ttimes.append(qMakePair(threadid,
                                    threadid2runtime.value(threadid)));
        }
        qSort(ttimes.begin(),ttimes.end(),threadTimeGreaterThan);

        fprintf(stderr,"Spike at time %g of %g\n", ov.first,ot/1000000);
        fprintf(stderr, "    %6s %15s\n", "Thread", "ThreadTime");
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
    delete _river_frame;
    delete _river_trickjobs;
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

    long freq;
    QMap<long,int> map_freq;
    long last_nonzero_timestamp = 0 ;

    long sum_rt = 0 ;
    long max_rt = 0 ;
    double sum_vv = 0.0;
    for ( int tidx = 0 ; tidx < npoints ; tidx++ ) {
        double time = timestamps[tidx];
        if ( time < 1.0 ) {
            continue;
        }
        long rt = (long)runtime[tidx];

        if ( tidx > 0 && rt > 0 ) {
            freq = _round_10((long)(time*1000000.0) - last_nonzero_timestamp);
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
            _max_tidx = tidx;
        }
        sum_rt += rt;

        _avg_runtime = ((double)sum_rt/(double)npoints);
        sum_vv += (rt-_avg_runtime)*(rt-_avg_runtime); // for stddev
    }

    _avg_runtime = (sum_rt/npoints)/1000000.0;
    _max_runtime = (max_rt)/1000000.0;
    _stddev_runtime = sqrt(sum_vv/npoints)/1000000.0 ;

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

int Job::max_timeidx()
{
    _do_stats();
    return _max_tidx;
}

double Job::stddev_runtime()
{
    _do_stats();
    return _stddev_runtime;
}

// Returns map(tidx)->map(thread_id,runtime)
QMap<int,QMap<int,long> >  Jobs::_threadtimes() const
{
    QMap<int,QMap<int,long> > tidx_2_thread2runtime;

    int npoints = _river_frame->getNumPoints();
    for ( int tidx = 0; tidx < npoints; ++tidx) {

        QMap<int,long> thread2runtime;
        foreach ( Job* job, _jobs ) {
            long rt = (long)(job->runtime[tidx]);
            int thread = job->thread_id();
            thread2runtime[thread] += rt;
        }

        tidx_2_thread2runtime[tidx] = thread2runtime;
    }

    return  tidx_2_thread2runtime;
}

// Input is map(tidx)->map(thread_id,runtime)
// Return list of thread_id => (avg,stddev,max rt.of thread)
QMap<int,ThreadStat> Jobs::_thread_stats(
        const QMap<int,QMap<int,long> >&  threadtimes) const
{
    QMap<int,ThreadStat> stats;

    QList<int> tidxs = threadtimes.keys();
    int tidx0 = tidxs.at(0);
    int ntidxs = tidxs.length();
    QList<int> threadids = threadtimes.value(tidx0).keys();

    QMap<int,long> sumtime;
    foreach ( int threadid, threadids ) {

        ThreadStat stat;
        stat.thread_id = threadid;
        stat.tidx_max = 0;
        stat.max = 0.0;

        sumtime[threadid] = 0 ;

        foreach ( int tidx, tidxs ) {
            long rt = threadtimes.value(tidx).value(threadid);
            sumtime[threadid] += rt;
            if ( rt > stat.max ) {
                stat.max = rt;
                stat.tidx_max = tidx;
            }
        }
        stat.max =  (double)stat.max/1000000.0;
        stat.avg =  (double)(sumtime.value(threadid))/(double)ntidxs/1000000.0;

        // Stddev
        foreach ( int tidx, tidxs ) {
            double rt = threadtimes.value(tidx).value(threadid)/1000000.0;
            double vv = (stat.avg-rt)*(stat.avg-rt);
            stat.stdev += vv;
        }
        stat.stdev = sqrt(stat.stdev/(double)ntidxs);

        // List of jobs per thread sorted by runtime
        foreach ( Job* job, _jobs ) {
            if ( job->thread_id() == threadid ) {
                stat.hotjobs.append(job);
            }
        }
        qSort(stat.hotjobs.begin(),stat.hotjobs.end(),avgJobTimeGreaterThan);

        // Frequency (cycle time) of thread
        stat.freq = 1.0e20;
        foreach ( Job* job, stat.hotjobs ) {
            if ( job->freq() < 0.000001 ) continue;
            if ( job->freq() < stat.freq ) {
                stat.freq = job->freq();
            }
        }
        if (stat.freq == 1.0e20) {
            stat.freq = 0.0;
        }

        stats[threadid] = stat;
    }

    return stats;
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

        QString job_name(""); QString job_num(""); double freq = 0.0; double start = 1.0;
        double stop = 1.0e20; QString job_class(""); bool is_enabled = true; int phase = 0;
        int processor_id = -1;

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
bool Jobs::_process_job_river( BoundedTrickBinaryRiver* river )
{
    bool ret = true;

    std::vector<LOG_PARAM> params = river->getParamList();
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

        job->npoints = river->getNumPoints();
        job->timestamps = river->getTimeStamps();
        job->runtime = river->getVals(const_cast<char*>
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
    Jobs::_process_frame_river(BoundedTrickBinaryRiver* river)
{
    QList<QPair<double, long> > overruns;

    std::vector<LOG_PARAM> params = river->getParamList();
    int npoints = river->getNumPoints();
    double* timestamps = river->getTimeStamps();
    double* overrun = 0 ;
    QString param_overrun("real_time.rt_sync.frame_sched_time");
    for ( unsigned int ii = 1 ; ii < params.size(); ++ii ) {

        LOG_PARAM param = params.at(ii);

        QString qparam(param.data.name.c_str());
        if ( qparam ==  param_overrun ) {
            overrun = river->getVals(const_cast<char*>
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



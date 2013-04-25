#include "blame.h"

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

Jobs::Jobs(const QString &rundir)
{
    //_parse_s_job_execution(rundir);
    _parse_log_userjobs(rundir);
    _parse_log_trickjobs(rundir);

    QList<QPair<double, long> > ovs = _parse_log_frame(rundir);

    //
    // Main Summary
    //
    fprintf(stderr,"\n");
    fprintf(stderr,"**************************************************\n");
    fprintf(stderr,"*                Snap! Results                   *\n");
    fprintf(stderr,"**************************************************\n");
    fprintf(stderr,"\n");

    fprintf(stderr,"Run dir = %s\n",rundir.toAscii().constData());
    fprintf(stderr,"Num jobs = %d\n",_jobs.length());
    fprintf(stderr,"Num frames = %d\n",ovs.length());

    //
    // Frame rate(s) (could be multiple e.g. 0.0120, 0.0125)
    //
    QList<long> rates;
    double* timestamps = _river_frame->getTimeStamps();
    long ltime = 0;
    int npoints = _river_userjobs->getNumPoints();
    for ( int ii = 0 ; ii < npoints ; ii++ ) {
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
    fprintf(stderr,"Frame rate(s) = %s\n",buf.toAscii().constData());

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
    fprintf(stderr,"Avg frame exec time = %.6lf\n",avg/1000000);

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
    fprintf(stderr,"Std dev for frame exec time = %.6lf\n",stddev/1000000);

    //
    // Thread summary
    //
    QSet<int> threads = _thread_list();
    fprintf(stderr,"Num threads = %d\n",threads.size());
    fprintf(stderr,"Thread list = ");
    int ii = 0 ;
    int ltid = 0;
    foreach ( int tid, threads ) {
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
    // Spike Summary
    //
    fprintf(stderr,"------------------------------------------------\n");
    fprintf(stderr,"Top 10 Spikes\n\n");
    fprintf(stderr,"    %10s %10s\n", "Time", "Spike");

    for ( int ii = 0 ; ii < 10 ; ++ii ) {
        QPair<double,long> ov = ovs.at(ii);
        double tt = ov.first;
        double ot = ov.second;
        fprintf(stderr,"     %10.6lf %10.6lf\n", tt ,ot/1000000.0);
    }
    fprintf(stderr,"\n\n");

    //
    // Thread Avgs/Stddev
    //
    fprintf(stderr,"------------------------------------------------\n");
    fprintf(stderr,"Thread Exec Time Summary\n\n");
    fprintf(stderr,"    %10s %15s %15s %15s\n",
            "Thread", "AvgTime", "StdDev", "Max");

    QList<ThreadStat> stats = thread_stats();
    foreach ( ThreadStat stat, stats ) {
        fprintf(stderr,"    %10d %15.6lf %15.6lf %15.6lf\n",
                stat.thread_id, stat.avg, stat.stdev, stat.max ) ;
    }
    fprintf(stderr,"\n\n");

    //
    // Job Exec Time Avgs
    //
    fprintf(stderr,"------------------------------------------------\n");
    int njobs = 10;
    fprintf(stderr,"Top %d Job Avg Exec Times\n\n", njobs);
    fprintf(stderr,"    %-4s %-10s %-40s\n", "Thread", "AvgTime", "Job");

    QList<QPair<Job*,double> > avgtimes;
    QList<QPair<Job*,double> > maxtimes;
    foreach ( Job* job, _jobs )  {
        long sum_rt = 0 ;
        long max_rt = 0 ;
        for ( int tidx = 0 ; tidx < npoints ; tidx++ ) {
            double time = job->timestamps[tidx];
            if ( time < 1.0 ) {
                continue;
            }
            long rt = (long)job->runtime[tidx];
            if ( rt > max_rt ) max_rt = rt;
            sum_rt += rt;
        }
        double avg_rt = sum_rt/npoints;
        avgtimes.append(qMakePair(job,avg_rt/1000000.0));
        maxtimes.append(qMakePair(job,max_rt/1000000.0));
    }
    qSort(avgtimes.begin(), avgtimes.end(), doubleTimeGreaterThan);
    for ( int ii = 0 ; ii <  njobs; ii++ ) {
        QPair<Job*,double> avgtime = avgtimes.at(ii);
        Job* job = avgtime.first;
        fprintf(stderr,"    %4d %10.6lf %-40s\n",
                job->thread_id(), avgtime.second,
                job->job_name().toAscii().constData() );

    }
    fprintf(stderr,"\n\n");

    //
    // Job Exec Time Maxes
    //
    fprintf(stderr,"------------------------------------------------\n");
    fprintf(stderr,"Top %d Job Max Exec Times\n\n", njobs);
    fprintf(stderr,"    %-4s %-10s %-40s\n", "Thread", "MaxTime", "Job");
    qSort(maxtimes.begin(), maxtimes.end(), doubleTimeGreaterThan);
    for ( int ii = 0 ; ii <  njobs; ii++ ) {
        QPair<Job*,double> maxtime = maxtimes.at(ii);
        Job* job = maxtime.first;
        fprintf(stderr,"    %4d %10.6lf %-40s\n",
                job->thread_id(), maxtime.second,
                job->job_name().toAscii().constData() );

    }
    fprintf(stderr,"\n\n");

    //
    // Spike to Job Correlation
    //
    fprintf(stderr,"------------------------------------------------\n");
    fprintf(stderr,"Top 10 Spike Job Breakdown\n\n");
    for ( int ii = 0 ; ii < 10 ; ++ii ) {
        QPair<double,long> ov = ovs.at(ii);
        double tt = ov.first;
        double ot = ov.second;

        // Skip initial spikes
        if ( tt < 1.0 ) {
            continue;
        }

        fprintf(stderr,"Spike at time %g of %g\n", tt ,ot/1000000);
        QList<QPair<Job*,long> > snaps = jobtimes(tt);
        int tidx = _river_userjobs->getIndexAtTime(&tt);
        fprintf(stderr, "    %-10s %-10s     %-50s\n",
                        "thread",
                        "time",
                        "job");
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
            strcpy(format,"    %-10d %-10.3lf     %-50s\n");

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

        // Skip initial spikes
        if ( tt < 1.0 ) {
            continue;
        }

        fprintf(stderr,"Spike at time %g of %g\n", tt,ot/1000000);
        fprintf(stderr, "    %-10s %-10s\n", "thread", "time");
        QList<QPair<int,long> > ttimes = threadtimes(tt);
        for ( int ii = 0 ; ii < ttimes.length(); ++ii ) {
            QPair<int,long> ttime = ttimes.at(ii);
            double t = ttime.second/1000000.0;
            if ( t < 1.0e-3 ) {
                break;
            }
            fprintf(stderr,"    %-10d %-10.3lf\n", ttime.first ,t);
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

// Return list of thread_id => (avg,stddev) of exec time
QList<ThreadStat> Jobs::thread_stats() const
{
    QList<ThreadStat> stats;

    QList<QList<QPair<int,long> > >* frametimes
            = new QList<QList<QPair<int,long> > >();

    double* tstamps = _river_frame->getTimeStamps();
    int npoints = _river_frame->getNumPoints();
    QMap<int,long> sumtime;
    QMap<int,long> maxes;
    int count = 0 ;
    for ( int tidx = 0; tidx < npoints; ++tidx) {

        // skip initial part of sim run
        if ( tstamps[tidx] < 1.0 ) {
            continue;
        }

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

QList<QPair<int, long> > Jobs::threadtimes(double t) const
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

QList<QPair<Job*, long> > Jobs::jobtimes(double t) const
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
    : _job_name(job_name),_job_num(job_num), _thread_id(thread_id),
      _processor_id(processor_id),_freq(freq),_start(start),
      _stop(stop),_job_class(job_class),_is_enabled(is_enabled),
      _phase(phase)
{
}

// Parse long logname and set job members accordingly
// An example logname:
// JOB_schedbus.SimBus##read_ALDS15_ObcsRouter_C1.1828.00(read_simbus_0.100)
Job::Job(const QString& log_jobname) :
    _start(0),_stop(1.0e37),_is_enabled(true),_phase(60000)
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

bool Jobs::_parse_log_userjobs(const QString &rundir)
{
    bool ret = true;

    QDir dir(rundir);
    if ( ! dir.exists() ) {
        qDebug() << "couldn't find run directory: " << rundir;
        ret = false;
        return(ret);
    }

    QString trk = rundir + QString("/log_userjobs.trk");
    QFile file(trk);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Latest version of Trick changed log_userjobs.trk name
        trk = rundir + QString("/log_frame_userjobs_main.trk");
        QFile file2(trk);
        if (!file2.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "couldn't read file: " << trk;
            exit(-1);
            return false;
        }
    } else {
        file.close();
    }

    _river_userjobs = new TrickBinaryRiver(trk.toAscii().data());
    std::vector<LOG_PARAM> params = _river_userjobs->getParamList();
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
            // Job (for reasons I don't understand) is not in S_job_execution
            _id_to_job[job_id] = job;
            _jobs.append(job);
        }

        job->npoints = _river_userjobs->getNumPoints();
        job->timestamps = _river_userjobs->getTimeStamps();
        job->runtime = _river_userjobs->getVals(const_cast<char*>
                                                 (param.data.name.c_str()));

        /*
        if ( job->job_name() == "v1553_rws_sim.v1553_TS21_125ms_job" ) {
            double avg = 0 ;
            double sum = 0 ;
            for ( int ii = 0 ; ii < job->npoints ; ++ii ) {
                double rt = job->runtime[ii]/1000000;
                if ( rt < 1 ) {
                    sum += rt;
                }
            }
            avg = sum/job->npoints;
            fprintf(stderr, "Avg runtime for v1553_rws job is %g\n", avg);
        }
        */
    }

    return ret;
}

bool Jobs::_parse_log_trickjobs(const QString &rundir)
{
    bool ret = true;

    QDir dir(rundir);
    if ( ! dir.exists() ) {
        qDebug() << "couldn't find run directory: " << rundir;
        ret = false;
        return(ret);
    }

    QString trk = rundir + QString("/log_trickjobs.trk");
    QFile file(trk);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "couldn't read file: " << trk;
        ret = false;
        return(ret);
    } else {
        file.close();
    }

    _river_trickjobs = new TrickBinaryRiver(trk.toAscii().data());
    std::vector<LOG_PARAM> params = _river_trickjobs->getParamList();
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

        job->npoints = _river_trickjobs->getNumPoints();
        job->timestamps = _river_trickjobs->getTimeStamps();
        job->runtime = _river_trickjobs->getVals(const_cast<char*>
                                                 (param.data.name.c_str()));
    }

    return ret;
}

bool dlGreaterThan(const QPair<double,long> &a,
                    const QPair<double,long> &b)
{
    return a.second > b.second;
}


QList<QPair<double, long> > Jobs::_parse_log_frame(const QString &rundir)
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
        return(overruns);
    } else {
        file.close();
    }

    _river_frame = new TrickBinaryRiver(trk.toAscii().data());
    std::vector<LOG_PARAM> params = _river_frame->getParamList();
    QString param_overrun("real_time.rt_sync.frame_sched_time");
                          //real_time.rt_sync.frame_overrun_time");
    int npoints = _river_frame->getNumPoints();
    double* timestamps = _river_frame->getTimeStamps();
    double* overrun = 0 ;
    for ( unsigned int ii = 1 ; ii < params.size(); ++ii ) {

        LOG_PARAM param = params.at(ii);

        QString qparam(param.data.name.c_str());
        if ( qparam !=  param_overrun ) {
            continue;
        }

        overrun = _river_frame->getVals(const_cast<char*>
                                        (param.data.name.c_str()));
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


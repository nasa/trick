/*
 * Snap! Crackle! Pop!
 */

#include "snap.h"


#include <stdio.h>
#include <ctype.h>
#include <cmath>
#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QDebug>

#include <stdio.h>

QString FrameStat::frame_time_name = "real_time.rt_sync.frame_sched_time";
QString FrameStat::overrun_time_name = "real_time.rt_sync.frame_overrun_time";
int FrameStat::num_overruns = 0 ;

bool intLessThan(int a, int b)
{
    return a < b;
}

bool simObjectAvgTimeGreaterThan(const SimObject& a, const SimObject& b)
{
    return a.avg_runtime > b.avg_runtime;
}

bool jobAvgTimeGreaterThan(Job* a,Job* b)
{
    return a->avg_runtime() > b->avg_runtime();
}

bool jobMaxTimeGreaterThan(Job* a,Job* b)
{
    return a->max_runtime() > b->max_runtime();
}

bool frameTopJobsGreaterThan(const QPair<double,Job*>& a,
                           const QPair<double,Job*>& b)
{
    return a.first > b.first;
}

bool topThreadGreaterThan(const QPair<double,ThreadStat>& a,
                         const QPair<double,ThreadStat>& b)
{
    if ( a.second.freq > 0.000001 && b.second.freq > 0.000001 ) {
        return (a.first/a.second.freq) > (b.first/b.second.freq);
    } else {
        return (a.first > b.first);
    }
}

bool frameTimeGreaterThan(const FrameStat& a,const FrameStat& b)
{
    return a.frame_time > b.frame_time;
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

BoundedTrickBinaryRiver *Snap::_open_river(const QString &rundir,
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

Snap::Snap(const QString &irundir, double istart, double istop) :
    _rundir(irundir), _start(istart),_stop(istop), _is_realtime(false),
    _frame_avg(0.0),_frame_stddev(0),_curr_sort_method(NoSort)
{
    _process_rivers();        // _jobs list and _frame_stats created
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

    delete _river_userjobs;
    delete _river_frame;
    delete _river_trickjobs;

    delete _threads;
    delete _sim_objects;
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

    _frame_stats = _process_frame_river(_river_frame);
    qSort(_frame_stats.begin(),_frame_stats.end(),frameTimeGreaterThan);

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

// TODO: Untested.  This was going to be used for S_job_execution,
//       but that was broken.  Keeping this in case snap eventually
//       uses S_job_execution.
Job::Job(BoundedTrickBinaryRiver *river,
         const QString& job_name,  // e.g. simbus.SimBus::read_ObcsRouter
         const QString& job_num,   // e.g. 1828.00
         int thread_id,            // e.g. 1
         int processor_id,         // e.g. 1
         double freq,              // e.g. 0.100
         double start,             // e.g. 0.0
         double stop,              // e.g. 1.0e37
         const QString& job_class, // e.g.scheduled
         bool is_enabled,          // e.g. 1
         int phase)                // e.g. 60000
    : _river(river),_npoints(0), _timestamps(0), _runtime(0),
      _job_name(job_name),_job_num(job_num), _thread_id(thread_id),
      _processor_id(processor_id),_freq(freq),_start(start),
      _stop(stop),_job_class(job_class),_is_enabled(is_enabled),
      _phase(phase),_is_stats(false)
{
    if ( river == 0 ) {
        fprintf(stderr,"This code is not ready to use.\n");
        fprintf(stderr,"It's eventual use is for S_job_execution.\n");
        fprintf(stderr,"Exiting!!!\n");
        exit(-1);
    }
    _npoints = river->getNumPoints();
    _timestamps = river->getTimeStamps();
    QString logged_jobname = id();
    _runtime = river->getVals(logged_jobname.toAscii().constData());
}

// Parse long logname and set job members accordingly
// An example logname:
// JOB_schedbus.SimBus##read_ALDS15_ObcsRouter_C1.1828.00(read_simbus_0.100)
Job::Job(BoundedTrickBinaryRiver *river, const char* log_jobname) :
    _river(river),_start(0),_stop(1.0e37),_is_enabled(true),_phase(60000),
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

bool Snap::_parse_s_job_execution(const QString &rundir)
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

        QString job_name("");
        QString job_num("");
        double freq = 0.0;
        double start = 1.0;
        double stop = 1.0e20;
        QString job_class("");
        bool is_enabled = true;
        int phase = 0;
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

        Job* job = new Job(0, job_name, job_num, thread_id, processor_id,
                            freq, start, stop, job_class, is_enabled,phase);

        _id_to_job[job->id()] = job;
        _jobs.append(job);
    }

    return ret;
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
    for ( int ii = 0 ; ii < _frame_stats.length() ; ++ii ) {
        sum += _frame_stats.at(ii).frame_time;
    }

    _frame_avg = sum/_frame_stats.length() ;
}

void Snap::_calc_frame_stddev()
{
    if ( _frame_avg == 0.0 ) {
        _calc_frame_avg();
    }

    double sum = 0.0 ;
    for ( int ii = 0 ; ii < _frame_stats.length() ; ++ii ) {
        double frametime =  _frame_stats.at(ii).frame_time;
        double vv = (frametime-_frame_avg)*(frametime-_frame_avg);
        sum += vv;
    }

    _frame_stddev = sqrt(sum/_frame_stats.length()) ;
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

bool dlGreaterThan(const FrameStat &a, const FrameStat &b)
{
    return a.frame_time > b.frame_time;
}


QList<FrameStat> Snap::_process_frame_river(BoundedTrickBinaryRiver* river)
{
    QList<FrameStat> framestats;

    std::vector<LOG_PARAM> params = river->getParamList();
    int npoints = river->getNumPoints();
    double* timestamps = river->getTimeStamps();
    double* frame_times = 0 ;
    double* overrun_times = 0 ;
    int cnt = 0 ;
    for ( unsigned int ii = 1 ; ii < params.size(); ++ii ) {

        LOG_PARAM param = params.at(ii);

        QString qparam(param.data.name.c_str());

        if ( qparam ==  FrameStat::frame_time_name ) {
            frame_times = river->getVals(const_cast<char*>
                                             (param.data.name.c_str()));
            cnt++;
        } else if ( qparam == FrameStat::overrun_time_name ) {
            overrun_times = river->getVals(const_cast<char*>
                                             (param.data.name.c_str()));
            cnt++;
        }
        if ( cnt == 2 ) {
            break;
        }
    }
    if ( frame_times == 0 ) {
        // Shouldn't happen unless trick renames that param
        fprintf(stderr,"snap [error]: Couldn't find parameter "
                        "%s in %s\n",
                FrameStat::frame_time_name.toAscii().data(),
                river->getFileName());
        exit(-1);
    }
    if ( overrun_times == 0 ) {
        // Shouldn't happen unless trick renames that param
        fprintf(stderr,"snap [error]: Couldn't find parameter "
                        "%s in %s\n",
                FrameStat::overrun_time_name.toAscii().data(),
                river->getFileName());
        exit(-1);
    }

    _is_realtime = false;
    FrameStat::num_overruns = 0;
    for ( int tidx = 0 ; tidx < npoints ; ++tidx ) {
        FrameStat framestat;
        double tt = timestamps[tidx];
        framestat.timeidx = tidx;
        framestat.timestamp = tt;
        framestat.frame_time = frame_times[tidx]/1000000.0;
        framestat.overrun_time = overrun_times[tidx]/1000000.0;
        if ( framestat.overrun_time > 0.0 ) {
            FrameStat::num_overruns++;
        }
        if ( !_is_realtime && frame_times[tidx] != 0 ) {
            _is_realtime = true;
        }

        // Job load percentage (a lot of jobs running hot)
        double hcnt = 0.0 ;
        double shcnt = 0.0 ;
        foreach ( Job* job, _jobs ) {
            double rt = job->runtime()[tidx]/1000000.0;
            if ( rt > job->avg_runtime()+job->stddev_runtime() ) {
                hcnt += 1.0;
                if ( rt > job->avg_runtime()+2*job->stddev_runtime() ) {
                    shcnt += 1.0;
                }
            }

            // List of top jobs with most runtime for the frame
            int len = framestat.topjobs.length() ;
            if ( len < 10 ) {
                framestat.topjobs.append(qMakePair(rt,job));
            } else {
                QPair<double,Job*> ljob = framestat.topjobs.last();
                double lrt = ljob.second->runtime()[tidx]/1000000.0;
                if ( rt > lrt ) {
                    framestat.topjobs.replace(len-1,qMakePair(rt,job));
                    qSort(framestat.topjobs.begin(),
                          framestat.topjobs.end(),
                          frameTopJobsGreaterThan);
                }
            }
        }
        framestat.hotjobpercentage = 100.0*hcnt/(double)num_jobs();
        framestat.spicyhotjobpercentage = 100.0*shcnt/(double)num_jobs();
        qSort(framestat.topjobs.begin(),
              framestat.topjobs.end(),
              frameTopJobsGreaterThan);

        framestats.append(framestat);
    }

    qSort(framestats.begin(), framestats.end(), dlGreaterThan);


    return framestats;
}

void ThreadStat::_do_stats()
{
    if ( jobs.size() == 0 ) {
        return;
    }

    qSort(jobs.begin(),jobs.end(),jobAvgTimeGreaterThan);

    // Frequency (cycle time) of thread
    freq = 1.0e20;
    foreach ( Job* job, jobs ) {
        if ( job->freq() < 0.000001 ) continue;
        if ( job->freq() < freq ) {
            freq = job->freq();
        }
    }
    if (freq == 1.0e20) {
        freq = 0.0;
    }

    Job* job0 = jobs.at(0);
    int npoints = job0->npoints();
    double* t = job0->timestamps();
    double tnext = t[0] + freq;
    double sum_time = 0.0;
    double frame_time = 0.0;
    max_runtime = 0.0;
    int last_frameidx = 0 ;
    bool is_frame_change = true;
    int nframes = 0 ;
    for ( int tidx = 0; tidx < npoints; ++tidx) {

        if ( freq < 0.000001 ) {
            is_frame_change = true;
        } else if ( tnext < t[tidx] ) {
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
            nframes++;
            frame_time = 0.0;
            last_frameidx = tidx;
            is_frame_change = false;
        }

        foreach ( Job* job, jobs ) {
            frame_time += job->runtime()[tidx];
        }
    }
    max_runtime /= 1000000.0;
    avg_runtime = sum_time/nframes/1000000.0;
    if ( freq > 0.0000001 ) {
        avg_load = 100.0*avg_runtime/freq;
        max_load = 100.0*max_runtime/freq;
    }
    if ( thread_id == 0 ) {
        // the thread 0 overrun count doesn't take into account
        // the executive overhead... just the sum of job runtimes
        // so override the overrun count for thread 0
        num_overruns = FrameStat::num_overruns;
    }

    // Stddev
    foreach ( int tidx, _frameidx2runtime.keys() ) {
        double rt = _frameidx2runtime[tidx];
        double vv = (avg_runtime-rt)*(avg_runtime-rt);
        stdev += vv;
    }
    stdev = sqrt(stdev/nframes);

}

double ThreadStat::runtime(int tidx) const
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

double ThreadStat::runtime(double timestamp) const
{
    int tidx = jobs.at(0)->river()->getIndexAtTime(&timestamp);
    double rt = runtime(tidx);
    return rt;
}

int ThreadStat::nframes() const
{
    return _frameidx2runtime.keys().size();
}

double ThreadStat::avg_job_runtime(Job *job) const
{
    return job->avg_runtime()*job->npoints()/nframes();
}

//
// Returns a percent average load of job on thread
// A load of 0.0 can mean it's negligible
// If there's only one job on a thread, load is 100% if there's any load
//
double ThreadStat::avg_job_load(Job *job) const
{
    double load = 0.0;

    if ( avg_runtime > 0.000001 ) {
        if ( jobs.length() == 1 ) {
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

Threads::Threads(const QList<Job*>& jobs) : _jobs(jobs)
{
    foreach ( Job* job, _jobs ) {

        int tid = job->thread_id();
        if ( ! _ids.contains(tid) ) {
            _ids.append(tid);
            ThreadStat* stat = new ThreadStat();
            stat->thread_id = tid;
            _threads.insert(tid,stat);
        }

        ThreadStat* stat = _threads.value(tid);
        stat->jobs.append(job);
    }

    qSort(_ids.begin(),_ids.end(),intLessThan);

    foreach ( ThreadStat* stat, _threads.values() ) {
        stat->_do_stats();
    }

}

Threads::~Threads()
{
    foreach ( ThreadStat* stat, _threads.values() ) {
        delete stat;
    }
}

ThreadStat Threads::get(int id) const
{
    return *(_threads[id]);
}

QList<ThreadStat> Threads::list() const
{
    QList<ThreadStat> list ;

    foreach ( ThreadStat* stat, _threads.values() ) {
        list.append(*stat);
    }

    return list;
}

QList<int> Threads::ids() const
{
    return _ids;
}

SimObject::SimObject(const QString& name ) : _name(name)
{
}

void SimObject::_do_stats()
{
    if ( jobs.size() == 0 ) {
        return;
    }

    qSort(jobs.begin(),jobs.end(),jobAvgTimeGreaterThan);

    int npoints = jobs.at(0)->npoints();
    double sum_time = 0.0;
    for ( int tidx = 0; tidx < npoints; ++tidx) {
        foreach ( Job* job, jobs ) {
            sum_time += job->runtime()[tidx];
        }
    }
    avg_runtime = sum_time/(double)npoints/1000000.0;
}

SimObjects::SimObjects(const QList<Job *> &jobs) : _jobs(jobs)
{
    foreach ( Job* job, _jobs ) {

        QString simobject_name = job->sim_object();
        if ( ! _sim_objects.keys().contains(simobject_name) ) {
            SimObject* sobject = new SimObject(simobject_name);
            _sim_objects.insert(simobject_name,sobject);
        }

        SimObject* sobject = _sim_objects.value(simobject_name);
        sobject->jobs.append(job);
    }

    foreach ( SimObject* sobject, _sim_objects.values() ) {
        sobject->_do_stats();
    }
}

SimObjects::~SimObjects()
{
    foreach ( SimObject* sobject, _sim_objects.values() ) {
        delete sobject;
    }
}

QList<SimObject> SimObjects::list() const
{
    QList<SimObject> list ;

    foreach ( SimObject* sobject, _sim_objects.values() ) {
        list.append(*sobject);
    }

    qSort(list.begin(),list.end(), simObjectAvgTimeGreaterThan);

    return list;
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
    const QList<FrameStat>* frame_stats = _snap.frame_stats();
    rpt += divider;
    rpt += QString("Top Spikes\n\n");
    rpt += str.sprintf("    %15s %15s %15s %15s\n",
                      "Time", "Spike", "HotJob%", "SpicyHotJob%");
    cnt = 0 ;
    foreach ( FrameStat frame, *frame_stats ) {
        if ( ++cnt > max_cnt ) break;
        double tt = frame.timestamp;
        double ft = frame.frame_time;
        rpt += str.sprintf("    %15.6lf %15.6lf %14.0lf%% %14.0lf%%\n",
                          tt ,ft,
                          frame.hotjobpercentage,
                          frame.spicyhotjobpercentage);

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

    QList<ThreadStat> threads = _snap.thread_stats()->list() ;
    foreach ( ThreadStat stat, threads ) {
        rpt += str.sprintf("    %10d %10d %15.6lf %15d %15.6lf "
                          "%14.0lf%% %15.6lf %14.0lf%%\n",
                 stat.thread_id,stat.jobs.length(),stat.freq,
                 stat.num_overruns, stat.avg_runtime,
                 stat.avg_load, stat.max_runtime, stat.max_load);
    }
    rpt += endsection;

    //
    // Top Jobs Per Thread
    //
    rpt += divider;
    rpt += QString("Top Jobs by Thread\n\n");
    rpt += str.sprintf("    %8s %8s %15s %15s %10s%%     %-50s\n",
           "Thread", "NumJobs", "ThreadAvg", "JobAvgTime", "Percent", "JobName");
    foreach ( ThreadStat stat, threads ) {

        rpt += str.sprintf("    %8d %8d %15.6lf ",
                   stat.thread_id,stat.jobs.length(),stat.avg_runtime) ;

        if ( stat.avg_runtime < 0.000005 && stat.jobs.length() > 1 ) {
            rpt += str.sprintf("%15s  %10s     %-49s\n", "--","--","--");
            continue;
        }

        double sum = 0;
        for ( int ii = 0; ii < 5; ++ii) {
            Job* job = stat.jobs.at(ii) ;
            double load = stat.avg_job_load(job);
            sum += load;
            if ( (load < 0.01 || (sum > 98.0 && ii > 0)) &&
                  stat.jobs.length() > 1 ) {
                break;
            } else {
                if ( ii > 0 ) {
                    rpt += str.sprintf("    %8s %8s %15s ", "","","");
                }
                rpt += str.sprintf("%15.6lf ",stat.avg_job_runtime(job));
                if ( stat.avg_runtime > 0.0000001 ) {
                    rpt += str.sprintf("%10.0lf%%", load);
                } else {
                    rpt += str.sprintf(" %10s", "--");
                }
                rpt += str.sprintf("     %-50s\n",
                                   job->job_name().toAscii().constData());
            }
            if ( ii == stat.jobs.length()-1 ) {
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
                   _snap.thread_stats()->get(job->thread_id()).avg_runtime,
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
                _snap.thread_stats()->get(job->thread_id()).
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
    format.sprintf("    %%%ds %%15s\n",maxlen);
    rpt += str.sprintf(TXT(format),"SimObject","AvgTime");
    format.sprintf("    %%%ds %%15.6lf\n",maxlen);
    cnt = 0 ;
    foreach ( SimObject sobject, simobjects ) {
        if ( ++cnt > max_cnt ) break;
        rpt += str.sprintf(TXT(format),TXT(sobject.name()),sobject.avg_runtime);
    }
    max_cnt = 10;
    rpt += endsection;

    //
    // Spike to Job Correlation
    //
    rpt += divider;
    rpt += QString("Top Spikes Job Breakdown\n\n");
    cnt = 0 ;
    foreach ( FrameStat frame, *frame_stats ) {

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
                    _snap.thread_stats()->get(job->thread_id()).runtime(tidx),
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
    foreach ( FrameStat frame, *frame_stats ) {

        if ( ++cnt > max_cnt ) break;

        int tidx =  frame.timeidx;
        double tt = frame.timestamp;
        double ft = frame.frame_time;

        QList<QPair<double, ThreadStat> > topthreads;
        foreach ( ThreadStat thread, threads ) {
            topthreads.append(qMakePair(thread.runtime(tidx),thread));
        }
        qSort(topthreads.begin(),topthreads.end(),topThreadGreaterThan);

        rpt += str.sprintf("Spike at time %g of %g\n",tt,ft);
        rpt += str.sprintf("    %6s %15s %15s\n", "Thread", "ThreadFreq",
                                            "ThreadTime");
        for ( int ii = 0 ; ii < topthreads.length(); ++ii ) {
            QPair<double,ThreadStat> topthread = topthreads.at(ii);
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

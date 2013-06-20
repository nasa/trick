#ifndef BLAME_H
#define BLAME_H

#include <QString>
#include <QList>
#include <QMap>
#include <QSet>
#include <QPair>
#include <QMultiMap>
#include <QtAlgorithms>

#include "TrickBinaryRiver.hh"

class Job;

class BoundedTrickBinaryRiver : public TrickBinaryRiver {
  public:
     BoundedTrickBinaryRiver(char* filename, double start, double stop);

     int start() { return _start ; }
     int stop() { return _stop ; }

     int getNumPoints() { return _npoints ; }
     double* getTimeStamps() { return _timestamps ; }
     double* getVals(const char* param) { return _vals.value(param) ; }

  private:
     BoundedTrickBinaryRiver(
                    char* file_name,
                    vector<Transformation>& transforms=empty_transforms) :
         TrickBinaryRiver(file_name,transforms){}

     double _start;
     double _stop;
     int _npoints;
     double* _timestamps;
     QMap<const char*,double*> _vals;
};


class FrameStat
{
  public:
    FrameStat() : frame_time(0), overrun_time(0), is_overrun(false)
    {}

    static QString frame_time_name;
    static QString overrun_time_name;
    static int num_overruns;
    double frame_time;
    double overrun_time;
    bool is_overrun;
};


class ThreadStat
{
  public:
    ThreadStat() : thread_id(0), njobs(0), avg(0),
                   tidx_max(0),max(0), stdev(0),freq(0.0),
                   num_overruns(0) {}
    int thread_id;
    int njobs;
    double avg;
    int tidx_max;
    double max ;
    double stdev;
    double freq;         // assume freq is freq of highest freq job on the thread
    QList<Job*> hotjobs; // sorted job list on thread with highest avg run times
    int num_overruns;
};

class Threads
{
  public:
    Threads(QList<Job*> jobs);
    QSet<int> ids();

  private:
    QList<Job*> _jobs;
    QSet<int> _ids;
    QSet<int> _calc_ids(const QList<Job *> &jobs);
};

class Job
{
public:
    Job(const QString& job_name,  // e.g. simbus.SimBus::read_ObcsRouter
        const QString& job_num,   // e.g. 1828.00
        int thread_id,            // e.g. 1
        int processor_id,         // e.g. 1
        double freq,              // e.g. 0.100
        double start,             // e.g. 0.0
        double stop,              // e.g. 1.0e37
        const QString& job_class, // e.g.scheduled
        bool is_enabled,          // e.g. 1
        int phase);                // e.g. 60000

    // job_id is logged job name
    // e.g. JOB_bus.SimBus##read_ObcsRouter_C1.1828.00(read_simbus_0.100)
    Job(const QString& job_id);

    QString id() const; // logged jobname
    QString job_num() const { return _job_num; }
    QString job_name() const { return _job_name; }
    QString sim_object() const ;
    int thread_id() const { return _thread_id; }
    int processor_id() const { return _processor_id;}
    double freq() const { return _freq ;}
    double start() const { return _start ;}
    double stop()  const{ return _stop ;}
    QString job_class() const { return _job_class ;}
    bool is_enabled() const { return _is_enabled ;}
    int phase() const { return _phase ;}

    double avg_runtime();
    double max_runtime();
    int max_timeidx();
    double stddev_runtime();

    // Log jobs info
    int npoints;
    double* timestamps;
    double* runtime;

private:
    Job() {}
    QString _job_name;
    QString _job_num;   // e.g. 1831.04
    int _thread_id;
    int _processor_id;
    double _freq;
    double _start;
    double _stop;
    QString _job_class;
    bool _is_enabled;
    int _phase;

    bool _is_stats;
    void _do_stats();
    double _avg_runtime;
    double _stddev_runtime;
    double _max_runtime;
    int _max_tidx;
};

class Jobs
{
public:
    Jobs(const QString& rundir, double start, double stop);
    ~Jobs();

private:
    Jobs() {}

    QList<Job*> _jobs;
    QString _job_logname(const Job* job) const;
    QMap<QString,Job*> _id_to_job;

    bool _parse_s_job_execution(const QString& rundir);
    bool _process_job_river( BoundedTrickBinaryRiver *river );
    QList<QPair<double, FrameStat> > _process_frame_river(
                                    BoundedTrickBinaryRiver *river );


    BoundedTrickBinaryRiver* _river_userjobs;
    BoundedTrickBinaryRiver* _river_frame;
    BoundedTrickBinaryRiver* _river_trickjobs;

    QMap<int, ThreadStat> _thread_stats(
        const QMap<int,QMap<int,long> >&  threadtimes) const;
    QMap<int,QMap<int,long> >  _threadtimes() const; // tidx->(tid->thread_rt)
    QList<QPair<Job *, long> > _jobtimes(double t) const ;

    void _rpt_summary();

    double _start;
    double _stop;

    BoundedTrickBinaryRiver* _open_river(const QString& rundir,
                                          const QString& logfilename,
                                          double start, double stop);
};

#endif // BLAME_H

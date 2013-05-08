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
class ThreadStat
{
  public:
    ThreadStat() : thread_id(0), njobs(0), avg(0), max(0), stdev(0) {}
    int thread_id;
    int njobs;
    double avg;
    double max;
    double stdev;
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
};

class Jobs
{
public:
    Jobs(const QString& rundir);
    ~Jobs();
    QList<QPair<int, long> > threadtimes(double t) const;
    QList<ThreadStat> thread_stats() const;
    QList<QPair<Job *, long> > jobtimes(double t) const ;

private:
    Jobs() {}

    QList<Job*> _jobs;
    QString _job_logname(const Job* job) const;
    QMap<QString,Job*> _id_to_job;

    bool _parse_s_job_execution(const QString& rundir);
    bool _parse_log_userjobs(const QString& rundir);
    bool _parse_log_trickjobs(const QString &rundir);
    QList<QPair<double, long> >  _parse_log_frame(const QString& rundir);

    QSet<int> _thread_list();

    TrickBinaryRiver* _river_userjobs;
    TrickBinaryRiver* _river_frame;
    TrickBinaryRiver* _river_trickjobs;

    void _rpt_summary();

};

#endif // BLAME_H

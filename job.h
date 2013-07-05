#ifndef JOB_H
#define JOB_H

#include <QString>

#include "boundedtrickbinaryriver.h"

class Job;

bool jobAvgTimeGreaterThan(Job* a,Job* b);
bool jobMaxTimeGreaterThan(Job* a,Job* b);

class Job
{
  public:
    Job(BoundedTrickBinaryRiver* river,
        const QString& job_name,  // e.g. simbus.SimBus::read_ObcsRouter
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
    Job(BoundedTrickBinaryRiver* river, const char *job_id);

    QString id() const; // logged jobname
    QString job_num() const { return _job_num; }
    QString job_name() const { return _job_name; }
    QString sim_object_name() const ;
    int thread_id() const { return _thread_id; }
    int processor_id() const { return _processor_id;}
    double freq() ;
    double start() const { return _start ;}
    double stop()  const{ return _stop ;}
    QString job_class() const { return _job_class ;}
    bool is_enabled() const { return _is_enabled ;}
    int phase() const { return _phase ;}

    double avg_runtime();
    double max_runtime();
    double max_timestamp();
    double stddev_runtime();

    inline double* runtime() const { return _runtime; }
    inline double* timestamps() const { return _timestamps; }
    inline int npoints() const { return _npoints; }

  private:
    Job() {}

    int _npoints;
    double* _timestamps;
    double* _runtime;

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
    double _max_timestamp;
};

#endif // JOB_H

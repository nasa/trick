#ifndef THREAD_H
#define THREAD_H

#include "job.h"
#include "frame.h"
#include "utils.h"
#include <stdexcept>

class Thread
{
  public:
    Thread() : thread_id(0), avg_runtime(0),
                   tidx_max_runtime(0),max_runtime(0), stdev(0),freq(0.0),
                   num_overruns(0) {}
    int thread_id;
    double avg_runtime;
    double avg_load;       // this is a percentage
    int tidx_max_runtime;
    double max_runtime ;
    double max_load ;      // this is a percentage
    double stdev;
    double freq;      // assume freq is freq of highest freq job on the thread
    QList<Job*> jobs; // sorted job list on thread with highest avg run times
    int num_overruns;

    void _do_stats(); // TODO: make private later

    double runtime(int tidx) const;
    double runtime(double timestamp) const;

    int nframes() const; // this differs from number of timestamps
                        //  since frames can span multiple timestamps

    double avg_job_runtime(Job* job) const ; // avg runtime per thread frame
                                        // differs from avg job runtime because
                                        // a job  may be called multiple times
                                        // per frame
                                        // e.g. a 0.1 job is called 10X if the
                                        // thread has a 1.0 second AMF frame
    double avg_job_load(Job* job) const ;

  private:
    QMap<int,double> _frameidx2runtime;
};

class Threads
{
  public:
    Threads(const QList<Job *> &jobs);
    ~Threads();
    Thread get(int id) const;
    QList<int> ids() const;
    QList<Thread> list() const ;
    int size() const { return _threads.keys().size(); }

  private:
    QList<Job*> _jobs;
    QList<int> _ids;
    QMap<int,Thread*> _threads;
};

#endif // THREAD_H

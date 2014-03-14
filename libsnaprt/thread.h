#ifndef THREAD_H
#define THREAD_H

#include "job.h"
#include "frame.h"
#include "utils.h"
#include "sjobexecthreadinfo.h"
#include <stdexcept>

#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QDataStream>
#include <QRegExp>

class Threads;


class Thread
{
  friend class Threads;

  public:
    Thread(const QString& runDir);
    ~Thread();

    void addJob(Job* job);  // all jobs added must have same threadId
    void setNumOverruns(int n) { num_overruns = n ; } // TODO: thread should calculate this, not snap so delete this and you will not have this long comment ya big dummy

    QList<Job*> jobs()               const { return _jobs; }
    Job*   jobAtIndex(int idx)       const { return _jobs.at(idx); }
    int    numJobs()                 const { return _jobs.size() ; }
    int    threadId()                const { return _threadId; }
    double avgRunTime()              const { return avg_runtime; }
    double runtime(int tidx)         const;
    double runtime(double timestamp)  const;
    double avgLoad()                 const { return avg_load; }
    int    timeIdxAtMaxRunTime()     const { return tidx_max_runtime; }
    double maxRunTime()              const { return max_runtime ; }
    double maxLoad()                 const { return max_load ; }
    double stdDeviation()            const { return stdev; }
    double frequency()               const { return freq; }
    int    numOverruns()             const { return num_overruns; }

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

    QString _runDir;
    int _threadId;
    SJobExecThreadInfo* _sJobExecThreadInfo;
    QList<Job*> _jobs;

    double avg_runtime;
    double avg_load;       // this is a percentage
    int tidx_max_runtime;
    double max_runtime ;
    double max_load ;      // this is a percentage
    double stdev;
    double freq;      // assume freq is freq of highest freq job on the thread
    int num_overruns;

    void _do_stats(); // TODO: make private later
    double _calcFrequency() ;

    static QString _err_string;
    static QTextStream _err_stream;


  private:
    QMap<int,double> _frameidx2runtime;
};

class Threads
{
  public:
    Threads(const QString& runDir, const QList<Job *> &jobs);
    ~Threads();
    Thread get(int id) const;
    QList<int> ids() const;
    QList<Thread> list() const ;
    int size() const { return _threads.keys().size(); }

  private:
    QString _runDir;
    QList<Job*> _jobs;
    QList<int> _ids;
    QMap<int,Thread*> _threads;
};

#endif // THREAD_H

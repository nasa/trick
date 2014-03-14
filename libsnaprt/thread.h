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
    void setNumOverruns(int n) { _num_overruns = n ; } // TODO: thread should calculate this, not snap so delete this and you will not have this long comment ya big dummy

    QList<Job*> jobs()               const { return _jobs; }
    Job*   jobAtIndex(int idx)       const { return _jobs.at(idx); }
    int    numJobs()                 const { return _jobs.size() ; }
    int    threadId()                const { return _threadId; }
    double avgRunTime()              const { return _avg_runtime; }
    double runtime(int tidx)         const;
    double runtime(double timestamp)  const;
    double avgLoad()                 const { return _avg_load; }
    int    timeIdxAtMaxRunTime()     const { return _tidx_max_runtime; }
    double maxRunTime()              const { return _max_runtime ; }
    double maxLoad()                 const { return _max_load ; }
    double stdDeviation()            const { return _stdev; }
    double frequency()               const { return _freq; }
    int    numOverruns()               const { return _num_overruns; }

    int numFrames() const; // this differs from number of timestamps
                        //  since frames can span multiple timestamps

    double avgJobRuntime(Job* job) const ; // avg runtime per thread frame
                                        // differs from avg job runtime because
                                        // a job  may be called multiple times
                                        // per frame
                                        // e.g. a 0.1 job is called 10X if the
                                        // thread has a 1.0 second AMF frame
    double avgJobLoad(Job* job) const ;

  private:

    QString _runDir;
    int _threadId;
    SJobExecThreadInfo _sJobExecThreadInfo;
    QList<Job*> _jobs;

    double _avg_runtime;
    double _avg_load;       // this is a percentage
    int _tidx_max_runtime;
    double _max_runtime ;
    double _max_load ;      // this is a percentage
    double _stdev;
    double _freq;      // assume freq is freq of highest freq job on the thread
    int _num_overruns;

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

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

#define TXT(X) X.toAscii().constData()

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

    int timeidx;
    double timestamp;
    double frame_time;
    double overrun_time;
    bool is_overrun;
    double hotjobpercentage;
    double spicyhotjobpercentage;

    // List of top jobs with most runtime for the frame
    QList<QPair<double,Job*> > topjobs;    // (rt,job)
};


class ThreadStat
{
  public:
    ThreadStat() : thread_id(0), avg_runtime(0),
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
    ThreadStat get(int id) const;
    QList<int> ids() const;
    QList<ThreadStat> list() const ;
    int size() const { return _threads.keys().size(); }

  private:
    QList<Job*> _jobs;
    QList<int> _ids;
    QMap<int,ThreadStat*> _threads;
};

class SimObject
{
  public:
    SimObject(const QString& name);
    QString name() const { return _name ; }
    double avg_runtime;
    QList<Job*> jobs; // sorted job list on simobject with highest avg run times

    void _do_stats(); // TODO: meant to be only used by SimObjects

  private:
    SimObject();
    QString _name;


};

class SimObjects
{
  public:
    SimObjects(const QList<Job *> &jobs);
    ~SimObjects();
    QList<SimObject> list() const; // sorted by highest avg runtime

  private:
    SimObjects();
    QList<Job*> _jobs;
    QMap<QString,SimObject*> _sim_objects;
};

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
    QString sim_object() const ;
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

    BoundedTrickBinaryRiver* river() const { return _river; }
    inline double* runtime() const { return _runtime; }
    inline double* timestamps() const { return _timestamps; }
    inline int npoints() const { return _npoints; }
private:
    Job() {}

    BoundedTrickBinaryRiver* _river;
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

class Snap
{
public:
    Snap(const QString& irundir, double istart, double istop);
    ~Snap();

    enum SortBy {
        NoSort,
        SortByJobAvgTime,
        SortByJobMaxTime
    };

    bool is_realtime() const { return _is_realtime ; }
    QString rundir() const { return _rundir ; }
    double start() const { return  _river_frame->getTimeStamps()[0]; }
    double stop() const  { return _river_frame->getTimeStamps()
                                 [_river_frame->getNumPoints()-1];}
    QList<Job *> *jobs(Snap::SortBy sort_method = SortByJobAvgTime) ;
    int num_jobs() const { return _jobs.size(); }
    int num_frames() const { return _frame_stats.size(); }
    int num_overruns() const { return FrameStat::num_overruns; }
    double percent_overruns() const { return 100.0*
                                   (double)num_overruns()/(double)num_frames(); }
    QString frame_rate() const ; // for now return list of frame rates
    double frame_avg() const { return _frame_avg; }
    double frame_stddev() const { return _frame_stddev; }

    int num_threads() const { return _threads->size(); }
    QString thread_listing() const ;

    QList<SimObject> simobjects() const { return _sim_objects->list(); }
    Threads* thread_stats() const { return _threads; }
    const QList<FrameStat>* frame_stats() const { return &_frame_stats; }

private:
    Snap() {}

    QString _rundir;
    double _start;
    double _stop;

    bool _is_realtime ;
    double _frame_avg;    void _calc_frame_avg();
    double _frame_stddev; void _calc_frame_stddev();

    SortBy _curr_sort_method;
    QList<Job*> _jobs;
    QString _job_logname(const Job* job) const;
    QMap<QString,Job*> _id_to_job;


    BoundedTrickBinaryRiver* _open_river(const QString& rundir,
                                          const QString& logfilename,
                                          double start, double stop);
    void _process_rivers();
    bool _parse_s_job_execution(const QString& rundir);
    bool _process_job_river( BoundedTrickBinaryRiver *river );
    QList<FrameStat> _process_frame_river(BoundedTrickBinaryRiver *river);

    BoundedTrickBinaryRiver* _river_userjobs;
    BoundedTrickBinaryRiver* _river_frame;
    BoundedTrickBinaryRiver* _river_trickjobs;

    QList<FrameStat>  _frame_stats;

    Threads* _threads;
    SimObjects* _sim_objects;
    QMap<int, ThreadStat> _thread_stats(
        const QMap<int,QMap<int,long> >&  threadtimes) const;
    QMap<int,QMap<int,long> >  _threadtimes() const; // tidx->(tid->thread_rt)

};

class SnapReport
{
  public:
    SnapReport(Snap& snap);
    QString report();

  private:
    Snap& _snap;

};

#endif // BLAME_H

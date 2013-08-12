#ifndef BLAME_H
#define BLAME_H

#include <QString>
#include <QList>
#include <QMap>
#include <QSet>
#include <QPair>
#include <QtAlgorithms>
#include <QDir>
#include <QTextStream>
#include <QBuffer>

#include "boundedtrickbinaryriver.h"
#include "job.h"
#include "thread.h"
#include "simobject.h"
#include "frame.h"
#include "utils.h"
#include "snaptable.h"

#define TXT(X) X.toAscii().constData()

class Snap
{
public:
    Snap(const QString& irundir, double istart=1.0, double istop=1.0e20);
    ~Snap();

    enum SortBy {
        NoSort,
        SortByJobAvgTime,
        SortByJobMaxTime
    };

    bool is_realtime() const { return _is_realtime ; }
    QString rundir() const { return _rundir ; }

    double start() const {
        if ( _river_frame->getNumPoints() > 0 ) {
            return  _river_frame->getTimeStamps()[0];
        } else {
            return _start;
        }
    }

    double stop() const  {
        if ( _river_frame->getNumPoints() > 0 ) {
            return _river_frame->getTimeStamps()
                                 [_river_frame->getNumPoints()-1];
        } else {
            return _stop;
        }
    }

    QList<Job *> *jobs(Snap::SortBy sort_method = SortByJobAvgTime) ;
    int num_jobs() const { return _jobs.size(); }
    int num_frames() const { return _frames.size(); }
    int num_overruns() const { return _num_overruns; }
    double percent_overruns() const { return 100.0*
                                   (double)num_overruns()/(double)num_frames(); }
    QString frame_rate() const ; // for now return list of frame rates
    double frame_avg() const { return _frame_avg; }
    double frame_stddev() const { return _frame_stddev; }

    int num_threads() const { return _threads->size(); }
    QString thread_listing() const ;

    QList<SimObject> simobjects() const { return _sim_objects->list(); }
    Threads* threads() const { return _threads; }
    const QList<Frame>* frames() const { return &_frames; }

    QList<SnapTable*> tables;

private:
    Snap() {}

    QString _err_string;
    QTextStream _err_stream;

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
    QList<Frame> _process_frame_river(BoundedTrickBinaryRiver *river);

    BoundedTrickBinaryRiver* _river_userjobs;
    BoundedTrickBinaryRiver* _river_frame;
    BoundedTrickBinaryRiver* _river_trickjobs;

    QList<Frame>  _frames;
    int _num_overruns;

    Threads* _threads;
    SimObjects* _sim_objects;

    SnapTable* _create_table_summary();
    SnapTable* _create_table_spikes();
    SnapTable* _create_table_thread_summary();
    SnapTable* _create_table_top_jobs();

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

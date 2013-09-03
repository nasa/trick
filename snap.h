#ifndef BLAME_H
#define BLAME_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QSet>
#include <QPair>
#include <QtAlgorithms>
#include <QDir>
#include <QTextStream>
#include <QBuffer>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QThread>

#include "boundedtrickbinaryriver.h"
#include "job.h"
#include "thread.h"
#include "simobject.h"
#include "frame.h"
#include "utils.h"
#include "snaptable.h"

#define TXT(X) X.toAscii().constData()

class Snap : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int progress
               READ getProgress
               WRITE setProgress
               NOTIFY progressChanged)

friend class LoadThread;

public:
    Snap(const QString& irundir, double istart=1.0, double istop=1.0e20,
         bool is_delay_load=false);
    ~Snap();

    enum SortBy {
        NoSort,
        SortByJobAvgTime,
        SortByJobMaxTime
    };

    void load();

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

    QList<SimObject> simobjects() const { return _simobjects->list(); }
    Threads* threads() const { return _threads; }
    const QList<Frame>* frames() const { return &_frames; }

    QList<SnapTable*> tables;
    SnapTable* jobTableAtTime(double time);

    int getProgress() const { return _progress; }
    void setProgress(int p) { _progress = p ;
                              emit progressChanged(p); }

signals:
    void progressChanged(int v);
    void finishedLoading();

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


    BoundedTrickBinaryRiver* _create_river(const QString& rundir,
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
    SimObjects* _simobjects;

    SnapTable* _table_summary;
    void _create_table_summary();
    void _set_data_table_summary();

    SnapTable* _table_spikes;
    void _create_table_spikes();
    void _set_data_table_spikes();

    SnapTable* _table_thread_summary;
    void _create_table_thread_summary();
    void _set_data_table_thread_summary();

    SnapTable* _table_top_jobs;
    void _create_table_top_jobs();
    void _set_data_table_top_jobs();

    SnapTable* _table_sim_objects;
    void _create_table_sim_objects();
    void _set_data_table_sim_objects();

    SnapTable* _table_thread_runtimes;
    void _create_table_thread_runtimes();
    void _set_data_table_thread_runtimes();

    int _progress;
    void _load();

};

class SnapReport
{
  public:
    SnapReport(Snap& snap);
    QString report();

  private:
    Snap& _snap;

};

class AnimationThread : public QThread
{
  public:
    AnimationThread(QPropertyAnimation* anim,
                      int duration, int start, int stop,
                      QObject* parent=0) :
        QThread(parent), _anim(anim),
        _duration(duration),
        _start(start),
        _stop(stop)
    {}

    void run()
    {
        QEasingCurve ez(QEasingCurve::InQuad);
        _anim->setEasingCurve(ez);
        _anim->setDuration(_duration);
        _anim->setStartValue(_start);
        _anim->setEndValue(_stop);
        _anim->start();
        exec();
    }

  private:
    QPropertyAnimation* _anim;
    int _duration;
    int _start;
    int _stop;
};

class LoadThread : public QThread
{
  public:
    LoadThread(Snap* snap,
              int expectedDuration,                // ms
              int startProgress, int stopProgress, // 0-100
              QObject* parent=0) :
        QThread(parent), _snap(snap),
        _duration(expectedDuration),
        _start(startProgress),
        _stop(stopProgress)
    {
    }

    void run()
    {
        QPropertyAnimation* anim = new QPropertyAnimation(_snap,"progress");
        AnimationThread* t = new AnimationThread(anim,_duration,_start,_stop);
        t->start();
        _snap->_load();
        anim->stop();
        t->quit();
        delete anim;
    }

  protected:
    Snap* _snap;
    int _duration;
    int _start;
    int _stop;
};

#endif // BLAME_H

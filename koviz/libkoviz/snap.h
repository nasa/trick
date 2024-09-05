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

#include "job.h"
#include "thread.h"
#include "simobject.h"
#include "frame.h"
#include "utils.h"
#include "snaptable.h"
#include "datamodel.h"
#include "curvemodel.h"

#define TXT(X) X.toLatin1().constData()

class Snap : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int progress
               READ getProgress
               WRITE setProgress
               NOTIFY progressChanged)

friend class LoadThread;

public:
    Snap(const QString& irundir,
         const QStringList& timeNames,
         bool is_delay_load=false);
    ~Snap();

    enum SortBy {
        NoSort,
        SortByJobAvgTime,
        SortByJobMaxTime
    };

    QString fileNameLogFrame() { return _fileNameLogFrame; }
    QString fileNameTrickJobs() { return _fileNameTrickJobs; }
    QStringList fileNamesUserJobs() { return _fileNamesUserJobs; }

    void load();

    bool is_realtime() const { return _is_realtime ; }

    QString rundir() const {
        QString dir(_rundir);
        if (dir.endsWith('/') ) {
            dir.chop(1);
        }
        return dir ;
    }

    QList<Job *> *jobs(Snap::SortBy sort_method = SortByJobAvgTime) ;
    int num_jobs() const { return _jobs.size(); }
    int num_frames() const { return _numFrames; }
    int num_overruns() const { return _num_overruns; }
    double percent_overruns() const { return 100.0*
                                  (double)num_overruns()/(double)num_frames(); }
    double frame_rate() const ; // for now return list of frame rates
    double frame_avg() const { return _frame_avg; }
    double frame_stddev() const { return _frame_stddev; }

    int num_threads() const { return _threads->hash()->size(); }
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

    QString _rundir;
    QStringList _timeNames;

    void _setLogFileNames();
    QString _fileNameLogFrame;
    QString _fileNameTrickJobs;
    QStringList _fileNamesUserJobs;

    bool _is_realtime ;

    SortBy _curr_sort_method;
    QList<Job*> _jobs;
    QList<CurveModel*> _curves;
    QString _job_logname(const Job* job) const;
    QMap<QString,Job*> _id_to_job;


    DataModel* _createModel(const QString& trk);
    void _process_models();
    bool _parse_s_job_execution(const QString& rundir);
    QList<Frame> _process_frames();
    bool _process_jobs(DataModel* model);

    DataModel* _trickJobModel;
    QList<DataModel*> _userJobModels;
    DataModel* _modelFrame;
    Thread* _thread0;  // main thread

    QList<Frame>  _frames;
    int _num_overruns;
    int _numFrames;
    double _frame_avg;    double _calc_frame_avg();
    double _frame_stddev; double _calc_frame_stddev(double frameAvg);

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

    int _progress;
    void _load();

    static QString _err_string;
    static QTextStream _err_stream;
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

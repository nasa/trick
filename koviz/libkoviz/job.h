#ifndef JOB_H
#define JOB_H

#include <QString>
#include <QTextStream>
#include <QFileInfo>
#include <QRegularExpression>
#include <stdlib.h>
#include <stdexcept>

#include "curvemodel.h"

class Job;

bool jobAvgTimeGreaterThan(Job* a,Job* b);
bool jobMaxTimeGreaterThan(Job* a,Job* b);

class Job
{
  public:
    // job_id is logged job name
    // e.g. JOB_bus.SimBus##read_ObcsRouter_C1.1828.00(read_simbus_0.100)
    Job(CurveModel* curve);

    bool isFrameTimerJob() { return _isFrameTimerJob; }

    QString job_id() const; // trick binary logged jobname
    QString job_num() const { return _job_num; }
    QString job_name() const { return _job_name; }
    QString job_class() const { return _job_class ;}
    QString sim_object_name() const ;
    int thread_id() const { return _thread_id; }
    double freq() ;

    double avg_runtime();
    double max_runtime();
    double max_timestamp();
    double stddev_runtime(); // TODO: make unit test

    inline CurveModel* curve() const { return _curve; }
    inline int npoints() const { return _npoints; }

private:
    Job() {}

    void _parseJobId(const QString& job_id, const QString& fileName);

    CurveModel* _curve;
    int _npoints;
    bool _isFrameTimerJob;

    QString _log_name;
    QString _job_name;
    QString _job_num;   // e.g. 1831.04
    int _thread_id;
    double _freq;
    QString _job_class;

    bool _is_stats;
    void _do_stats();
    double _avg_runtime;
    double _stddev_runtime;
    double _max_runtime;
    double _max_timestamp;

    static QString _err_string;
    static QTextStream _err_stream;

    int _threadId();
};

#endif // JOB_H

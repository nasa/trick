#include "simobject.h"
#include <QtCore/qmath.h>  // qSqrt
#include <limits.h> // Max/Min

bool simObjectAvgTimeGreaterThan(const SimObject& a, const SimObject& b)
{
    return a.avg_runtime() > b.avg_runtime();
}

SimObject::SimObject(const QString& name , const QList<Job *> &jobs) :
    _name(name), _jobs(jobs)
{
    _timeStamps = new QVector<double>;
    _runTimes = new QVector<double>;
    _do_stats();
}

void SimObject::_do_stats()
{
    if ( _jobs.size() == 0 ) {
        return;
    }

    qSort(_jobs.begin(),_jobs.end(),jobAvgTimeGreaterThan);

    long sum_squares = 0 ;
    long sum_rt = 0 ;
    long max_rt = 0 ;
    long min_rt = LONG_MAX;

    TrickCurveModel* curve0 = _jobs.at(0)->curve();
    const TrickModelIterator e0 = curve0->end();
    TrickModelIterator it0 = curve0->begin();
    while (it0 != e0) {
        double timeStamp = it0.t();
        _timeStamps->append(timeStamp);

        double rt = 0;
        foreach ( Job* job, _jobs ) {
            int tidx = job->curve()->indexAtTime(timeStamp);
            double jrt = job->curve()->begin()[tidx].x();
            rt += jrt;
        }
        _runTimes->append(rt/1000000.0);

        long irt = (long)rt;
        if ( irt > max_rt ) {
            max_rt = irt;
            _max_timestamp = timeStamp;
        }
        if ( irt < min_rt ) {
            min_rt = irt;
            _min_timestamp = timeStamp;
        }
        sum_squares += irt*irt;
        sum_rt += irt;

        ++it0;
    }

    double ss = (double)sum_squares;
    double s = (double)sum_rt;
    double n = (double)(_timeStamps->size());

    _min_runtime = (min_rt)/1000000.0;
    _max_runtime = (max_rt)/1000000.0;
    _avg_runtime = (s/n)/1000000.0;
    _stddev_runtime = qSqrt(ss/n - s*s/(n*n))/1000000.0 ;
}

SimObjects::SimObjects(const QList<Job *> &jobs)
{
    QMap<QString,QList<Job*> > _sim_object_jobs;

    foreach ( Job* job, jobs ) {
        QString simobject_name = job->sim_object_name();
        _sim_object_jobs[job->sim_object_name()].append(job);
    }

    foreach ( QString sim_object_name, _sim_object_jobs.keys() ) {
        SimObject* sobject = new SimObject(sim_object_name,
                                        _sim_object_jobs.value(sim_object_name));
        _sim_objects.append(sobject);
    }
}

SimObjects::~SimObjects()
{
    foreach ( SimObject* sobject, _sim_objects ) {
        delete sobject;
    }
}

QList<SimObject> SimObjects::list() const
{
    QList<SimObject> list ;

    foreach ( SimObject* sobject, _sim_objects ) {
        list.append(*sobject);
    }

    qSort(list.begin(),list.end(), simObjectAvgTimeGreaterThan);

    return list;
}


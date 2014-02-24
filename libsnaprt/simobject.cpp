#include "simobject.h"

bool simObjectAvgTimeGreaterThan(const SimObject& a, const SimObject& b)
{
    return a.avg_runtime() > b.avg_runtime();
}

SimObject::SimObject(const QString& name , const QList<Job *> &jobs) :
    _name(name), _jobs(jobs)
{
    _do_stats();
}

void SimObject::_do_stats()
{
    if ( _jobs.size() == 0 ) {
        return;
    }

    qSort(_jobs.begin(),_jobs.end(),jobAvgTimeGreaterThan);

    int npoints = _jobs.at(0)->npoints();
    double sum_time = 0.0;
    for ( int tidx = 0; tidx < npoints; ++tidx) {
        foreach ( Job* job, _jobs ) {
            sum_time += job->runtime()[tidx];
        }
    }

    _avg_runtime = sum_time/(double)npoints/1000000.0;
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


#include "simobject.h"
#include <QtCore/qmath.h>  // qSqrt
#include <QVector>
#include <float.h> // Max/Min

bool simObjectAvgTimeGreaterThan(const SimObject& a, const SimObject& b)
{
    return a.avg_runtime() > b.avg_runtime();
}

SimObject::SimObject(const QString& name , const QList<Job *> &jobs,
                     double frameRate) :
    _name(name), _jobs(jobs), _frameRate(frameRate)
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

    std::sort(_jobs.begin(),_jobs.end(),jobAvgTimeGreaterThan);

    double sum_squares = 0 ;
    double sum_rt = 0 ;
    double max_rt = 0 ;
    double min_rt = DBL_MAX;

    // Get begin/end timestamps
    double begTimeStamp = 0.0;
    double endTimeStamp = 0;
    CurveModel* curve0 = _jobs.at(0)->curve();
    ModelIterator* it0 = curve0->begin();
    bool isFirst = true;
    while ( !it0->isDone() ) {
        if ( isFirst ) {
            begTimeStamp = it0->t();
            isFirst = false;
        }
        endTimeStamp = it0->t();
        it0->next();
    }
    delete it0;

    // Determine cycle (freq) of SimObject
    // Default cycleTime to frameRate (passed in)
    // If all jobs have higher cycleTimes than
    // frameRate passed in, set cycleTime to
    // freq of job with minimum freq
    _cycleTime = _frameRate;
    double minCycle = 1.0e20;
    foreach ( Job* job, _jobs ) {
        if ( job->freq() < minCycle ) {
            minCycle = job->freq();
        }
    }
    if ( minCycle > _frameRate ) {
        _cycleTime = minCycle;
    }

    // The reason for the complexity
    // is accounting for jobs on the SimObject
    // with different frequencies.
    // SimObjects really have no frequency (cycle time).
    // This algorithm sums up job run times for a
    // frame rate guess.
    QHash<Job*,int> currentJobTimeIdx;
    foreach ( Job* job, _jobs ) {
        currentJobTimeIdx.insert(job,0);
    }
    double timeStamp = begTimeStamp;
    long cnt = 1;
    while ( timeStamp < endTimeStamp ) {

        _timeStamps->append(timeStamp);

        double nextTime = timeStamp+_cycleTime;
        double rt = 0;
        foreach ( Job* job, _jobs ) {
            int tidx = currentJobTimeIdx.value(job);
            ModelIterator* it = job->curve()->begin()->at(tidx);
            while ( !it->isDone() ) {
                double t = it->t();
                if ( t < nextTime-1.0e-9 ) {
                    rt += it->x();
                } else {
                    break;
                }
                ++tidx;
                it->next();
            }
            delete it;
            currentJobTimeIdx.insert(job,tidx);
        }

        _runTimes->append(rt);

        if ( rt > max_rt ) {
            max_rt = rt;
            _max_timestamp = timeStamp;
        }
        if ( rt < min_rt ) {
            min_rt = rt;
            _min_timestamp = timeStamp;
        }
        sum_squares += rt*rt;
        sum_rt += rt;

        timeStamp = begTimeStamp + cnt*_cycleTime;
        ++cnt;
    }

    double ss = (double)sum_squares;
    double s = (double)sum_rt;
    double n = (double)(_timeStamps->size());

    _min_runtime = min_rt;
    _max_runtime = max_rt;
    _avg_runtime = s/n;
    _stddev_runtime = qSqrt(ss/n - s*s/(n*n));
}

SimObjects::SimObjects(const QList<Job *> &jobs, double frameRate)
{
    QMap<QString,QList<Job*> > _sim_object_jobs;

    foreach ( Job* job, jobs ) {
        _sim_object_jobs[job->sim_object_name()].append(job);
    }

    foreach ( QString sim_object_name, _sim_object_jobs.keys() ) {
        SimObject* sobject = new SimObject(sim_object_name,
                                        _sim_object_jobs.value(sim_object_name),
                                        frameRate);
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

    std::sort(list.begin(),list.end(), simObjectAvgTimeGreaterThan);

    return list;
}


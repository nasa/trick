#ifndef SIMOBJECT_H
#define SIMOBJECT_H

#include "job.h"

class SimObject
{
  public:
    SimObject(const QString& name, const QList<Job*>& jobs, double frameRate);
    QString name() const { return _name ; }
    double avg_runtime() const { return _avg_runtime; }
    double min_runtime() const { return _min_runtime; }
    double max_runtime() const { return _max_runtime; }
    double stddev_runtime() const { return _stddev_runtime; }
    QList<Job*> jobs() const { return _jobs; }

  private:
    SimObject();
    QString _name;
    QList<Job*> _jobs; // sorted job list on simobject w/ highest avg run times
    double _frameRate;
    double _cycleTime;
    QVector<double> *_timeStamps;
    QVector<double> *_runTimes;
    double _avg_runtime;
    double _min_runtime;
    double _min_timestamp;
    double _max_runtime;
    double _max_timestamp;
    double _stddev_runtime;
    void _do_stats();
};

// TODO: Can't I just inherit off of QList?
class SimObjects
{
  public:
    SimObjects(const QList<Job *> &jobs, double frameRate);
    ~SimObjects();
    QList<SimObject> list() const; // sorted by highest avg runtime

  private:
    SimObjects();
    QList<SimObject*> _sim_objects;
};

#endif // SIMOBJECT_H

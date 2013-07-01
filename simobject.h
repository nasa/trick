#ifndef SIMOBJECT_H
#define SIMOBJECT_H

#include "job.h"

class SimObject
{
  public:
    SimObject(const QString& name, const QList<Job*>& jobs);
    QString name() const { return _name ; }
    double avg_runtime() const { return _avg_runtime; }
    QList<Job*> jobs() const { return _jobs; }

  private:
    SimObject();
    QString _name;
    double _avg_runtime;
    QList<Job*> _jobs; // sorted job list on simobject w/ highest avg run times
    void _do_stats();
};

class SimObjects
{
  public:
    SimObjects(const QList<Job *> &jobs);
    ~SimObjects();
    QList<SimObject> list() const; // sorted by highest avg runtime

  private:
    SimObjects();
    QList<SimObject*> _sim_objects;
};

#endif // SIMOBJECT_H

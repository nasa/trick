#ifndef FRAME_H
#define FRAME_H

#include <QList>
#include <QPair>
#include <QHash>
#include "job.h"

class Frame;

bool frameTimeGreaterThan(const Frame& a,const Frame& b);

class Frame
{
  public:
    Frame(QList<Job*>* jobs, int timeidx,
          double timestamp, double frame_time);

    static QString frame_sched_time;
    static QString frame_overrun_time;

    int timeidx() const { return _tidx; }
    double timestamp() const { return _timestamp; }
    double frame_time() const { return _frame_time; }
    double jobloadindex();   // Percentage of jobs in sim running
                             // a factor of std devs above normal

    // List of top jobs with most runtime for the frame
    QList<QPair<double, Job *> > *topjobs();    // (rt,job)

  private:

    Frame() ;

    QList<Job*>* _jobs;
    int _tidx;
    double _timestamp;
    double _frame_time;
    double _jobloadindex;   // Percentage of jobs in sim running
                           // a factor of std devs above normal

    // List of top jobs with most runtime for the frame
    QList<QPair<double,Job*> > _topjobs;    // (rt,job)
    void _calc_topjobs();
};


#endif // FRAME_H

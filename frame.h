#ifndef FRAME_H
#define FRAME_H

#include <QPair>
#include "job.h"

class Frame;

bool frameTimeGreaterThan(const Frame& a,const Frame& b);

class Frame
{
  public:
    Frame() : frame_time(0), overrun_time(0), is_overrun(false)
    {}

    static QString frame_time_name;
    static QString overrun_time_name;

    int timeidx;
    double timestamp;
    double frame_time;
    double overrun_time;
    bool is_overrun;
    double jobloadindex;   // Percentage of jobs in sim running
                          // a factor of std devs above normal

    // List of top jobs with most runtime for the frame
    QList<QPair<double,Job*> > topjobs;    // (rt,job)
};


#endif // FRAME_H

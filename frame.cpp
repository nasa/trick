#include "frame.h"

QString Frame::frame_time_name = "real_time.rt_sync.frame_sched_time";
QString Frame::overrun_time_name = "real_time.rt_sync.frame_overrun_time";

bool frameTimeGreaterThan(const Frame& a,const Frame& b)
{
    return a.frame_time > b.frame_time;
}

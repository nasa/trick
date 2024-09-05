#ifndef SNAPFRAMELOG_H
#define SNAPFRAMELOG_H

/*******************************************************************************
PURPOSE:
    (Real-time Frame logging without SimBus jobs or Timeline)
REFERENCES:
    (See Trick FrameLog class)
LANGUAGE:
    (C++)
ASSUMPTIONS AND LIMITATIONS:
    ((None))
LIBRARY DEPENDENCY:
    ((snapframelog.cpp))
PROGRAMMERS:
    (((Keith)))
*******************************************************************************/

#include "sim_services/FrameLog/include/FrameLog.hh"

class SnapFrameLog {

    TRICK_MM_INTERFACE(SnapFrameLog, SnapFrameLog)

  public:

    SnapFrameLog();
    ~SnapFrameLog();

    void addJob(std::string jobName, unsigned int instance = 1);

    int log_start(Trick::JobData * curr_job);
    int log_stop(Trick::JobData * curr_job);
    int log_on();
    int log_off();
    int log_shutdown();

  private:

    bool frame_log_flag;                      /**< trick_io(*io) trick_units(--) */

    std::vector < Trick::JobData *> userSpecJobs;
    std::vector < Trick::FrameDataRecordGroup * >drg_users; /**< trick_io(**) */
    Trick::FrameDataRecordGroup * drg_trick;  /**<  trick_io(*io) trick_units(--) */
    Trick::FrameDataRecordGroup * drg_frame;  /**<  trick_io(*io) trick_units(--) */

    int num_threads;                          /**<  trick_io(**) */

    bool log_init_start;                      /**<  trick_io(**) */
    bool log_init_end;                        /**<  trick_io(**) */
    ATTRIBUTES time_value_attr;               /**<  trick_io(**) */
    std::string rt_sim_object_name;           /**<  trick_io(**) */
};

#endif                          // SNAPFRAMELOG_H

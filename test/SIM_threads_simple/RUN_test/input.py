import os

def main():
    trick.real_time_enable()
    #trick.itimer_enable()
    trick.exec_set_software_frame(0.10)
    trick.stop(4)

    trick.exec_set_thread_process_type(1, trick.PROCESS_TYPE_AMF_CHILD)
    trick.exec_set_thread_amf_cycle_time(1, 0.5)
    #thr_con = trick.exec_get_thread(1).trigger_container
    #thr_con.setThreadTrigger(trick.TT_MUTEX)

    trick.exec_set_thread_process_type(2, trick.PROCESS_TYPE_AMF_CHILD)
    trick.exec_set_thread_amf_cycle_time(2, 0.1)

    trick.echo_jobs_on()
    trick.frame_log_on()

    trick.add_read(2.0,"trick.echo_jobs_off()")

if __name__ == "__main__":
    main()


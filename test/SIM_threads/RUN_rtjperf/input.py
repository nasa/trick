"""Live RTJPerf exercise: localhost:9105, 25 ms frames, no external GUIs.

Each 80-second cycle is healthy (20 s), near budget (20 s), overload
(20 s), recovery (20 s). Four cycles give time to attach the GUI.
Load durations are wall-clock microseconds in the existing Thread1 jobs.
Manual controls over VariableServer: trick.exec_freeze(), trick.exec_run(),
trick.exec_terminate(). Freeze does not advance the stage schedule.
"""

trick.exec_set_software_frame(0.025)
trick.exec_set_freeze_frame(0.025)
trick.exec_set_terminate_time(320.0)
trick.var_server_set_source_address("127.0.0.1")
trick.var_server_set_port(9105)
trick.var_server_set_enabled(True)
trick.var_set_allow_connections(True)
trick.real_time_enable()
trick.itimer_enable()
trick.frame_log_on()

trick.exec_set_thread_process_type(1, trick.PROCESS_TYPE_AMF_CHILD)
trick.exec_set_thread_amf_cycle_time(1, 0.05)
trick.exec_set_thread_process_type(2, trick.PROCESS_TYPE_AMF_CHILD)
trick.exec_set_thread_amf_cycle_time(2, 0.1)

# Child threads retain a small identifiable workload; main-thread load is
# deliberately above the 25 ms frame budget only in the overload stage.
deadlock_test.thread_25ms.m_bBusy = True
deadlock_test.thread_50ms.m_bBusy = True
deadlock_test.thread_100ms.m_bBusy = True
deadlock_test.thread_25ms.m_iBusyUSecs = 3000
deadlock_test.thread_50ms.m_iBusyUSecs = 5000
deadlock_test.thread_100ms.m_iBusyUSecs = 8000

for cycle in range(4):
    for offset, label, usec in (
        (0, "healthy", 3000),
        (20, "near-budget", 22000),
        (40, "overload", 35000),
        (60, "recovery", 3000),
    ):
        at = cycle * 80 + offset
        trick.add_read(at, (
            "deadlock_test.thread_25ms.m_iBusyUSecs = %d\n"
            "print('RTJPERF stage=%s sim_time=%d load_us=%d', flush=True)"
        ) % (usec, label, at, usec))

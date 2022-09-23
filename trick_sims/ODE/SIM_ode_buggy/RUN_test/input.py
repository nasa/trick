
import trick

def main():
    buggy_so.buggy.chassis_length = 1.2
    buggy_so.buggy.chassis_width = 1.0

    buggy_so.buggy.wheel_width = 0.2

    trick.sim_control_panel_set_enabled(True)
    trick.real_time_enable()
    trick.itimer_enable()

    trick.exec_set_software_frame(0.05)
    trick.exec_set_freeze_frame(0.05)
    trick.exec_set_freeze_command(True)

    trick.frame_log_on()

    trick.exec_set_thread_process_type(1, trick.PROCESS_TYPE_ASYNC_CHILD)
    trick.exec_set_thread_amf_cycle_time(1, 0.1)

if __name__ == "__main__":
    main()



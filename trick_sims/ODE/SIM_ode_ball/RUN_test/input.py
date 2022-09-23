
import trick

def main():
    trick.sim_control_panel_set_enabled(True)
    trick.real_time_enable()
    trick.itimer_enable()

    trick.exec_set_software_frame(0.05)
    trick.exec_set_freeze_frame(0.05)
    trick.exec_set_freeze_command(True)

    trick.frame_log_set_max_samples(100)
    trick.frame_log_on()

    trick.exec_set_thread_process_type(1, trick.PROCESS_TYPE_ASYNC_CHILD)
    trick.exec_set_thread_amf_cycle_time(1, 0.1)

if __name__ == "__main__":
    main()



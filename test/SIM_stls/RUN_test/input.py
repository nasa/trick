
def main():

    #trick.echo_jobs_on()

    #trick.sim_control_panel_set_enabled(True)
    #trick.real_time_enable()
    #trick.itimer_enable()

    trick.checkpoint_pre_init(True)
    #trick.checkpoint_post_init(True)
    #trick.checkpoint_end(True)

    #trick.freeze(2.0)

    trick.exec_set_software_frame(0.10)
    trick.exec_set_freeze_frame(0.10)
    trick.stop(5.0)

if __name__ == "__main__":
    main()


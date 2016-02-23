
def main():

    #trick.echo_jobs_on()
    simControlPanel = trick.SimControlPanel()
    simControlPanel.thisown = 0
    trick.add_external_application(simControlPanel)

    trick.real_time_enable()
    #trick.itimer_enable()

    trick.checkpoint_post_init(True)
    trick.checkpoint_end(True)

    trick.freeze(2.0)

    the_object.stlc2.name = "Daisy"

    trick.exec_set_freeze_frame(0.10)
    trick.stop(5.0)

if __name__ == "__main__":
    main()



import trick

def main():
    simControlPanel = trick.SimControlPanel()
    simControlPanel.thisown = 0
    #trick.add_external_application(simControlPanel)

    #ball.my_integ = trick.getIntegrator( trick.Runge_Kutta_2, 4, 1.0);
    my_integ_loop.getIntegrator( trick.Runge_Kutta_2, 4 )

    trick_message.mpublisher.sim_name = "ball_sim"
    trick.stop(300.0)
    trick.echo_jobs_on()

    read = 2.4
    trick.add_read(read , "trick.echo_jobs_off()")

    read = 2.6
    trick.add_read(read , "trick.debug_pause_on()")



if __name__ == "__main__":
    main()

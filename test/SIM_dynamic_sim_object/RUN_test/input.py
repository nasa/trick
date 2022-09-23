
def main():
    ball = trick.ballSimObject(TMMName = "ball") ;
    trick.exec_add_sim_object(ball, "ball")
    # instead of using collect, we are using a class specific add_external_force
    # function that accepts a "double *" and is checkpointable.
    #ball.obj.state.add_external_force( ball.obj.force.output.force )

    # declare a new Integrator loop sim object
    my_integ_loop = trick.IntegLoopSimObject(0.01, 0, ball, None, TMMName="my_integ_loop") ;
    trick.exec_add_sim_object(my_integ_loop, "my_integ_loop")
    # create a new Runge Kutta 2 integrator with 4 states
    my_integ_loop.getIntegrator(trick.Runge_Kutta_2, 4)
    my_integ_loop.integ_sched.rebuild_jobs()

    # Data recording Ascii
    #drg0 = trick.DRAscii("Ball")
    #for param in [ 'position' , 'velocity' , 'acceleration' , 'external_force' ] :
    #    for index in range(0,2) :
    #        var = "ball.obj.state.output." + param + "[" + str(index) + "]"
    #        drg0.add_variable(var)
    #drg0.set_cycle(0.01)
    #drg0.freq = trick.DR_Always
    #drg0.thisown = 0
    #trick.add_data_record_group(drg0, trick.DR_Buffer)

    trick.checkpoint(120.0)
    trick.stop(300.0)


if __name__ == "__main__":
    main()



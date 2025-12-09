
import trick

def main():

    # Data recording HDF5 test
    drg0 = trick.DRAscii("Ball")
    for param in [ 'position' , 'velocity' , 'acceleration' , 'external_force' ] :
        for index in range(0,2) :
            var = "ball.obj.state.output." + param + "[" + str(index) + "]"
            drg0.add_variable(var)
    drg0.set_cycle(0.01)
    drg0.freq = trick.DR_Always
    drg0.thisown = 0
    trick.add_data_record_group(drg0, trick.DR_Buffer)

    my_integ_loop.getIntegrator( trick.Runge_Kutta_2, 4 );

    #print trick.ref_attributes("ball.obj.state.output.velocity").attr.units

    #message.mpublisher.sim_name = "ball_sim"

    trick.stop(300.0)

    #trick.exec_set_time(-10.0)
    trick.checkpoint_pre_init(True)
    #trick.checkpoint_post_init(True)
    #trick.checkpoint_end(True)

    #read = 99.999999
    #trick.add_read(read,"""trick.checkpoint()""") ;

    #trick.checkpoint(120.0)

    #trick.exec_set_freeze_frame(0.10)

    #read = 9.900001
    #trick.add_read(read,"""print "hello" , trick.exec_get_sim_time()""")
    #read = 9.900002
    #trick.add_read(read,"""print "hello" , trick.exec_get_sim_time()""")

    #trick.add_read(read,"""trick.stop()""")
    #trick.add_read(read , """trick.set_job_onoff("ball.obj.state_print", False)""")
    #trick.add_read(read , """trick.set_job_onoff("ball.obj.state_print", False)""")

    #print trick.exec_get_job_cycle(None)
    #print trick.exec_get_job_cycle("ball.obj.state_print")

    #trick_sie.class_attr_map.print_xml()
    #trick_sie.enum_attr_map.print_xml()
    #trick.exec_set_trap_sigfpe(True)


if __name__ == "__main__":
    main()



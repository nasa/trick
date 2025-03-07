
import trick

def main():

    # Data recording test
    drg0 = trick.DRAscii("Ball")
    for param in [ 'position' , 'velocity' , 'acceleration' , 'external_force' ] :
        for index in range(0,2) :
            var = "ball.obj.state.output." + param + "[" + str(index) + "]"
            drg0.add_variable(var)
    drg0.set_cycle(0.01)
    drg0.freq = trick.DR_Always
    drg0.thisown = 0
    trick.add_data_record_group(drg0, trick.DR_Buffer)

    my_integ_loop.getIntegrator(trick.Runge_Kutta_2, 4);

    trick_message.mpublisher.sim_name = "ball_sim"

    trick.checkpoint_pre_init(False)
    trick.checkpoint_post_init(False)
    trick.checkpoint_end(False)

    trick.exec_set_freeze_frame(0.10)
    trick.stop(300.0)

    trick_utest.unit_tests.enable() ;
    trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_Ball++_L1.xml" ) ;

if __name__ == "__main__":
    main()



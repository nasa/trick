
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
    my_integ_loop.add_rate(0.005)
    trick.exec_set_job_onoff("SINGLE", 1, False);
    trick.exec_set_job_onoff("MULTI", 1, True);
    my_integ_loop.integ_sched.verbosity = True
    trick.add_read(0.015, '''
my_integ_loop.set_integ_cycle(0.02)
trick.message_publish(1, 'Called my_integ_loop.set_integ_cycle(0.02)\\n')
''')
    trick.add_read(0.055, '''
my_integ_loop.integ_sched.set_integ_rate(1, 0.01)
trick.message_publish(1, 'Called my_integ_loop.integ_sched.set_integ_rate(1, 0.01)\\n')
''')
    trick.add_read(1.0, '''
my_integ_loop.integ_sched.set_integ_rate(1, 0.005)
trick.message_publish(1, 'Called my_integ_loop.integ_sched.set_integ_rate(1, 0.005)\\n')
trick.message_publish(1, f'Total num rates = {my_integ_loop.integ_sched.get_num_rates()}\\n')
trick.message_publish(1, f'Rate 0 = {my_integ_loop.integ_sched.get_rate(0)}\\n')
trick.message_publish(1, f'Rate 1 = {my_integ_loop.integ_sched.get_rate(1)}\\n')
trick.message_publish(1, f'Rate 2 = {my_integ_loop.integ_sched.get_rate(2)}\\n')
''')
    trick.add_read(1.2, '''
my_integ_loop.integ_sched.verbosity = False
''')

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



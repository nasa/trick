
import trick
import top
import time

def main():

    # Data recording HDF5 test
    drg0 = trick.DRAscii("Ball")
    for param in [ 'position' ] :
        for index in range(0,2) :
            var = "ball.obj.state.output." + param + "[" + str(index) + "]"
            drg0.add_variable(var)
    drg0.set_cycle(0.01)
    drg0.freq = trick.DR_Always
    drg0.thisown = 0
    trick.add_data_record_group(drg0, trick.DR_Buffer)

    trick.sim_control_panel_set_enabled(True)

    trick.real_time_enable()
    trick.itimer_enable()

    #trick.TMM_hexfloat_checkpoint(True)
    #trick.exec_set_rt_nap(False)

    #trick_real_time.gtod_clock.set_clock_ID(trick.TRICK_CLOCK_MONOTONIC)
    #trick.real_time_change_clock(trick_tpro.tpro_cte)
    #trick.real_time_change_clock(trick_bc635.bc635)
    trick.message_publish(trick.MSG_INFO, "Current clock is = " + trick.real_time_clock_get_name() + "\n")
    trick_real_time.rt_sync.align_sim_to_wall_clock = True
    #real_time.rt_sync.align_tic_mult = 60.0

    #trick.echo_jobs_on() ;
    #trick.debug_pause_on() ;

    if hasattr(top.cvar, 'trick_jsonvs'):
        trick_jsonvs.vs.set_port(50000)
        trick.message_publish(trick.MSG_INFO, "JSON listen port = " + str(trick_jsonvs.vs.get_port()) + "\n")

    # set the software frame and freeze_frame
    trick.exec_set_software_frame(0.1)
    trick.exec_set_freeze_frame(0.1)
    #trick.exec_set_freeze_command(True)

    #trick.exec_set_enable_freeze(True)
    #trick.exec_set_stack_trace(False)
    #trick.freeze()
    #trick.freeze(5.0)

    #trick.add_read(1.0,"""trick.checkpoint("chkpnt_1.0")""")

    my_integ_loop.getIntegrator(trick.Runge_Kutta_2, 4)

    trick.exec_set_terminate_time(3000.0)

    # Turn on frame logging
    #real_time.frame_log.tl_max_samples = 10000;
    trick.frame_log_on()

    #read = 28.0
    #trick.add_read(read , """trick.real_time_disable()""") ;

    #read = 58.0
    #trick.add_read(read , """trick.real_time_enable()""") ;

    #trick.exec_set_lock_memory(True)

    #trick.exec_set_thread_priority(0,1)
    #trick.exec_set_thread_cpu_affinity(0,6)

    #trick.exec_set_thread_priority(1,2)
    #trick.exec_set_thread_cpu_affinity(1,5)

    #trick.exec_set_thread_priority(2,3)
    #trick.exec_set_thread_cpu_affinity(2,4)

    #read = 5.0
    #trick.add_read(read , """trick.debug_pause_on()""")

    #trick.exec_set_thread_process_type(2 , sim.PROCESS_TYPE_SCHEDULED) ;
    #trick.exec_set_thread_process_type(2 , sim.PROCESS_TYPE_AMF_CHILD) ;
    #trick.exec_set_thread_process_type(2 , sim.PROCESS_TYPE_ASYNC_CHILD) ;
    #trick.exec_set_thread_async_wait(2 , 1) ;

    #read = 5.0
    #trick.add_read(read , "print 'read at 5'")
    #read = 8.0
    #trick.add_read(read , "print 'read at 8'")

    #my_event = trick.new_event("my_event")
    #my_event.cycle = 1.0
    #my_event.condition(0, "trick.exec_get_sim_time() > 0.0")
    #my_event.action(0 , """print "hello world" , trick.exec_get_sim_time() , trick.activate_event("my_event")""")
    #my_event.activate()
    #trick.add_event(my_event)

    #trick.exec_set_thread_cpu_affinity(0,5)
    #trick.exec_set_thread_cpu_affinity(0,6)
    #trick.data_record_cpu_set(7)
    #trick.data_record_cpu_set(8)
    #trick.var_server_cpu_set(9)
    #trick.var_server_cpu_set(10)
    #trick.message_cpu_set(11)
    #trick.message_cpu_set(12)
    #trick.add_read(1.0, "data_record.drd.drd_writer_thread.dump()")
    #trick.add_read(1.0, "trick.exec_get_thread(0).dump()")
    #trick.add_read(1.0, "trick_vs.vs.get_listen_thread().dump()")
    #trick.add_read(1.0, "message.mdevice.get_listen_thread().dump()")

if __name__ == "__main__":
    main()



trick.exec_set_terminate_time(300.0)

read = 5.0
trick.add_read(read , "print 'read at 5'")

read = 15.0
trick.add_read(read , "print 'read at 15'")

my_event = trick.new_event("my_event")
my_event.set_cycle(10.0)
my_event.condition(0, "ball.obj.state.output.position[0] > 0.0")

my_event.action(0 , """
print "hello world" , ball.obj.state.output.position[0] , trick.exec_get_sim_time()
my_event.activate()
""")

my_event.activate()
#my_event.set_thread(1)
trick.add_event(my_event)

#trick.exec_set_thread_process_type(1 , trick.PROCESS_TYPE_AMF_CHILD)
#trick.exec_set_thread_amf_cycle_time(1, 10.0)

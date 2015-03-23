
execfile("RUN_test/input.py")

print '\nFinding number of CPUs...'
os.system('cat /proc/cpuinfo | grep processor')
os.system('cat /proc/cpuinfo | grep processor | wc -l')

'''
Automate the overrun condition(s)
'''
trick.real_time_enable()
trick.itimer_enable()

read = 2.0
trick.add_read(read, """deadlock_test.thread_50ms.m_bBusy = True""" )
read = 4.0
trick.add_read(read, """deadlock_test.thread_100ms.m_bBusy = True""" )
read = 5.0
trick.add_read(read, """deadlock_test.thread_100ms.m_iBusyUSecs = 97000""" )
read = 7.0
trick.add_read(read, """deadlock_test.thread_50ms.m_iBusyUSecs = 49000""" )

""" gradually increase the delay to better obtain an overrun """
event_1 = trick.new_event("slowDownThread3")
event_1.set_cycle(2.0)
event_1.condition(0, """trick.exec_get_mode() == trick.Run and \
                        trick.exec_get_sim_time() > 10.0""")
event_1.action(0,"""
deadlock_test.thread_100ms.m_iBusyUSecs += 1000
event_1.activate()
""")
event_2 = trick.new_event("slowDownThread2")
event_2.set_cycle(5.0)
event_2.condition(0, """trick.exec_get_mode() == trick.Run and \
                        trick.exec_get_sim_time() > 10.0""")
event_2.action(0,"""
deadlock_test.thread_50ms.m_iBusyUSecs += 1000
event_2.activate()
""")

trick.add_event(event_1)
trick.add_event(event_2)
event_1.activate()
event_2.activate()

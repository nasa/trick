
##########################################################
# Trick setup:
# enable real-time
trick.real_time_enable()
trick.exec_set_software_frame(0.1)
# turn on frame logging
#trick.frame_log_on()

# turn on sim control panel
#trick.sim_control_panel_set_enabled(True)
# start sim in freeze?
#trick.exec_set_freeze_command(True)
# allow ctrl-C freezing
trick.exec_set_enable_freeze(True)

stop_time = 5
trick.stop(stop_time)

##########################################################
# Trick unit test setup:
from trick.unit_test import *
trick_utest.unit_tests.enable()
trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_events.xml" )
trick_utest.unit_tests.set_test_name( "EventsTest" )
from array import array
expected= array('f')
result = array('f')
##########################################################
test_suite = "events cyclic"

# TEST 0: event created in S_define, fire only once
trick.add_event(ev.event0)
expected.append(1)
result.append(0)

# TEST 1: cyclic event, fire every second
event1 = trick.new_event("event1")
event1.condition(0, "ev.cond_var_true==True")
event1.action(0, "print (\"event1\"); result[1] += 1");
event1.action(1, "event1.activate()")
event1.set_cycle(1.0)
event1.activate() 
trick.add_event(event1)
expected.append(1+stop_time)
result.append(0)

# TEST 2: cyclic event, condition variable, added to 0.5 sched job so fire twice a second
event2 = trick.new_event("event2")
event2.condition_var(0, "ev.cond_var_true")
event2.action(0, "print (\"event2\"); result[2] += 1");
event2.action(1, "event2.activate()")
event2.activate() 
trick.add_event_after(event2, "ev.sched")
expected.append(1+(stop_time*2))
result.append(0)

# TEST 3: cyclic event, condition variable, change condition and change action
event3 = trick.new_event("event3")
event3.condition_var(0, "ev.cond_var_true")
event3.action(0, "print (\"event3\"); result[3] += 1");
event3.action(1, "event3.activate()")
event3.set_cycle(1.0)
event3.activate() 
trick.add_event(event3)
#condition will be false during sim time 2.0 and 3.0
trick.add_read(2.0, """event3.condition(0, "False")""")
trick.add_read(2.0, """event3.action(0, \"print (\\\"event3_changed\\\"); result[3] += 5\")""")
trick.add_read(4.0, """event3.condition_var(0, "ev.cond_var_true")""")
expected.append(2+((stop_time-3)*5))
result.append(0)

# TEST 4: cyclic event, condition variable, delete event and add again with new cycle time
event4 = trick.new_event("event4")
event4.condition_var(0, "ev.cond_var_true")
event4.action(0, "print (\"event4\"); result[4] += 1");
event4.action(1, "event4.activate()")
event4.set_cycle(1.0)
event4.activate() 
trick.add_event(event4)
# deleting of event will occur in current frame 2.0
event4_deleted1 = False
trick.add_read(2.0, """
trick.delete_event(event4)
event4_deleted1 = (trick.get_event("event4") == None)
""")
# adding of event will occur in next frame after 4.0
event4_deleted2 = False
trick.add_read(4.0, """
event4.set_cycle(0.1)
trick.add_event(event4)
event4_deleted2 = (trick.get_event("event4") == None)
""")
expected.append(2+((stop_time-4)*10))
result.append(0)

# TEST 5: cyclic event, set hold on so that it continues to fire until hold off
event5 = trick.new_event("event5")
event5.condition(0, "trick.exec_get_sim_time()==1.0")
event5.condition_hold_on(0)
event5.action(0, "print (\"event5\"); result[5] += 1");
event5.action(1, "event5.activate()")
event5.set_cycle(1.0)
event5.activate() 
trick.add_event(event5)
trick.add_read(4.0, """event5.condition_hold_off(0)""")
expected.append(3)
result.append(0)

# TEST 6: cyclic event, condition job, then action_disable so it is not reactivated
event6 = trick.new_event("event6")
event6.condition_job(0, "ev.cond_job_true")
event6.action(0, "print (\"event6\"); result[6] += 1");
event6.action(1, "event6.activate()")
event6.set_cycle(1.0)
event6.activate() 
trick.add_event(event6)
trick.add_read(3.0, """event6.action_disable(1)""")
expected.append(4)
result.append(0)

# TEST 7: cyclic event, condition_all true & false, then condition_disable the false one so event will fire
event7 = trick.new_event("event7")
event7.condition_var(0, "ev.cond_var_true")
event7.condition_var(1, "ev.cond_var_false")
event7.condition_all()
event7.action(0, "print (\"event7\"); result[7] += 1");
event7.action(1, "event7.activate()")
event7.set_cycle(1.0)
event7.activate() 
trick.add_event(event7)
trick.add_read(4.0, """event7.condition_disable(1)""")
expected.append(1+stop_time-4)
result.append(0)

# TEST 8: cyclic event, action_job_off the sched_count job, so ev.count variable should stop updating prior to 2.0
event8 = trick.new_event("event8")
event8.condition(0, "trick.exec_get_sim_time()==2.0")
event8.condition(1, "trick.exec_get_sim_time()==4.0")
event8.condition_any()
event8.action(0, "print (\"event8\"); result[8] = ev.count");
event8.action(1, "event8.activate()")
event8.action_job_off(2, "ev.sched_count")
event8.set_cycle(1.0)
event8.activate() 
trick.add_event(event8)
expected.append(2)
result.append(0)

# TEST 9: cyclic event, action_job is the sched_count job, note: results are partially dependent on event8
event9 = trick.new_event("event9")
event9.condition(0, "trick.exec_get_sim_time()==5.0")
event9.action_job(0, "ev.sched_count")
event9.action_job(1, "ev.sched_count")
event9.action_job(2, "ev.sched_count")
event9.action_job(3, "ev.sched_count")
event9.action(4, "print (\"event9\"); result[9] = ev.count");
event9.action(5, "event9.activate()")
event9.set_cycle(1.0)
event9.activate() 
trick.add_event(event9)
expected.append(6)
result.append(0)

# TEST 10: manual_on event fires every cycle
manual10 = trick.new_event("manual10")
manual10.condition(0, "False")
manual10.action(0, "print (\"manual10\"); result[10] += 1");
# you don't have to reactivate a manual mode event
manual10.set_cycle(1.0)
manual10.activate() 
manual10.manual_on() 
trick.add_event(manual10)
expected.append(1+stop_time)
result.append(0)

# TEST 11: manual_off event does not fire, manual_done will make condition be used (cyclic)
manual11 = trick.new_event("manual11")
manual11.condition(0, "True")
manual11.action(0, "print (\"manual11\"); result[11] += 1");
manual11.action(1, "manual11.activate()")
manual11.set_cycle(1.0)
manual11.activate() 
manual11.manual_off() 
trick.add_event(manual11)
trick.add_read(4.0, """manual11.manual_done()""")
expected.append(1+stop_time-4)
result.append(0)

# TEST 12: manual_fire at a certain time and see if it fires at that time
manual12 = trick.new_event("manual12")
manual12.action(0, "print (\"manual12\"); result[12] = trick.exec_get_sim_time()");
manual12.action(1, "manual12.activate()")
manual12.set_cycle(1.0)
manual12.activate() 
trick.add_event(manual12)
trick.add_read(3.5, """manual12.manual_fire()""")
expected.append(3.5)
result.append(0)

##########################################################
# TEST RESULTS AT SHUTDOWN
result_event = trick.new_event("result_event")
result_event.condition(0, "True")
result_event.action(0, """
print (\"RESULTS...\")
TRICK_EXPECT_EQ(result[0], expected[0], test_suite, "event0")
TRICK_EXPECT_EQ(result[1], expected[1], test_suite, "event1")
TRICK_EXPECT_EQ(result[2], expected[2], test_suite, "event2")
TRICK_EXPECT_EQ(result[3], expected[3], test_suite, "event3")
TRICK_EXPECT_TRUE(event4_deleted1, test_suite, "event4_deleted1")
TRICK_EXPECT_FALSE(event4_deleted2, test_suite, "event4_deleted2")
TRICK_EXPECT_EQ(result[4], expected[4], test_suite, "event4")
TRICK_EXPECT_EQ(result[5], expected[5], test_suite, "event5")
TRICK_EXPECT_EQ(result[6], expected[6], test_suite, "event6")
TRICK_EXPECT_EQ(result[7], expected[7], test_suite, "event7")
TRICK_EXPECT_EQ(result[8], expected[8], test_suite, "event8")
TRICK_EXPECT_EQ(result[9], expected[9], test_suite, "event9")
TRICK_EXPECT_EQ(result[10], expected[10], test_suite, "manual10")
TRICK_EXPECT_EQ(result[11], expected[11], test_suite, "manual11")
TRICK_EXPECT_EQ(result[12], expected[12], test_suite, "manual12")
""")
result_event.activate() 
trick.add_event_after(result_event, "ev.shutdown")

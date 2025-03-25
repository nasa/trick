
# Data recording test
drg0 = trick.DRBinary("Ball")
for param in [ 'position' , 'velocity' , 'acceleration' , 'external_force' ] :
    for index in range(0,2) :
        var = "ball.output_" + param + "[" + str(index) + "]"
        drg0.add_variable(var)
#drg0.set_cycle(0.01)
drg0.freq = trick.DR_Always
trick_data_record.drd.add_group(drg0, trick.DR_No_Buffer)

my_integ_loop.add_rate(0.005)

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

read = 0.0
trick.add_read(read , """trick.checkpoint("chkpnt_pre_init")""")

read = 300.0
trick.add_read(read , """trick.checkpoint("chkpnt_300.0")""")

trick.exec_set_terminate_time(300.0)


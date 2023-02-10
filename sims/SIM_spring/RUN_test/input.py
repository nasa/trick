# Empty input file

drg0 = trick.DRBinary("spring")
drg0.set_freq(trick.DR_Always)
drg0.enable()
drg0.set_cycle(0.01)
drg0.add_variable("spring.pos", "spring.pos")
drg0.add_variable("spring.vel", "spring.vel")
drg0.add_variable("spring.acc", "spring.acc")
trick.add_data_record_group( drg0 )

trick.mc_set_enabled(1)
trick.mc_set_num_runs(100)

var0 = trick.MonteVarFile("spring.zeta", "monte_file.txt", 2, "--")
trick_mc.mc.add_variable(var0)

trick.exec_set_terminate_time(10.0)

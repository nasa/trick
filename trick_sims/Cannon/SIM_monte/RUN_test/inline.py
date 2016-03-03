
execfile("Modified_data/cannon_aero.dr")

trick.mc_set_enabled(1)
#trick.mc_set_dry_run(1)
trick.mc_set_num_runs(10)
#trick.mc_add_range(5)
# Inline method below
var0 = trick.MonteVarFile("dyn.baseball.time_to_fire_jet_1","M_jet_firings_inline", 2)
var1 = trick.MonteVarFile("dyn.baseball.time_to_fire_jet_2","M_jet_firings_inline", 3,"s")
var2 = trick.MonteVarFile("dyn.baseball.time_to_fire_jet_3","M_jet_firings_inline", 4)
var3 = trick.MonteVarFile("dyn.baseball.time_to_fire_jet_4","M_jet_firings_inline", 5,"s")

trick_mc.mc.add_variable(var0)
trick_mc.mc.add_variable(var1)
trick_mc.mc.add_variable(var2)
trick_mc.mc.add_variable(var3)
'''
trick.mc_set_listen_device_port(7200)
trick.mc_set_data_listen_device_port(7205)
trick.mc_set_connection_device_port(7201)
trick.mc_set_data_connection_device_port(7215)

print trick.mc_get_listen_device_port()
print trick.mc_get_data_listen_device_port()
print trick.mc_get_connection_device_port()
print trick.mc_get_data_connection_device_port()
'''
#slave0 = trick.MonteSlave("megatron")
#trick_sys.sched.add_slave(slave0)
#slave0.remote_shell = trick.TRICK_USER_REMOTE_SH
#slave0.user_remote_shell = "/bin/tcsh"
#slave0.remote_shell_args = "-c"

dyn.baseball.pos[0] = 0.0
dyn.baseball.pos[1] = 0.0
dyn.baseball.pos[2] = 0.0

dyn.baseball.vel[0] = 43.30
dyn.baseball.vel[1] = 0.0
dyn.baseball.vel[2] = 25.0

dyn.baseball.theta  = trick.attach_units("d" , -90.0)
dyn.baseball.phi    = trick.attach_units("d" , 1.0)
dyn.baseball.omega0 = trick.attach_units("rev/s" , 30.0)

dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 6)

trick.sim_services.exec_set_terminate_time(10.0)

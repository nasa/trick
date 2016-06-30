
import math
from trick.unit_test import *

if trick.mc_get_slave_id() == 0:
    trick_utest.unit_tests.enable();
    trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_monte.xml" )

trick.mc_set_enabled(1)
#trick.mc_set_dry_run(1)
trick.mc_set_num_runs(10)

slave0 = trick.MonteSlave("localhost")
trick_mc.mc.add_slave(slave0)
slave0.set_S_main_name("./T_main_${TRICK_HOST_CPU}_test.exe")

#trick.mc_add_range(5)
# Inline method below
var0 = trick.MonteVarFile("dyn.baseball.time_to_fire_jet_1","M_jet_firings_inline", 2)

var1 = trick.MonteVarFile("dyn.baseball.time_to_fire_jet_2","M_jet_firings_inline", 3, "s")

var2 = trick.MonteVarRandom("dyn.baseball.time_to_fire_jet_3",trick.MonteVarRandom.GAUSSIAN)
var2.set_seed(3)
var2.set_sigma(0.6667)
var2.set_mu(4.0)
var2.set_min(-4.0)
var2.set_max(4.0)

var3 = trick.MonteVarRandom("dyn.baseball.time_to_fire_jet_4",trick.MonteVarRandom.GAUSSIAN, "s")
var3.set_seed(4)
var3.set_sigma(0.6667)
var3.set_mu(4.0)
var3.set_min(-4.0)
var3.set_max(4.0)

trick_mc.mc.add_variable(var0)
trick_mc.mc.add_variable(var1)
trick_mc.mc.add_variable(var2)
trick_mc.mc.add_variable(var3)

dyn.baseball.pos[0] = 0.0
dyn.baseball.pos[1] = 0.0
dyn.baseball.pos[2] = 0.0

dyn.baseball.vel[0] = 43.30
dyn.baseball.vel[1] = 0.0
dyn.baseball.vel[2] = 25.0

dyn.baseball.theta  = trick.attach_units("degree" , -90.0)
dyn.baseball.phi    = trick.attach_units("degree" , 1.0)
dyn.baseball.omega0 = trick.attach_units("revolution/s" , 30.0)

dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 6)

trick.sim_services.exec_set_terminate_time(10.0)

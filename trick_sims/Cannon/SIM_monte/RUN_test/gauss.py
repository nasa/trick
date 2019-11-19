
exec(open("Modified_data/cannon_aero.dr").read())

dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 6)

trick.mc_set_enabled(1)
#trick.mc_set_dry_run(1)
trick.mc_set_num_runs(50)
#trick.mc_add_range(5)
# Inline method below

# If configure trick build with -std=c++0x or -std=c++11 if available,
# can use trick.MonteVarRandom.TRICK_DEFAULT_ENGINE, or other selections.
# to use C++11 <random> pseudo-random number generators

var0 = trick.MonteVarRandom("dyn.baseball.time_to_fire_jet_1",trick.MonteVarRandom.GAUSSIAN,"s",trick.MonteVarRandom.NO_ENGINE) # NO_ENGINE is default
var0.set_seed(1)
var0.set_sigma(0.6667)
var0.set_mu(4.0)
var0.set_min(-4.0)
var0.set_max(4.0)
var0.set_min_is_relative(True) # default is True
var0.set_max_is_relative(True) # default is True
var0.set_sigma_range(0) # 1 is default, so usually set to zero to turn off the algorithm

var1 = trick.MonteVarRandom("dyn.baseball.time_to_fire_jet_2",trick.MonteVarRandom.GAUSSIAN)
var1.set_seed(2)
var1.set_sigma(0.6667)
var1.set_mu(4.0)
var1.set_min(-4.0)
var1.set_max(4.0)
var1.set_sigma_range(0)

var2 = trick.MonteVarRandom("dyn.baseball.time_to_fire_jet_3",trick.MonteVarRandom.GAUSSIAN,"s")
var2.set_seed(3)
var2.set_sigma(0.6667)
var2.set_mu(4.0)
var2.set_min(-4.0)
var2.set_max(4.0)
var2.set_sigma_range(0)

var3 = trick.MonteVarRandom("dyn.baseball.time_to_fire_jet_4",trick.MonteVarRandom.GAUSSIAN)
var3.set_seed(4)
var3.set_sigma(0.6667)
var3.set_mu(4.0)
var3.set_min(-4.0)
var3.set_max(4.0)
var3.set_sigma_range(0)

trick_mc.mc.add_variable(var0)
trick_mc.mc.add_variable(var1)
trick_mc.mc.add_variable(var2)
trick_mc.mc.add_variable(var3)

trick.mc_set_timeout(2)

dyn.baseball.pos[0] = 0.0
dyn.baseball.pos[1] = 0.0
dyn.baseball.pos[2] = 0.0

dyn.baseball.vel[0] = 43.30
dyn.baseball.vel[1] = 0.0
dyn.baseball.vel[2] = 25.0

dyn.baseball.theta  = trick.attach_units("d" , -90.0)
dyn.baseball.phi    = trick.attach_units("d" , 1.0)
dyn.baseball.omega0 = trick.attach_units("rev/s" , 30.0)


trick.stop(10.0)


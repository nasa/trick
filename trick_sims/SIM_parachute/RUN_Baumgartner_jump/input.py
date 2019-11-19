
exec(open("Modified_data/parachutist.dr").read())
trick.TMM_reduced_checkpoint(0)
dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 2)

# The ground is a 1000 meters above sea level.
dyn.groundAltitude = 1000

# State of the skydiver when he jumps
dyn.parachutist.altitude = 38969.6 # 127852 feet
dyn.parachutist.velocity = 0.0
dyn.parachutist.area = 0.75
dyn.parachutist.Cd = 0.75
dyn.parachutist.mass = 82.0

# At 4 minutes and 19 seconds, pop the chute.
deployChute = trick.new_event("deployChute")
deployChute.condition(0, "trick.exec_get_sim_time() >= 259.0")
deployChute.action(0, "dyn.parachutist.Cd = 1.3");
deployChute.action(1, "dyn.parachutist.area = 30.0");
deployChute.action(2, "print \"Parachute Deployed.\"");
deployChute.set_cycle(1.0)
deployChute.activate()
trick.add_event(deployChute)

# Run for 800 seconds.
trick.stop(800)

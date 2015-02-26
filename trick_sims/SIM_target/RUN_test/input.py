import math

target_integloop.getIntegrator(trick.Runge_Kutta_Fehlberg_45, 42)

# Set up the Monte Carlo runs. 
#trick.mc_set_dry_run(1)
trick.mc_set_enabled(1)
trick.mc_set_num_runs(200)
var0 = trick.MonteVarCalculated("target.target_state.velocity[0]")
var1 = trick.MonteVarCalculated("target.target_state.velocity[1]","m/s")
var2 = trick.MonteVarCalculated("target.target_state.velocity[2]")
trick_mc.mc.add_variable(var0)
trick_mc.mc.add_variable(var1)
trick_mc.mc.add_variable(var2)

#slave0 = trick.MonteSlave("localhost")
#slave1 = trick.MonteSlave("localhost")
#slave0 = trick.MonteSlave("Megatron")
#trick_mc.mc.add_slave(slave0)
#trick_mc.mc.add_slave(slave1)


# Set the iteration limit. */
iterate.iterate_data.iter_max = 12;

# Set the initial body position. */
target.target_state.position[0] = trick.attach_units( "km", -7378.0)
target.target_state.position[1] = trick.attach_units( "km", 0.0)
target.target_state.position[2] = trick.attach_units( "km", 0.0)

# Compute the initial circular velocity from position. */
#target.state.velocity[0] {km/s} = 0.0, 7.350214, 0.0;
pi = 3.14159265358979323846264338328;
mu = float(target.target_planet.mu);
r_mag = math.sqrt(   (float(target.target_state.position[0]) * float(target.target_state.position[0]))
                   + (float(target.target_state.position[1]) * float(target.target_state.position[1]))
                   + (float(target.target_state.position[2]) * float(target.target_state.position[2])) );

r_3 = r_mag * r_mag * r_mag;
target.target_state.velocity[0] = 0.0;
target.target_state.velocity[1] = math.sqrt( mu / r_mag );
target.target_state.velocity[2] = 0.0;

# Set the desired terminal position. */
target.target_data.position[0] = trick.attach_units( "km", 2*7378.0)
target.target_data.position[1] = trick.attach_units( "km", 0.0)
target.target_data.position[2] = trick.attach_units( "km", 0.0)

# Compute the stop time. */
trick.stop( pi * math.sqrt( r_3 / mu ) )

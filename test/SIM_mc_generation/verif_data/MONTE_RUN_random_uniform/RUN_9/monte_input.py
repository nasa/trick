monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_uniform/input.py').read())
monte_carlo.mc_master.monte_run_number = 9

test.x_uniform = 1930.026637775263
test.x_integer = 96421

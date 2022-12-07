monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_uniform/input.py').read())
monte_carlo.mc_master.monte_run_number = 4

test.x_uniform = 81653.07878167802
test.x_integer = 28114

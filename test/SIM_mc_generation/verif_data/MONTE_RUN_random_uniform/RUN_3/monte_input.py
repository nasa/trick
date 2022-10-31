test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_random_uniform/input.py').read())
test.mc_master.monte_run_number = 3

test.x_uniform = 5338.88635155008
test.x_integer = 2546

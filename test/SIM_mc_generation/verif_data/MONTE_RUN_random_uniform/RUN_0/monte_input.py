test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_random_uniform/input.py').read())
test.mc_master.monte_run_number = 0

test.x_uniform = 66000.76700176572
test.x_integer = 47109

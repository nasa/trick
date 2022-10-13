test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_random_uniform/input.py').read())
test.mc_master.monte_run_number = 7

test.x_uniform = 83068.39187988771
test.x_integer = 76648

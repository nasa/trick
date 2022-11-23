monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_file_sequential/input.py').read())
monte_carlo.mc_master.monte_run_number = 4

test.x_file_lookup[0] = 2
test.x_file_lookup[1] = 1
test.x_file_lookup[2] = 0

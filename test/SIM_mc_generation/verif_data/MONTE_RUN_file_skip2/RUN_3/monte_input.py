test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_file_skip2/input.py').read())
test.mc_master.monte_run_number = 3

test.x_file_lookup[0] = 08
test.x_file_lookup[1] = 25
test.x_file_lookup[2] = 43

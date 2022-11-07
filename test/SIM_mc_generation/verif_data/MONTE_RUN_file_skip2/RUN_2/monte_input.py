test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_file_skip2/input.py').read())
test.mc_master.monte_run_number = 2

test.x_file_lookup[0] = 06
test.x_file_lookup[1] = 22
test.x_file_lookup[2] = 39

test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_file_sequential/input.py').read())
test.mc_master.monte_run_number = 7

test.x_file_lookup[0] = 32
test.x_file_lookup[1] = 31
test.x_file_lookup[2] = 30

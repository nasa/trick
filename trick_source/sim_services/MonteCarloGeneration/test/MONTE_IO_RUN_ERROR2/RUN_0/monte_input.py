test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('IO_RUN_ERROR2/input.py').read())
test.mc_master.monte_run_number = 0

test.x_file_lookup[0] = 2

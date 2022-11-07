test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_random_normal__untruncate/input.py').read())
test.mc_master.monte_run_number = 0

test.x_normal_trunc[0] = 9.954870507417668
test.x_normal_trunc[1] = 9.954870507417668

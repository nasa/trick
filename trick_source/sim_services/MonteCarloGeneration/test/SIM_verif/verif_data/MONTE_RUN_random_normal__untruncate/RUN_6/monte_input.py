test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_random_normal__untruncate/input.py').read())
test.mc_master.monte_run_number = 6

test.x_normal_trunc[0] = 14.56731728448253
test.x_normal_trunc[1] = 14.56731728448253

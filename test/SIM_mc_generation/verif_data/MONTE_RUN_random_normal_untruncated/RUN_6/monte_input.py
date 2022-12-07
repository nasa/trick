monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_untruncated/input.py').read())
monte_carlo.mc_master.monte_run_number = 6

test.x_normal_trunc[0] = -0.515827640920414
test.x_normal_trunc[1] = 73.94635381551484
test.x_normal_trunc[2] = 94.74949569148831
test.x_normal_trunc[3] = 123.3649345465698

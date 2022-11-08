monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_truncate_rel/input.py').read())
monte_carlo.mc_master.monte_run_number = 5

monte_carlo.x_normal_trunc[0] = -0.515827640920414
monte_carlo.x_normal_trunc[1] = 73.94635381551484
monte_carlo.x_normal_trunc[2] = 105.1087287464966
monte_carlo.x_normal_trunc[3] = 127.2490899001316

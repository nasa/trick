monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_truncate_sd/input.py').read())
monte_carlo.mc_master.monte_run_number = 2

monte_carlo.x_normal_trunc[0] = 1.461034784495358
monte_carlo.x_normal_trunc[1] = 84.51824337814119
monte_carlo.x_normal_trunc[2] = 104.588645782558
monte_carlo.x_normal_trunc[3] = 133.1532695604445

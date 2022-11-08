monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal__untruncate/input.py').read())
monte_carlo.mc_master.monte_run_number = 9

monte_carlo.x_normal_trunc[0] = 11.9273239842278
monte_carlo.x_normal_trunc[1] = 11.9273239842278

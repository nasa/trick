monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal__untruncate/input.py').read())
monte_carlo.mc_master.monte_run_number = 2

monte_carlo.x_normal_trunc[0] = 8.225020001340255
monte_carlo.x_normal_trunc[1] = 8.225020001340255

monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal__untruncate/input.py').read())
monte_carlo.mc_master.monte_run_number = 8

test.x_normal_trunc[0] = 11.10848764602406
test.x_normal_trunc[1] = 11.10848764602406

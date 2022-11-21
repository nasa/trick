monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal__untruncate/input.py').read())
monte_carlo.mc_master.monte_run_number = 1

test.x_normal_trunc[0] = 11.32489560639709
test.x_normal_trunc[1] = 11.32489560639709

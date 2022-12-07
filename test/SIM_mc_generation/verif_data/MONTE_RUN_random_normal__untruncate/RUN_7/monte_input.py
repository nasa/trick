monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal__untruncate/input.py').read())
monte_carlo.mc_master.monte_run_number = 7

test.x_normal_trunc[0] = 9.489924230632374
test.x_normal_trunc[1] = 9.489924230632374

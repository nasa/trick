monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_untruncated/input.py').read())
monte_carlo.mc_master.monte_run_number = 2

test.x_normal_trunc[0] = -10.401646875566
test.x_normal_trunc[1] = 84.51824337814119
test.x_normal_trunc[2] = 104.588645782558
test.x_normal_trunc[3] = 133.1532695604445

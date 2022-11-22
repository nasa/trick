monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_truncate_rel/input.py').read())
monte_carlo.mc_master.monte_run_number = 7

test.x_normal_trunc[0] = 0.5120114132190287
test.x_normal_trunc[1] = 74.6382876071584
test.x_normal_trunc[2] = 100.6542128088184
test.x_normal_trunc[3] = 129.480796762798

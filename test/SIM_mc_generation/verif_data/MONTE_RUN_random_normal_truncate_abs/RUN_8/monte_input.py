monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_truncate_abs/input.py').read())
monte_carlo.mc_master.monte_run_number = 8

test.x_normal_trunc[0] = 0.1507923509436825
test.x_normal_trunc[1] = 73.53475552621356
test.x_normal_trunc[2] = 110.2710903282531
test.x_normal_trunc[3] = 128.6747991010428

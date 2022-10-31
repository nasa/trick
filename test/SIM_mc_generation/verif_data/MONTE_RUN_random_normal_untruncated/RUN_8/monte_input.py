test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_untruncated/input.py').read())
test.mc_master.monte_run_number = 8

test.x_normal_trunc[0] = 0.5120114132190287
test.x_normal_trunc[1] = 78.79536897207045
test.x_normal_trunc[2] = 110.2710903282531
test.x_normal_trunc[3] = 128.6747991010428

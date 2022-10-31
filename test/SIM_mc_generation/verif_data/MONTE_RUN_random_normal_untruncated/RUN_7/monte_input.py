test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_untruncated/input.py').read())
test.mc_master.monte_run_number = 7

test.x_normal_trunc[0] = 3.085497251625431
test.x_normal_trunc[1] = 71.13283645956025
test.x_normal_trunc[2] = 100.6542128088184
test.x_normal_trunc[3] = 129.480796762798

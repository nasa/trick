test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_untruncated/input.py').read())
test.mc_master.monte_run_number = 5

test.x_normal_trunc[0] = 3.900928181442498
test.x_normal_trunc[1] = 72.11325027756652
test.x_normal_trunc[2] = 105.1087287464966
test.x_normal_trunc[3] = 127.2490899001316

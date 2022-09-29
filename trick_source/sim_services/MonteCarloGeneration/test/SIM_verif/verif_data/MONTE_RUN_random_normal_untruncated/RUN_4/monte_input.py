test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_untruncated/input.py').read())
test.mc_master.monte_run_number = 4

test.x_normal_trunc[0] = 1.534953256077651
test.x_normal_trunc[1] = 72.86668090542759
test.x_normal_trunc[2] = 94.81797011312233
test.x_normal_trunc[3] = 134.5453170649493

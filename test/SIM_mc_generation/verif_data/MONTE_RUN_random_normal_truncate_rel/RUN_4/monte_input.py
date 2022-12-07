monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_truncate_rel/input.py').read())
monte_carlo.mc_master.monte_run_number = 4

test.x_normal_trunc[0] = 3.900928181442498
test.x_normal_trunc[1] = 72.86668090542759
test.x_normal_trunc[2] = 94.81797011312233
test.x_normal_trunc[3] = 134.5453170649493

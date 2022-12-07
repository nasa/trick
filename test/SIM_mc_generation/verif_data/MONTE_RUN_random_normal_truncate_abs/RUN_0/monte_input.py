monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_truncate_abs/input.py').read())
monte_carlo.mc_master.monte_run_number = 0

test.x_normal_trunc[0] = 6.461374027019938
test.x_normal_trunc[1] = 76.32067515735949
test.x_normal_trunc[2] = 100.8063868905812
test.x_normal_trunc[3] = 118.8667092869175

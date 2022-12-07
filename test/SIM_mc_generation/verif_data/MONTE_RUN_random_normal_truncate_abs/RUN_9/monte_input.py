monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_truncate_abs/input.py').read())
monte_carlo.mc_master.monte_run_number = 9

test.x_normal_trunc[0] = -3.033526117216713
test.x_normal_trunc[1] = 76.26213327382
test.x_normal_trunc[2] = 98.5795871899536
test.x_normal_trunc[3] = 129.7063625385798

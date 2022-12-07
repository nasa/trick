monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_truncate_sd/input.py').read())
monte_carlo.mc_master.monte_run_number = 6

test.x_normal_trunc[0] = 3.085497251625431
test.x_normal_trunc[1] = 78.79536897207045
test.x_normal_trunc[2] = 94.74949569148831
test.x_normal_trunc[3] = 123.3649345465698

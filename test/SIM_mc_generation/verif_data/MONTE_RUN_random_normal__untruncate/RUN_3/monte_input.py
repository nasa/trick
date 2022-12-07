monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal__untruncate/input.py').read())
monte_carlo.mc_master.monte_run_number = 3

test.x_normal_trunc[0] = 9.078215205210737
test.x_normal_trunc[1] = 9.078215205210737

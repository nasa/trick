test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_random_normal__untruncate/input.py').read())
test.mc_master.monte_run_number = 4

test.x_normal_trunc[0] = 8.61231801622891
test.x_normal_trunc[1] = 8.61231801622891

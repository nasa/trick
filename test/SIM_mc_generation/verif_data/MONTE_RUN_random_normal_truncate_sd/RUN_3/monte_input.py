monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_truncate_sd/input.py').read())
monte_carlo.mc_master.monte_run_number = 3

test.x_normal_trunc[0] = 1.534953256077651
test.x_normal_trunc[1] = 73.27087617076381
test.x_normal_trunc[2] = 93.62581014461907
test.x_normal_trunc[3] = 122.3943648119438

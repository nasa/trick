test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_random_normal_truncate_rel/input.py').read())
test.mc_master.monte_run_number = 2

test.x_normal_trunc[0] = 1.461034784495358
test.x_normal_trunc[1] = 84.51824337814119
test.x_normal_trunc[2] = 104.588645782558
test.x_normal_trunc[3] = 133.1532695604445

test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_random_uniform/input.py').read())
test.mc_master.monte_run_number = 1

test.x_uniform = 81004.55299703917
test.x_integer = 78712

test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_WARN_invalid_name/input.py').read())
test.mc_master.monte_run_number = 0

test.x_uniform = 15.92844616516683

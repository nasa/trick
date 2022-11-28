monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_uniform/input.py').read())
monte_carlo.mc_master.monte_run_number = 6

test.x_uniform = 7306.365748693855
test.x_integer = 82893

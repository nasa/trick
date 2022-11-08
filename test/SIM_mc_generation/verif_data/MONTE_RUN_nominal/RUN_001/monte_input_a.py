monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_nominal/input_a.py').read())
monte_carlo.mc_master.monte_run_number = 1

monte_carlo.x_uniform = 18.44265744256598
monte_carlo.x_normal = 11.32489560639709
monte_carlo.x_normal_trunc[0] = 9.078215205210737
monte_carlo.x_normal_trunc[1] = 9.912272360185705
monte_carlo.x_normal_trunc[2] = 10.72611121241102
monte_carlo.x_normal_trunc[3] = 11.32489560639709
monte_carlo.x_normal_trunc[4] = 3.806042167887552
monte_carlo.x_normal_length = trick.attach_units("ft", 11.32489560639709)
monte_carlo.x_integer = 2
monte_carlo.x_string = 'GHIJKL'
monte_carlo.x_line_command = monte_carlo.x_integer * monte_carlo.x_uniform
monte_carlo.standalone_function( monte_carlo.x_normal)
exec(open('Modified_data/sample.py').read())
monte_carlo.x_boolean = False
monte_carlo.x_file_lookup[0] = 12
monte_carlo.x_file_lookup[1] = 11
monte_carlo.x_file_lookup[2] = 10
monte_carlo.x_fixed_value_int = 7
monte_carlo.x_fixed_value_double = 7
monte_carlo.x_fixed_value_string = "7"
monte_carlo.x_semi_fixed_value =  2

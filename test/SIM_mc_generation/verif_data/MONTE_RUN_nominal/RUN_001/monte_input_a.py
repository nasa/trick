monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_nominal/input_a.py').read())
monte_carlo.mc_master.monte_run_number = 1

test.x_uniform = 18.44265744256598
test.x_normal = 11.32489560639709
test.x_normal_trunc[0] = 9.078215205210737
test.x_normal_trunc[1] = 9.912272360185705
test.x_normal_trunc[2] = 10.72611121241102
test.x_normal_trunc[3] = 11.32489560639709
test.x_normal_trunc[4] = 3.806042167887552
test.x_normal_length = trick.attach_units("ft", 11.32489560639709)
test.x_integer = 2
test.x_string = 'GHIJKL'
test.x_line_command = test.x_integer * test.x_uniform
test.standalone_function( test.x_normal)
exec(open('Modified_data/sample.py').read())
test.x_boolean = False
test.x_file_lookup[0] = 12
test.x_file_lookup[1] = 11
test.x_file_lookup[2] = 10
test.x_fixed_value_int = 7
test.x_fixed_value_double = 7
test.x_fixed_value_string = "7"
test.x_semi_fixed_value =  2

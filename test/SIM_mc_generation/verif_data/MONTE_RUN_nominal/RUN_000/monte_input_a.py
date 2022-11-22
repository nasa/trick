monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_nominal/input_a.py').read())
monte_carlo.mc_master.monte_run_number = 0

test.x_uniform = 15.92844616516683
test.x_normal = 9.954870507417668
test.x_normal_trunc[0] = 9.954870507417668
test.x_normal_trunc[1] = 9.954870507417668
test.x_normal_trunc[2] = 9.954870507417668
test.x_normal_trunc[3] = 9.954870507417668
test.x_normal_trunc[4] = 3.772280419911035
test.x_normal_length = trick.attach_units("ft", 9.954870507417668)
test.x_integer = 1
test.x_string = "ABC"
test.x_line_command = test.x_integer * test.x_uniform
test.standalone_function( test.x_normal)
exec(open('Modified_data/sample.py').read())
test.x_boolean = True
test.x_file_lookup[0] = 2
test.x_file_lookup[1] = 1
test.x_file_lookup[2] = 0
test.x_fixed_value_int = 7
test.x_fixed_value_double = 7
test.x_fixed_value_string = "7"
test.x_semi_fixed_value =  2

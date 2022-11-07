monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_nominal/input_a.py').read())
monte_carlo.mc_master.monte_run_number = 0

monte_carlo.x_uniform = 15.92844616516683
monte_carlo.x_normal = 9.954870507417668
monte_carlo.x_normal_trunc[0] = 9.954870507417668
monte_carlo.x_normal_trunc[1] = 9.954870507417668
monte_carlo.x_normal_trunc[2] = 9.954870507417668
monte_carlo.x_normal_trunc[3] = 9.954870507417668
monte_carlo.x_normal_trunc[4] = 3.772280419911035
monte_carlo.x_normal_length = trick.attach_units("ft", 9.954870507417668)
monte_carlo.x_integer = 1
monte_carlo.x_string = "ABC"
monte_carlo.x_line_command = monte_carlo.x_integer * monte_carlo.x_uniform
monte_carlo.standalone_function( monte_carlo.x_normal)
exec(open('Modified_data/sample.py').read())
monte_carlo.x_boolean = True
monte_carlo.x_file_lookup[0] = 2
monte_carlo.x_file_lookup[1] = 1
monte_carlo.x_file_lookup[2] = 0
monte_carlo.x_fixed_value_int = 7
monte_carlo.x_fixed_value_double = 7
monte_carlo.x_fixed_value_string = "7"
monte_carlo.x_semi_fixed_value =  2

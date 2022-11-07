mc_var = trick.MonteCarloVariableRandomUniform( "monte_carlo.x_uniform", 0, 10, 20)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableRandomNormal( "monte_carlo.x_normal", 2, 10, 2)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableRandomNormal( "monte_carlo.x_normal_trunc[0]", 2, 10, 2)
mc_var.truncate(0.5, trick.MonteCarloVariableRandomNormal.StandardDeviation)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableRandomNormal( "monte_carlo.x_normal_trunc[1]", 2, 10, 2)
mc_var.truncate(-0.5, 0.7, trick.MonteCarloVariableRandomNormal.Relative)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableRandomNormal( "monte_carlo.x_normal_trunc[2]", 2, 10, 2)
mc_var.truncate(9.9,11, trick.MonteCarloVariableRandomNormal.Absolute)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableRandomNormal( "monte_carlo.x_normal_trunc[3]", 2, 10, 2)
mc_var.truncate_low(9.9, trick.MonteCarloVariableRandomNormal.Absolute)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableRandomNormal( "monte_carlo.x_normal_trunc[4]", 2, 10, 2)
mc_var.truncate_high(4, trick.MonteCarloVariableRandomNormal.Absolute)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableRandomNormal( "monte_carlo.x_normal_length", 2, 10, 2)
mc_var.units = "ft"
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableRandomUniformInt( "monte_carlo.x_integer", 1, 0, 2)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableRandomStringSet( "monte_carlo.x_string", 3)
mc_var.add_string("\"ABC\"")
mc_var.add_string("\"DEF\"")
mc_var.add_string("'GHIJKL'")
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloPythonLineExec( "monte_carlo.x_line_command",
                                         "monte_carlo.x_integer * monte_carlo.x_uniform")
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloPythonLineExec(
      "monte_carlo.standalone_function( monte_carlo.x_normal)")
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloPythonFileExec( "Modified_data/sample.py")
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableRandomBool( "monte_carlo.x_boolean", 4)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

# call this one mc_var1 because I'm going to use it as the seed for the
# MonteCarloVariableSemiFixed later.
mc_var1 = trick.MonteCarloVariableFile( "monte_carlo.x_file_lookup[0]",
                                        "Modified_data/datafile.txt",
                                        3)
mc_var1.thisown = False
monte_carlo.mc_master.add_variable(mc_var1)

mc_var = trick.MonteCarloVariableFile( "monte_carlo.x_file_lookup[1]",
                                        "Modified_data/datafile.txt",
                                        2)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableFile( "monte_carlo.x_file_lookup[2]",
                                        "Modified_data/datafile.txt",
                                        1)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)


mc_var = trick.MonteCarloVariableFixed( "monte_carlo.x_fixed_value_int", 7)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableFixed( "monte_carlo.x_fixed_value_double", 7.0)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableFixed( "monte_carlo.x_fixed_value_string", "\"7\"")
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableSemiFixed( "monte_carlo.x_semi_fixed_value", mc_var1 )
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)


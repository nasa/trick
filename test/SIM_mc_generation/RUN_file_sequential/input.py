monte_carlo.mc_master.activate("RUN_file_sequential")
monte_carlo.mc_master.set_num_runs(10)


mc_var0 = trick.MonteCarloVariableFile( "test.x_file_lookup[0]",
                                        "Modified_data/datafile.txt",
                                        3)
monte_carlo.mc_master.add_variable(mc_var0)

mc_var1 = trick.MonteCarloVariableFile( "test.x_file_lookup[1]",
                                        "Modified_data/datafile.txt",
                                        2)
monte_carlo.mc_master.add_variable(mc_var1)

mc_var2 = trick.MonteCarloVariableFile( "test.x_file_lookup[2]",
                                        "Modified_data/datafile.txt",
                                        1)
monte_carlo.mc_master.add_variable(mc_var2)


print("\nmc_var0.has_dependents()           returns: " + str(mc_var0.has_dependents()))
print("mc_var0.get_column_number()        returns: " + str(mc_var0.get_column_number()))
print("mc_var0.get_first_column_number()  returns: " + str(mc_var0.get_first_column_number()))
print("mc_var0.get_filename()             returns: '" + mc_var0.get_filename() + "'")
print("\nmc_var1.has_dependents()           returns: " + str(mc_var1.has_dependents()))
print("mc_var1.get_column_number()        returns: " + str(mc_var1.get_column_number()))
print("mc_var1.get_first_column_number()  returns: " + str(mc_var1.get_first_column_number()))
print("mc_var1.get_filename()             returns: '" + mc_var1.get_filename() + "'")
print("\nmc_var2.has_dependents()           returns: " + str(mc_var2.has_dependents()))
print("mc_var2.get_column_number()        returns: " + str(mc_var2.get_column_number()))
print("mc_var2.get_first_column_number()  returns: " + str(mc_var2.get_first_column_number()))
print("mc_var2.get_filename()             returns: '" + mc_var2.get_filename() + "'")

# call parent class' "virtual int get_seed() const" method. should return ZERO.
# code coverage for: mc_variable.hh, line 70
print("\ncode coverage for parent's get_seed() virtual method... should return ZERO.")
print("mc_var2.get_seed()                 returns: " + str(mc_var2.get_seed()))

# Check the validity of looking up a variable by name.
print("\nmonte_carloing 'find_variable' and 'get_variable_name' for test.x_file_lookup[0]: "+
      "returns: " +
      monte_carlo.mc_master.find_variable("test.x_file_lookup[0]").get_variable_name())
print("monte_carloing 'find_variable' and 'get_variable_name' for test.x_file_lookup[1]: "+
      "returns: " +
      monte_carlo.mc_master.find_variable("test.x_file_lookup[1]").get_variable_name() + "\n")


trick.stop(1)

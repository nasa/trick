
trick.mc_set_enabled(0)
trick.mc_set_enabled(1)

trick.mc_set_dry_run(1)

trick.mc_set_num_runs(10)

trick.mc_add_range(0,2)

trick.mc_add_range(5,7)

var0 = trick.MonteVarFile("test_file_var" , "RUN_test/monte_data.txt" , 2 )
var1 = trick.MonteVarFile("test_file_var2" , "RUN_test/monte_data2.txt" , 2 )

var2 = trick.MonteVarRandom("test_random_var" , trick.MonteVarRandom.GAUSSIAN )
var2.set_seed(0) ;

var3 = trick.MonteVarCalculated( "ball.obj.state.input.position[0]" )

var4 = trick.MonteVarFixed( "test_fixed_var" , 6.666 )

trick_mc.mc.add_variable(var0)
trick_mc.mc.add_variable(var1)
trick_mc.mc.add_variable(var2)
trick_mc.mc.add_variable(var3)
trick_mc.mc.add_variable(var4)




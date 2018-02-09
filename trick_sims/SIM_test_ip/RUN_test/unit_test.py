
import math
from trick.unit_test import *

def main():

    my_integ_loop.getIntegrator( trick.Runge_Kutta_2, 4 );

    # These are here as a reference for the add_collect syntax... I have changed the code to not use collect
    # An example of removing collect in the input file (original collect added in S_define file)
    #ball.obj.state.work.external_force = trick.delete_collect(ball.obj.state.work.external_force, ball.obj.force.output.force)
    # An example of adding a collect in the input file
    #ball.obj.state.work.external_force = trick.add_collect(ball.obj.state.work.external_force, ball.obj.force.output.force)

    # An example of turning off a sim_object
    trick.exec_set_sim_object_onoff("disabled_obj" , False)

    trick.exec_set_terminate_time(1.0)

    trick_utest.unit_tests.enable()
    trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_ip.xml" )
    trick_utest.unit_tests.set_test_name( "IPtest" )

######################################################################################################################

    test_suite = "double"

    ball.obj.d = 2
    TRICK_EXPECT_NEAR( ball.obj.d , 2.0 , 0.000001 , test_suite , "no units" )
    trick.trick_test_add_parent( test_suite , "no units" , "910635102")

    ball.obj.d = trick.attach_units("lbm" , 2)
    TRICK_EXPECT_NEAR( ball.obj.d , 0.907185 , 0.000001 , test_suite , "units convert" )

    ball.obj.da = [ 20 , 21 , 22 ]
    TRICK_EXPECT_NEAR( ball.obj.da[0] , 20 , 0.000001 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_NEAR( ball.obj.da[1] , 21 , 0.000001 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_NEAR( ball.obj.da[2] , 22 , 0.000001 , test_suite , "1D array, integer value, no units" )

    ball.obj.da = [ 30.1 , 31.1 , 32.1 ]

    TRICK_EXPECT_NEAR( ball.obj.da[0] , 30.1 , 0.000001 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_NEAR( ball.obj.da[1] , 31.1 , 0.000001 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_NEAR( ball.obj.da[2] , 32.1 , 0.000001 , test_suite , "1D array, float value, no units" )

    ball.obj.da = 40.1 , 41.1 , 42.1

    TRICK_EXPECT_NEAR( ball.obj.da[0] , 40.1 , 0.000001 , test_suite , "1D array, tuple float value, no units" )
    TRICK_EXPECT_NEAR( ball.obj.da[1] , 41.1 , 0.000001 , test_suite , "1D array, tuple float value, no units" )
    TRICK_EXPECT_NEAR( ball.obj.da[2] , 42.1 , 0.000001 , test_suite , "1D array, tuple float value, no units" )

    ball.obj.da = trick.attach_units("lbm" , [2 , 3 , 4])
    TRICK_EXPECT_NEAR( ball.obj.da[0] , 0.907185 , 0.000001 , test_suite , "1D array, float value, units convert" )
    TRICK_EXPECT_NEAR( ball.obj.da[1] , 1.36078 ,  0.00001  , test_suite , "1D array, float value, units convert" )
    TRICK_EXPECT_NEAR( ball.obj.da[2] , 1.81437 ,  0.00001  , test_suite , "1D array, float value, units convert" )

    ball.obj.dp = trick.TMM_declare_var_s("double[6]")
    TRICK_EXPECT_NEAR( ball.obj.dp[0] , 0 ,  0.00001  , test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_NEAR( ball.obj.dp[5] , 0 ,  0.00001  , test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(ball.obj.dp) , "[0 kg, 0 kg, 0 kg, 0 kg, 0 kg, 0 kg]", test_suite , "1D ptr, allocation" )

    ball.obj.dp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(ball.obj.dp) , "[30 kg, 31 kg, 32 kg, 33 kg]", test_suite , "1D ptr, list assign, no units" )

    ball.obj.dp[2] = 62
    TRICK_EXPECT_EQ( str(ball.obj.dp) , "[30 kg, 31 kg, 62 kg, 33 kg]", test_suite , "1D ptr, partial assign, no units" )

    TRICK_EXPECT_EQ( ball.obj.dp[-1], 33, test_suite , "negative index integer value" )
    TRICK_EXPECT_EQ( ball.obj.dp[-1.0], 33, test_suite , "negative index float value" )

    ball.obj.dp[-1] = 55
    ball.obj.dp[-2] = 54
    TRICK_EXPECT_EQ( str(ball.obj.dp) , "[30 kg, 31 kg, 54 kg, 55 kg]", test_suite , "negative index assignments" )

    ball.obj.dp = None
    TRICK_EXPECT_EQ( str(ball.obj.dp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    # Mixed tuple/list notation
    ball.obj.daa = trick.attach_units( "kg", (( 50 , 51 , 52) , [53, 54, 55]) )
    TRICK_EXPECT_EQ( str(ball.obj.daa) , "[[50 kg, 51 kg, 52 kg],[53 kg, 54 kg, 55 kg]]",
     test_suite , "2D array, full assign, no units" )

    ball.obj.daa = trick.attach_units( "kg", [[ 40 , 41 , 42] , [43, 44, 45]] )
    TRICK_EXPECT_EQ( str(ball.obj.daa) , "[[40 kg, 41 kg, 42 kg],[43 kg, 44 kg, 45 kg]]",
     test_suite , "2D array, full assign, no units" )

    ball.obj.daa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(ball.obj.daa) , "[[40 kg, 41 kg, 42 kg],[50 kg, 51 kg, 52 kg]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.daa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(ball.obj.daa) , "[[40 kg, 41 kg, 42 kg],[50.1 kg, 51.2 kg, 52.3 kg]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.daa[1][1] =  60
    TRICK_EXPECT_EQ( str(ball.obj.daa) , "[[40 kg, 41 kg, 42 kg],[50.1 kg, 60 kg, 52.3 kg]]",
     test_suite , "2D array, single assign, no units" )

    ball.obj.daa[0] = trick.attach_units( "lbm",[ 4.0, 5.0, 6.0])
    TRICK_EXPECT_EQ( str(ball.obj.daa[0]) , "[1.81436948 kg, 2.26796185 kg, 2.72155422 kg]",
     test_suite , "2D array, single single row assignment with units conversion" )

    TRICK_EXPECT_EQ( str(ball.obj.dap) , "[NULL, NULL, NULL, NULL]", test_suite , "2D array of ptr, initial value" )

    ball.obj.dap[0] = trick.TMM_declare_var_1d( "double", 3)
    ball.obj.dap[1] = trick.TMM_declare_var_1d( "double", 4)
    ball.obj.dap[2] = trick.TMM_declare_var_1d( "double", 5)
    ball.obj.dap[3] = trick.TMM_declare_var_1d( "double", 6)

    TRICK_EXPECT_EQ( str(ball.obj.dap[0]) , "[0 kg, 0 kg, 0 kg]", test_suite , "2D array of ptr, single row access" )

    ball.obj.dap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(ball.obj.dap[3]) , "[60 kg, 61 kg, 62 kg, 63 kg]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    ball.obj.dap[3][1] = 75
    ball.obj.dap[3][3] = trick.attach_units("lbm", float(ball.obj.dap[3][3]) + 1.0)

    TRICK_EXPECT_EQ( str(ball.obj.dap[3]) , "[60 kg, 75 kg, 62 kg, 29.02991168 kg]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    ball.obj.dpp = trick.TMM_declare_var_s("double *[4]")
    TRICK_EXPECT_EQ( str(ball.obj.dpp) , "[NULL, NULL, NULL, NULL]", test_suite , "2D ptr of ptr, initial value" )

    ball.obj.dpp[0] = trick.TMM_declare_var_1d( "double", 5)
    TRICK_EXPECT_EQ( str(ball.obj.dpp[0]) , "[0 kg, 0 kg, 0 kg, 0 kg, 0 kg]", test_suite , "2D ptr of ptr, allocate 1 row" )

    ball.obj.dpp[0][1] = 85
    TRICK_EXPECT_EQ( str(ball.obj.dpp[0]) , "[0 kg, 85 kg, 0 kg, 0 kg, 0 kg]",
     test_suite , "2D ptr of ptr, scalar assignment" )

    ball.obj.dpp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(ball.obj.dpp[1]) , "[91 kg, 92 kg, 93 kg]",
     test_suite , "2D ptr of ptr, row assignment" )

    ball.obj.dpp[2] = trick.attach_units("lbm" , [ 91 , 92 , 93 , 94 , 95])
    TRICK_EXPECT_NEAR( ball.obj.dpp[2][0] , 41.276905 , 0.000001 ,
     test_suite , "2D ptr of ptr, united value" )

    ball.obj.dpp = None
    TRICK_EXPECT_EQ( str(ball.obj.dpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

    ball.obj.daaa[0][0] = [1, 2, 3, 4]
    ball.obj.daaa[0][1] = [5, 6, 7, 8]
    ball.obj.daaa[0][2][0] = 9
    ball.obj.daaa[0][2][1] = 10
    ball.obj.daaa[0][2][2] = 11
    ball.obj.daaa[0][2][3] = 12
    # 2D assignment fails with error message but does not exit sim. :(
    #ball.obj.daaa[1][0] = [[101, 102, 103, 104] , [105, 106, 107, 108] , [109, 110, 111, 112]]
    TRICK_EXPECT_EQ( str(ball.obj.daaa[0]) , "[[1 kg, 2 kg, 3 kg, 4 kg],[5 kg, 6 kg, 7 kg, 8 kg],[9 kg, 10 kg, 11 kg, 12 kg]]",
     test_suite , "3D array, list and scalar assignment" )

    # 4D assignment array is not supported yet
    #ball.obj.daaaa[0][0][0] = [51, 52, 53, 54, 55]

######################################################################################################################

    test_suite = "float"

    ball.obj.f = 2
    TRICK_EXPECT_NEAR( ball.obj.f , 2.0 , 0.000001 , test_suite , "no units" )
    trick.trick_test_add_parent( test_suite , "no units" , "1532242077")

    ball.obj.f = trick.attach_units("lbm" , 2)
    TRICK_EXPECT_NEAR( ball.obj.f , 0.907185 , 0.000001 , test_suite , "units convert" )

    ball.obj.fa = [ 20 , 21 , 22 ]
    TRICK_EXPECT_NEAR( ball.obj.fa[0] , 20 , 0.000001 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_NEAR( ball.obj.fa[1] , 21 , 0.000001 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_NEAR( ball.obj.fa[2] , 22 , 0.000001 , test_suite , "1D array, integer value, no units" )

    ball.obj.fa = [ 30.1 , 31.1 , 32.1 ]

    TRICK_EXPECT_NEAR( ball.obj.fa[0] , 30.1 , 0.0001 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_NEAR( ball.obj.fa[1] , 31.1 , 0.0001 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_NEAR( ball.obj.fa[2] , 32.1 , 0.0001 , test_suite , "1D array, float value, no units" )

    ball.obj.fa = trick.attach_units("lbm" , [2 , 3 , 4])
    TRICK_EXPECT_NEAR( ball.obj.fa[0] , 0.907185 , 0.000001 , test_suite , "1D array, float value, units convert" )
    TRICK_EXPECT_NEAR( ball.obj.fa[1] , 1.36078 ,  0.00001  , test_suite , "1D array, float value, units convert" )
    TRICK_EXPECT_NEAR( ball.obj.fa[2] , 1.81437 ,  0.00001  , test_suite , "1D array, float value, units convert" )

    ball.obj.fp = trick.alloc_type( 6 , "float")
    TRICK_EXPECT_NEAR( ball.obj.fp[0] , 0 ,  0.00001  , test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_NEAR( ball.obj.fp[5] , 0 ,  0.00001  , test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(ball.obj.fp) , "[0 kg, 0 kg, 0 kg, 0 kg, 0 kg, 0 kg]", test_suite , "1D ptr, allocation" )

    ball.obj.fp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(ball.obj.fp) , "[30 kg, 31 kg, 32 kg, 33 kg]", test_suite , "1D ptr, list assign, no units" )

    ball.obj.fp[2] = 62
    TRICK_EXPECT_EQ( str(ball.obj.fp) , "[30 kg, 31 kg, 62 kg, 33 kg]", test_suite , "1D ptr, partial assign, no units" )

    ball.obj.fp = None
    TRICK_EXPECT_EQ( str(ball.obj.fp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    ball.obj.faa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(ball.obj.faa) , "[[40 kg, 41 kg, 42 kg],[43 kg, 44 kg, 45 kg]]",
     test_suite , "2D array, full assign, no units" )

    ball.obj.faa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(ball.obj.faa) , "[[40 kg, 41 kg, 42 kg],[50 kg, 51 kg, 52 kg]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.faa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(ball.obj.faa) , "[[40 kg, 41 kg, 42 kg],[50.099998 kg, 51.200001 kg, 52.299999 kg]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.faa[1][1] =  60
    TRICK_EXPECT_EQ( str(ball.obj.faa) , "[[40 kg, 41 kg, 42 kg],[50.099998 kg, 60 kg, 52.299999 kg]]",
     test_suite , "2D array, partial assign, no units" )

    TRICK_EXPECT_EQ( str(ball.obj.fap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    ball.obj.fap[0] = trick.alloc_type( 3 , "float")
    ball.obj.fap[1] = trick.alloc_type( 4 , "float")
    ball.obj.fap[2] = trick.alloc_type( 5 , "float")
    ball.obj.fap[3] = trick.alloc_type( 6 , "float")
    TRICK_EXPECT_EQ( str(ball.obj.fap[0]) , "[0 kg, 0 kg, 0 kg]",
     test_suite , "2D array of ptr, single row access" )

    ball.obj.fap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(ball.obj.fap[3]) , "[60 kg, 61 kg, 62 kg, 63 kg]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    ball.obj.fap[3][1] = 75
    ball.obj.fap[3][3] = trick.attach_units("lbm", float(ball.obj.fap[3][3]) + 1.0)

    TRICK_EXPECT_EQ( str(ball.obj.fap[3]) , "[60 kg, 75 kg, 62 kg, 29.029911 kg]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    ball.obj.fpp = trick.alloc_type( 4 , "float *")
    TRICK_EXPECT_EQ( str(ball.obj.fpp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    ball.obj.fpp[0] = trick.alloc_type( 5 , "float")
    TRICK_EXPECT_EQ( str(ball.obj.fpp[0]) , "[0 kg, 0 kg, 0 kg, 0 kg, 0 kg]", test_suite , "2D ptr of ptr, allocate 1 row" )

    ball.obj.fpp[0][1] = 85
    TRICK_EXPECT_EQ( str(ball.obj.fpp[0]) , "[0 kg, 85 kg, 0 kg, 0 kg, 0 kg]", test_suite , "2D ptr of ptr, assign 1 value" )

    ball.obj.fpp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(ball.obj.fpp[1]) , "[91 kg, 92 kg, 93 kg]", test_suite , "2D ptr of ptr, row assignment" )

    ball.obj.fpp = None
    TRICK_EXPECT_EQ( str(ball.obj.fpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

    ball.obj.f_rad = 2.0
    TRICK_EXPECT_NEAR( ball.obj.f_rad , 2.0 , 0.000001 , test_suite , "no units" )

    ball.obj.f_rad = trick.attach_units("d" , 45.0)
    TRICK_EXPECT_NEAR( ball.obj.f_rad , 0.785398 , 0.000001 , test_suite , "unit conv" )

    ball.obj.d_deg = ball.obj.f_rad
    TRICK_EXPECT_NEAR( ball.obj.d_deg , 45.0 , 0.00001 , test_suite , "value to value assign with conversion" )

######################################################################################################################

    test_suite = "char"

    ball.obj.c = 'g'
    TRICK_EXPECT_EQ( str(ball.obj.c) , "103", test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "2896569040")

    ball.obj.c = 123
    TRICK_EXPECT_EQ( str(ball.obj.c) , "123", test_suite , "assignment" )

    ball.obj.ca = "Trick is great"
    TRICK_EXPECT_EQ( str(ball.obj.ca) , "Trick is great", test_suite , "arrray assignment" )

    ball.obj.ca = [65, 66, 67, 68, 69]
    TRICK_EXPECT_EQ( str(ball.obj.ca) , "ABCDE", test_suite , "arrray assignment" )

    ball.obj.ca = [65.1, 66.2, 67.3, 68.4, 69.6]
    TRICK_EXPECT_EQ( str(ball.obj.ca) , "ABCDE", test_suite , "arrray assignment" )
    TRICK_EXPECT_EQ( ball.obj.ca[3] , 68 , test_suite , "arrray assignment" )

    ball.obj.cp = trick.alloc_type( 6 , "char")
    TRICK_EXPECT_EQ( ball.obj.cp[0] , 0 , test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( ball.obj.cp[5] , 0 , test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(ball.obj.cp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    ball.obj.cp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(ball.obj.cp) , "[30, 31, ' ', '!']", test_suite , "float 1D ptr, list assign, no units" )

    ball.obj.cp[2] = 62
    TRICK_EXPECT_EQ( str(ball.obj.cp) , "[30, 31, '>', '!']", test_suite , "float 1D ptr, partial assign, no units" )


    ball.obj.cp = "testing"
    TRICK_EXPECT_EQ( str(ball.obj.cp) , "testing", test_suite , "ptr assignment" )

    ball.obj.cp = None
    TRICK_EXPECT_EQ( str(ball.obj.cp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    ball.obj.caa = [ "abcdefg" , "hijklmn" ]
    TRICK_EXPECT_EQ( str(ball.obj.caa) , "[\"abcdefg\",\"hijklmn\",[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]]", test_suite , "2D array string assignment" )
    TRICK_EXPECT_EQ( str(ball.obj.caa[1]) , "hijklmn", test_suite , "2D array item access" )
    TRICK_EXPECT_EQ( ball.obj.caa[1][4] , 108, test_suite , "2D array single char access" )

    TRICK_EXPECT_EQ( str(ball.obj.cap) , """[NULL, NULL, NULL, NULL]""", test_suite , "2D array of ptr initial value" )

    ball.obj.cap[0] = "cap0"
    ball.obj.cap[1] = "cap1"
    ball.obj.cap[2] = "cap2"
    ball.obj.cap[3] = "cap3"
    TRICK_EXPECT_EQ( str(ball.obj.cap) , "[\"cap0\", \"cap1\", \"cap2\", \"cap3\"]", test_suite , "2D array of ptr single item assignment" )
    TRICK_EXPECT_EQ( str(ball.obj.cap[0]) , "cap0", test_suite , "2D array of ptr single item assignment" )

    TRICK_EXPECT_EQ( ball.obj.cap[3][2] , 112 , test_suite , "2D array of ptr single item assignment" )

    ball.obj.cpp = trick.alloc_type( 4 , "char *")
    TRICK_EXPECT_EQ( str(ball.obj.cpp) , """[NULL, NULL, NULL, NULL]""", test_suite , "2D ptr of ptr initial value" )

    ball.obj.cpp[0] = "cpp_string_0"
    ball.obj.cpp[1] = "cpp_string_1"
    ball.obj.cpp[2] = "cpp_string_2"
    ball.obj.cpp[3] = "cpp_string_3"
    TRICK_EXPECT_EQ( str(ball.obj.cpp[2]) , "cpp_string_2", test_suite , "2D ptr of ptr single string access" )
    TRICK_EXPECT_EQ( ball.obj.cpp[2][3] , 95 , test_suite , "2D ptr of ptr single character access" )

    ball.obj.cpp = None
    TRICK_EXPECT_EQ( str(ball.obj.cpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "unsigned char"

    ball.obj.uc = 95
    TRICK_EXPECT_EQ( ball.obj.uc , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "219444977")

    ball.obj.uc += 1
    TRICK_EXPECT_EQ( ball.obj.uc , 96 , test_suite , "increment" )

    ball.obj.uc = ball.obj.uc + 1
    TRICK_EXPECT_EQ( ball.obj.uc , 97 , test_suite , "increment" )

    ball.obj.uc = 1 + ball.obj.uc
    TRICK_EXPECT_EQ( ball.obj.uc , 98 , test_suite , "increment" )

    ball.obj.uca = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( ball.obj.uca[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.uca[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.uca[2] , 22 , test_suite , "1D array, integer value, no units" )

    ball.obj.uca = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( ball.obj.uca[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.uca[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.uca[2] , 32 , test_suite , "1D array, float value, no units" )

    ball.obj.ucp = trick.alloc_type( 6 , "unsigned char")
    TRICK_EXPECT_EQ( ball.obj.ucp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( ball.obj.ucp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(ball.obj.ucp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    ball.obj.ucp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(ball.obj.ucp) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    ball.obj.ucp[2] = 62
    TRICK_EXPECT_EQ( str(ball.obj.ucp) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    ball.obj.ucp = None
    TRICK_EXPECT_EQ( str(ball.obj.ucp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    ball.obj.ucaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(ball.obj.ucaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    ball.obj.ucaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(ball.obj.ucaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.ucaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(ball.obj.ucaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.ucaa[1][1] =  60
    TRICK_EXPECT_EQ( str(ball.obj.ucaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(ball.obj.ucap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    ball.obj.ucap[0] = trick.alloc_type( 3 , "unsigned char")
    ball.obj.ucap[1] = trick.alloc_type( 4 , "unsigned char")
    ball.obj.ucap[2] = trick.alloc_type( 5 , "unsigned char")
    ball.obj.ucap[3] = trick.alloc_type( 6 , "unsigned char")

    TRICK_EXPECT_EQ( str(ball.obj.ucap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    ball.obj.ucap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(ball.obj.ucap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    ball.obj.ucap[3][1] = 75
    ball.obj.ucap[3][3] = trick.attach_units("--", int(ball.obj.ucap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(ball.obj.ucap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    ball.obj.ucpp = trick.alloc_type( 4 , "unsigned char *")
    TRICK_EXPECT_EQ( str(ball.obj.ucpp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    ball.obj.ucpp[0] = trick.alloc_type( 5 , "unsigned char")
    TRICK_EXPECT_EQ( str(ball.obj.ucpp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    ball.obj.ucpp[0][1] = 85
    TRICK_EXPECT_EQ( str(ball.obj.ucpp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    ball.obj.ucpp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(ball.obj.ucpp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    ball.obj.ucpp = None
    TRICK_EXPECT_EQ( str(ball.obj.ucpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "short"

    ball.obj.s = 95
    TRICK_EXPECT_EQ( ball.obj.s , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "2880907803")

    ball.obj.s += 1
    TRICK_EXPECT_EQ( ball.obj.s , 96 , test_suite , "increment" )

    ball.obj.s = ball.obj.s + 1
    TRICK_EXPECT_EQ( ball.obj.s , 97 , test_suite , "increment" )

    ball.obj.s = 1 + ball.obj.s
    TRICK_EXPECT_EQ( ball.obj.s , 98 , test_suite , "increment" )

    ball.obj.sa = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( ball.obj.sa[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.sa[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.sa[2] , 22 , test_suite , "1D array, integer value, no units" )

    ball.obj.sa = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( ball.obj.sa[0] , 30 , test_suite , "short 1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.sa[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.sa[2] , 32 , test_suite , "1D array, float value, no units" )

    ball.obj.sp = trick.alloc_type( 6 , "short")
    TRICK_EXPECT_EQ( ball.obj.sp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( ball.obj.sp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(ball.obj.sp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    ball.obj.sp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(ball.obj.sp) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    ball.obj.sp[2] = 62
    TRICK_EXPECT_EQ( str(ball.obj.sp) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    ball.obj.sp = None
    TRICK_EXPECT_EQ( str(ball.obj.sp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    ball.obj.saa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(ball.obj.saa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    ball.obj.saa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(ball.obj.saa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.saa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(ball.obj.saa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.saa[1][1] =  60
    TRICK_EXPECT_EQ( str(ball.obj.saa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(ball.obj.sap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    ball.obj.sap[0] = trick.alloc_type( 3 , "short")
    ball.obj.sap[1] = trick.alloc_type( 4 , "short")
    ball.obj.sap[2] = trick.alloc_type( 5 , "short")
    ball.obj.sap[3] = trick.alloc_type( 6 , "short")

    TRICK_EXPECT_EQ( str(ball.obj.sap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    ball.obj.sap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(ball.obj.sap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    ball.obj.sap[3][1] = 75
    ball.obj.sap[3][3] = trick.attach_units("--", int(ball.obj.sap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(ball.obj.sap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    ball.obj.spp = trick.alloc_type( 4 , "short *")
    TRICK_EXPECT_EQ( str(ball.obj.spp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    ball.obj.spp[0] = trick.alloc_type( 5 , "short")
    TRICK_EXPECT_EQ( str(ball.obj.spp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    ball.obj.spp[0][1] = 85
    TRICK_EXPECT_EQ( str(ball.obj.spp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    ball.obj.spp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(ball.obj.spp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    ball.obj.spp = None
    TRICK_EXPECT_EQ( str(ball.obj.spp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "unsigned short"

    ball.obj.us = 95
    TRICK_EXPECT_EQ( ball.obj.us , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "217750348")

    ball.obj.us += 1
    TRICK_EXPECT_EQ( ball.obj.us , 96 , test_suite , "increment" )

    ball.obj.us = ball.obj.us + 1
    TRICK_EXPECT_EQ( ball.obj.us , 97 , test_suite , "increment" )

    ball.obj.us = 1 + ball.obj.us
    TRICK_EXPECT_EQ( ball.obj.us , 98 , test_suite , "increment" )

    ball.obj.usa = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( ball.obj.usa[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.usa[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.usa[2] , 22 , test_suite , "1D array, integer value, no units" )

    ball.obj.usa = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( ball.obj.usa[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.usa[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.usa[2] , 32 , test_suite , "1D array, float value, no units" )

    ball.obj.usp = trick.alloc_type( 6 , "unsigned short")
    TRICK_EXPECT_EQ( ball.obj.usp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( ball.obj.usp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(ball.obj.usp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    ball.obj.usp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(ball.obj.usp) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    ball.obj.usp[2] = 62
    TRICK_EXPECT_EQ( str(ball.obj.usp) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    ball.obj.usp = None
    TRICK_EXPECT_EQ( str(ball.obj.usp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    ball.obj.usaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(ball.obj.usaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    ball.obj.usaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(ball.obj.usaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.usaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(ball.obj.usaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.usaa[1][1] =  60
    TRICK_EXPECT_EQ( str(ball.obj.usaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(ball.obj.usap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    ball.obj.usap[0] = trick.alloc_type( 3 , "unsigned short")
    ball.obj.usap[1] = trick.alloc_type( 4 , "unsigned short")
    ball.obj.usap[2] = trick.alloc_type( 5 , "unsigned short")
    ball.obj.usap[3] = trick.alloc_type( 6 , "unsigned short")

    TRICK_EXPECT_EQ( str(ball.obj.usap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    ball.obj.usap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(ball.obj.usap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    ball.obj.usap[3][1] = 75
    ball.obj.usap[3][3] = trick.attach_units("--", int(ball.obj.usap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(ball.obj.usap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    ball.obj.uspp = trick.alloc_type( 4 , "unsigned short *")
    TRICK_EXPECT_EQ( str(ball.obj.uspp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    ball.obj.uspp[0] = trick.alloc_type( 5 , "unsigned short")
    TRICK_EXPECT_EQ( str(ball.obj.uspp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    ball.obj.uspp[0][1] = 85
    TRICK_EXPECT_EQ( str(ball.obj.uspp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    ball.obj.uspp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(ball.obj.uspp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    ball.obj.uspp = None
    TRICK_EXPECT_EQ( str(ball.obj.uspp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "enum"

    ball.obj.e = trick.THIRD
    TRICK_EXPECT_EQ( ball.obj.e , trick.THIRD , test_suite , "scalar, integer value, no units" )
    trick.trick_test_add_parent( test_suite , "scalar, integer value, no units" , "3331066868")

    ball.obj.ea = [ trick.THIRD , trick.SECOND , trick.FIRST ]
    TRICK_EXPECT_EQ( ball.obj.ea[0] , trick.THIRD , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ea[1] , trick.SECOND , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ea[2] , trick.FIRST , test_suite , "1D array, integer value, no units" )

    ball.obj.ea[1] = trick.THIRD
    ball.obj.ea[2] = trick.SECOND
    TRICK_EXPECT_EQ( ball.obj.ea[1] , trick.THIRD , test_suite , "1D array single value assignment" )
    TRICK_EXPECT_EQ( ball.obj.ea[2] , trick.SECOND , test_suite , "1D array single value assignment" )

    ball.obj.ep = trick.alloc_type( 6 , "MY_ENUM")
    TRICK_EXPECT_EQ( ball.obj.ep[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( ball.obj.ep[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(ball.obj.ep) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    ball.obj.ep = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(ball.obj.ep) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    ball.obj.ep[2] = 62
    TRICK_EXPECT_EQ( str(ball.obj.ep) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    ball.obj.ep = None
    TRICK_EXPECT_EQ( str(ball.obj.ep) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    ball.obj.eaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(ball.obj.eaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    ball.obj.eaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(ball.obj.eaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.eaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(ball.obj.eaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.eaa[1][1] =  60
    TRICK_EXPECT_EQ( str(ball.obj.eaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(ball.obj.eap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    ball.obj.eap[0] = trick.alloc_type( 3 , "MY_ENUM")
    ball.obj.eap[1] = trick.alloc_type( 4 , "MY_ENUM")
    ball.obj.eap[2] = trick.alloc_type( 5 , "MY_ENUM")
    ball.obj.eap[3] = trick.alloc_type( 6 , "MY_ENUM")

    TRICK_EXPECT_EQ( str(ball.obj.eap[0]) , "[0, 0, 0]", test_suite , "2D array of ptr, single row access" )

    ball.obj.eap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(ball.obj.eap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    ball.obj.eap[3][1] = 75
    ball.obj.eap[3][3] = trick.attach_units("--", int(ball.obj.eap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(ball.obj.eap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    ball.obj.epp = trick.alloc_type( 4 , "MY_ENUM *")
    TRICK_EXPECT_EQ( str(ball.obj.epp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "int 2D ptr of ptr, initial value" )

    ball.obj.epp[0] = trick.alloc_type( 5 , "MY_ENUM")
    TRICK_EXPECT_EQ( str(ball.obj.epp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    ball.obj.epp[0][1] = 85
    TRICK_EXPECT_EQ( str(ball.obj.epp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    ball.obj.epp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(ball.obj.epp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    ball.obj.epp = None
    TRICK_EXPECT_EQ( str(ball.obj.epp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "int"

    ball.obj.i = 95
    TRICK_EXPECT_EQ( ball.obj.i , 95 , test_suite , "assignment" )
    #print "ball.obj.i = " , ball.obj.i
    trick.trick_test_add_parent( test_suite , "assignment" , "3074788233")

    ball.obj.i += 1
    TRICK_EXPECT_EQ( ball.obj.i , 96 , test_suite , "increment" )

    ball.obj.i = ball.obj.i + 1
    TRICK_EXPECT_EQ( ball.obj.i , 97 , test_suite , "increment" )

    ball.obj.i = 1 + ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.i , 98 , test_suite , "increment" )

    ball.obj.ia = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( ball.obj.ia[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ia[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ia[2] , 22 , test_suite , "1D array, integer value, no units" )

    ball.obj.ia = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( ball.obj.ia[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ia[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ia[2] , 32 , test_suite , "1D array, float value, no units" )

    ball.obj.ia = trick.attach_units("--" , [60, 70])
    TRICK_EXPECT_EQ( ball.obj.ia[0] , 60 , test_suite , "1D array, -- units" )
    TRICK_EXPECT_EQ( ball.obj.ia[1] , 70 , test_suite , "1D array, -- units" )

    ball.obj.ip = trick.alloc_type( 6 , "int")
    TRICK_EXPECT_EQ( ball.obj.ip[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( ball.obj.ip[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(ball.obj.ip) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    ball.obj.ip = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(ball.obj.ip) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    ball.obj.ip[2] = 62
    TRICK_EXPECT_EQ( str(ball.obj.ip) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    ball.obj.ip = trick.attach_units("--" , [60, 70])
    TRICK_EXPECT_EQ( str(ball.obj.ip) , "[60, 70]", test_suite , "1D ptr, assign list -- unit-ed values" )

    ball.obj.ip = None
    TRICK_EXPECT_EQ( str(ball.obj.ip) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    ball.obj.iaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(ball.obj.iaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    ball.obj.iaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(ball.obj.iaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.iaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(ball.obj.iaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.iaa[1][1] =  60
    TRICK_EXPECT_EQ( str(ball.obj.iaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(ball.obj.iap) , "[NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    ball.obj.iap[0] = trick.alloc_type( 3 , "int")
    ball.obj.iap[1] = trick.alloc_type( 4 , "int")
    ball.obj.iap[2] = trick.alloc_type( 5 , "int")
    ball.obj.iap[3] = trick.alloc_type( 6 , "int")
    TRICK_EXPECT_EQ( str(ball.obj.iap[0]) , "[0, 0, 0]", test_suite , "2D array of ptr, single row access" )

    ball.obj.iap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(ball.obj.iap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    ball.obj.iap[3][1] = 75
    ball.obj.iap[3][3] = trick.attach_units("--", int(ball.obj.iap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(ball.obj.iap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    ball.obj.ipp = trick.alloc_type( 4 , "int *")
    TRICK_EXPECT_EQ( str(ball.obj.ipp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    ball.obj.ipp[0] = trick.alloc_type( 5 , "int")
    TRICK_EXPECT_EQ( str(ball.obj.ipp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    ball.obj.ipp[0][1] = 85
    TRICK_EXPECT_EQ( str(ball.obj.ipp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    ball.obj.ipp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(ball.obj.ipp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    ball.obj.ipp = None
    TRICK_EXPECT_EQ( str(ball.obj.ipp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "unsigned int"

    ball.obj.ui = 95
    TRICK_EXPECT_EQ( ball.obj.ui , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "1873736978")

    ball.obj.ui += 1
    TRICK_EXPECT_EQ( ball.obj.ui , 96 , test_suite , "increment" )

    ball.obj.ui = ball.obj.ui + 1
    TRICK_EXPECT_EQ( ball.obj.ui , 97 , test_suite , "increment" )

    ball.obj.ui = 1 + ball.obj.ui
    TRICK_EXPECT_EQ( ball.obj.ui , 98 , test_suite , "increment" )

    ball.obj.uia = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( ball.obj.uia[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.uia[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.uia[2] , 22 , test_suite , "1D array, integer value, no units" )

    ball.obj.uia = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( ball.obj.uia[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.uia[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.uia[2] , 32 , test_suite , "1D array, float value, no units" )

    ball.obj.uip = trick.alloc_type( 6 , "unsigned int")
    TRICK_EXPECT_EQ( ball.obj.uip[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( ball.obj.uip[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(ball.obj.uip) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    ball.obj.uip = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(ball.obj.uip) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    ball.obj.uip[2] = 62
    TRICK_EXPECT_EQ( str(ball.obj.uip) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    ball.obj.uip = None
    TRICK_EXPECT_EQ( str(ball.obj.uip) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    ball.obj.uiaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(ball.obj.uiaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    ball.obj.uiaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(ball.obj.uiaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.uiaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(ball.obj.uiaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.uiaa[1][1] =  60
    TRICK_EXPECT_EQ( str(ball.obj.uiaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(ball.obj.uiap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    ball.obj.uiap[0] = trick.alloc_type( 3 , "unsigned int")
    ball.obj.uiap[1] = trick.alloc_type( 4 , "unsigned int")
    ball.obj.uiap[2] = trick.alloc_type( 5 , "unsigned int")
    ball.obj.uiap[3] = trick.alloc_type( 6 , "unsigned int")

    TRICK_EXPECT_EQ( str(ball.obj.uiap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    ball.obj.uiap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(ball.obj.uiap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    ball.obj.uiap[3][1] = 75
    ball.obj.uiap[3][3] = trick.attach_units("--", int(ball.obj.uiap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(ball.obj.uiap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    ball.obj.uipp = trick.alloc_type( 4 , "unsigned int *")
    TRICK_EXPECT_EQ( str(ball.obj.uipp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    ball.obj.uipp[0] = trick.alloc_type( 5 , "unsigned int")
    TRICK_EXPECT_EQ( str(ball.obj.uipp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    ball.obj.uipp[0][1] = 85
    TRICK_EXPECT_EQ( str(ball.obj.uipp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    ball.obj.uipp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(ball.obj.uipp[1]) , "[91, 92, 93]", test_suite , "unsigned int 2D ptr of ptr, row assignment" )

    ball.obj.uipp = None
    TRICK_EXPECT_EQ( str(ball.obj.uipp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "long"

    ball.obj.l = 95
    TRICK_EXPECT_EQ( ball.obj.l , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "3338288463")

    ball.obj.l += 1
    TRICK_EXPECT_EQ( ball.obj.l , 96 , test_suite , "increment" )

    ball.obj.l = ball.obj.l + 1
    TRICK_EXPECT_EQ( ball.obj.l , 97 , test_suite , "increment" )

    ball.obj.l = 1 + ball.obj.l
    TRICK_EXPECT_EQ( ball.obj.l , 98 , test_suite , "increment" )

    ball.obj.la = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( ball.obj.la[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.la[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.la[2] , 22 , test_suite , "1D array, integer value, no units" )

    ball.obj.la = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( ball.obj.la[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.la[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.la[2] , 32 , test_suite , "1D array, float value, no units" )

    ball.obj.lp = trick.alloc_type( 6 , "long")
    TRICK_EXPECT_EQ( ball.obj.lp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( ball.obj.lp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(ball.obj.lp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    ball.obj.lp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(ball.obj.lp) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    ball.obj.lp[2] = 62
    TRICK_EXPECT_EQ( str(ball.obj.lp) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    ball.obj.lp = None
    TRICK_EXPECT_EQ( str(ball.obj.lp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    ball.obj.laa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(ball.obj.laa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    ball.obj.laa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(ball.obj.laa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.laa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(ball.obj.laa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.laa[1][1] =  60
    TRICK_EXPECT_EQ( str(ball.obj.laa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(ball.obj.lap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    ball.obj.lap[0] = trick.alloc_type( 3 , "long")
    ball.obj.lap[1] = trick.alloc_type( 4 , "long")
    ball.obj.lap[2] = trick.alloc_type( 5 , "long")
    ball.obj.lap[3] = trick.alloc_type( 6 , "long")

    TRICK_EXPECT_EQ( str(ball.obj.lap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    ball.obj.lap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(ball.obj.lap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    ball.obj.lap[3][1] = 75
    ball.obj.lap[3][3] = trick.attach_units("--", int(ball.obj.lap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(ball.obj.lap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    ball.obj.lpp = trick.alloc_type( 4 , "long *")
    TRICK_EXPECT_EQ( str(ball.obj.lpp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    ball.obj.lpp[0] = trick.alloc_type( 5 , "long")
    TRICK_EXPECT_EQ( str(ball.obj.lpp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    ball.obj.lpp[0][1] = 85
    TRICK_EXPECT_EQ( str(ball.obj.lpp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    ball.obj.lpp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(ball.obj.lpp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    ball.obj.lpp = None
    TRICK_EXPECT_EQ( str(ball.obj.lpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "unsinged long"

    ball.obj.ul = 95
    TRICK_EXPECT_EQ( ball.obj.ul , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "2844151852")

    ball.obj.ul += 1
    TRICK_EXPECT_EQ( ball.obj.ul , 96 , test_suite , "increment" )

    ball.obj.ul = ball.obj.ul + 1
    TRICK_EXPECT_EQ( ball.obj.ul , 97 , test_suite , "increment" )

    ball.obj.ul = 1 + ball.obj.ul
    TRICK_EXPECT_EQ( ball.obj.ul , 98 , test_suite , "increment" )

    ball.obj.ula = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( ball.obj.ula[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ula[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ula[2] , 22 , test_suite , "1D array, integer value, no units" )

    ball.obj.ula = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( ball.obj.ula[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ula[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ula[2] , 32 , test_suite , "1D array, float value, no units" )

    ball.obj.ulp = trick.alloc_type( 6 , "unsigned long")
    TRICK_EXPECT_EQ( ball.obj.ulp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( ball.obj.ulp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(ball.obj.ulp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    ball.obj.ulp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(ball.obj.ulp) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    ball.obj.ulp[2] = 62
    TRICK_EXPECT_EQ( str(ball.obj.ulp) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    ball.obj.ulp = None
    TRICK_EXPECT_EQ( str(ball.obj.ulp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    ball.obj.ulaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(ball.obj.ulaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    ball.obj.ulaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(ball.obj.ulaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.ulaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(ball.obj.ulaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.ulaa[1][1] =  60
    TRICK_EXPECT_EQ( str(ball.obj.ulaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(ball.obj.ulap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    ball.obj.ulap[0] = trick.alloc_type( 3 , "unsigned long")
    ball.obj.ulap[1] = trick.alloc_type( 4 , "unsigned long")
    ball.obj.ulap[2] = trick.alloc_type( 5 , "unsigned long")
    ball.obj.ulap[3] = trick.alloc_type( 6 , "unsigned long")

    TRICK_EXPECT_EQ( str(ball.obj.ulap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    ball.obj.ulap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(ball.obj.ulap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    ball.obj.ulap[3][1] = 75
    ball.obj.ulap[3][3] = trick.attach_units("--", int(ball.obj.ulap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(ball.obj.ulap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    ball.obj.ulpp = trick.alloc_type( 4 , "unsigned long *")
    TRICK_EXPECT_EQ( str(ball.obj.ulpp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    ball.obj.ulpp[0] = trick.alloc_type( 5 , "unsigned long")
    TRICK_EXPECT_EQ( str(ball.obj.ulpp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    ball.obj.ulpp[0][1] = 85
    TRICK_EXPECT_EQ( str(ball.obj.ulpp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    ball.obj.ulpp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(ball.obj.ulpp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    ball.obj.ulpp = None
    TRICK_EXPECT_EQ( str(ball.obj.ulpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "long long"

    ball.obj.ll = 95
    TRICK_EXPECT_EQ( ball.obj.ll , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "2165977787")

    ball.obj.ll += 1
    TRICK_EXPECT_EQ( ball.obj.ll , 96 , test_suite , "increment" )

    ball.obj.ll = ball.obj.ll + 1
    TRICK_EXPECT_EQ( ball.obj.ll , 97 , test_suite , "increment" )

    ball.obj.ll = 1 + ball.obj.ll
    TRICK_EXPECT_EQ( ball.obj.ll , 98 , test_suite , "increment" )

    ball.obj.lla = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( ball.obj.lla[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.lla[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.lla[2] , 22 , test_suite , "1D array, integer value, no units" )

    ball.obj.lla = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( ball.obj.lla[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.lla[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.lla[2] , 32 , test_suite , "1D array, float value, no units" )

    ball.obj.llp = trick.alloc_type( 6 , "long long")
    TRICK_EXPECT_EQ( ball.obj.llp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( ball.obj.llp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(ball.obj.llp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    ball.obj.llp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(ball.obj.llp) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    ball.obj.llp[2] = 62
    TRICK_EXPECT_EQ( str(ball.obj.llp) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    ball.obj.llp = None
    TRICK_EXPECT_EQ( str(ball.obj.llp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    ball.obj.llaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(ball.obj.llaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    ball.obj.llaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(ball.obj.llaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.llaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(ball.obj.llaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.llaa[1][1] =  60
    TRICK_EXPECT_EQ( str(ball.obj.llaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(ball.obj.llap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    ball.obj.llap[0] = trick.alloc_type( 3 , "long long")
    ball.obj.llap[1] = trick.alloc_type( 4 , "long long")
    ball.obj.llap[2] = trick.alloc_type( 5 , "long long")
    ball.obj.llap[3] = trick.alloc_type( 6 , "long long")

    TRICK_EXPECT_EQ( str(ball.obj.llap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    ball.obj.llap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(ball.obj.llap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    ball.obj.llap[3][1] = 75
    ball.obj.llap[3][3] = trick.attach_units("--", int(ball.obj.llap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(ball.obj.llap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    ball.obj.llpp = trick.alloc_type( 4 , "long long *")
    TRICK_EXPECT_EQ( str(ball.obj.llpp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    ball.obj.llpp[0] = trick.alloc_type( 5 , "long long")
    TRICK_EXPECT_EQ( str(ball.obj.llpp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    ball.obj.llpp[0][1] = 85
    TRICK_EXPECT_EQ( str(ball.obj.llpp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    ball.obj.llpp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(ball.obj.llpp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    ball.obj.llpp = None
    TRICK_EXPECT_EQ( str(ball.obj.llpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "unsigned long long"

    ball.obj.ull = 95
    TRICK_EXPECT_EQ( ball.obj.ull , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "3783821020")	

    ball.obj.ull += 1
    TRICK_EXPECT_EQ( ball.obj.ull , 96 , test_suite , "increment" )

    ball.obj.ull = ball.obj.ull + 1
    TRICK_EXPECT_EQ( ball.obj.ull , 97 , test_suite , "increment" )

    ball.obj.ull = 1 + ball.obj.ull
    TRICK_EXPECT_EQ( ball.obj.ull , 98 , test_suite , "increment" )

    ball.obj.ulla = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( ball.obj.ulla[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ulla[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ulla[2] , 22 , test_suite , "1D array, integer value, no units" )

    ball.obj.ulla = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( ball.obj.ulla[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ulla[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ulla[2] , 32 , test_suite , "1D array, float value, no units" )

    ball.obj.ullp = trick.alloc_type( 6 , "unsigned long long")
    TRICK_EXPECT_EQ( ball.obj.ullp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( ball.obj.ullp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(ball.obj.ullp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    ball.obj.ullp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(ball.obj.ullp) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    ball.obj.ullp[2] = 62
    TRICK_EXPECT_EQ( str(ball.obj.ullp) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    ball.obj.ullp = None
    TRICK_EXPECT_EQ( str(ball.obj.ullp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    ball.obj.ullaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(ball.obj.ullaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    ball.obj.ullaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(ball.obj.ullaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.ullaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(ball.obj.ullaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.ullaa[1][1] =  60
    TRICK_EXPECT_EQ( str(ball.obj.ullaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(ball.obj.ullap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    ball.obj.ullap[0] = trick.alloc_type( 3 , "unsigned long long")
    ball.obj.ullap[1] = trick.alloc_type( 4 , "unsigned long long")
    ball.obj.ullap[2] = trick.alloc_type( 5 , "unsigned long long")
    ball.obj.ullap[3] = trick.alloc_type( 6 , "unsigned long long")

    TRICK_EXPECT_EQ( str(ball.obj.ullap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    ball.obj.ullap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(ball.obj.ullap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    ball.obj.ullap[3][1] = 75
    ball.obj.ullap[3][3] = trick.attach_units("--", int(ball.obj.ullap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(ball.obj.ullap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    ball.obj.ullpp = trick.alloc_type( 4 , "unsigned long long *")
    TRICK_EXPECT_EQ( str(ball.obj.ullpp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    ball.obj.ullpp[0] = trick.alloc_type( 5 , "unsigned long long")
    TRICK_EXPECT_EQ( str(ball.obj.ullpp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    ball.obj.ullpp[0][1] = 85
    TRICK_EXPECT_EQ( str(ball.obj.ullpp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    ball.obj.ullpp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(ball.obj.ullpp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    ball.obj.ullpp = None
    TRICK_EXPECT_EQ( str(ball.obj.ullpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "bool"

    ball.obj.b = True
    TRICK_EXPECT_EQ( ball.obj.b , True , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "4134211307")

    ball.obj.b += 1
    TRICK_EXPECT_EQ( ball.obj.b , True , test_suite , "increment" )

    ball.obj.b = ball.obj.b + 1
    TRICK_EXPECT_EQ( ball.obj.b , True , test_suite , "increment" )

    ball.obj.ull = 1 + ball.obj.ull
    TRICK_EXPECT_EQ( ball.obj.b , True , test_suite , "increment" )

    ball.obj.ba = [ False , True , True ]
    TRICK_EXPECT_EQ( ball.obj.ba[0] , False , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ba[1] , True , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ba[2] , True , test_suite , "1D array, integer value, no units" )

    ball.obj.ba = [ 2.2 , 1.1 , 0 ]
    TRICK_EXPECT_EQ( ball.obj.ba[0] , True , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ba[1] , True , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( ball.obj.ba[2] , False , test_suite , "1D array, float value, no units" )

    ball.obj.bp = trick.alloc_type( 6 , "bool" )
    TRICK_EXPECT_EQ( ball.obj.bp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( ball.obj.bp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(ball.obj.bp) , "[False, False, False, False, False, False]", test_suite , "1D ptr, allocation" )

    ball.obj.bp = [ 0 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(ball.obj.bp) , "[False, True, True, True]", test_suite , "1D ptr, list assign, no units" )

    ball.obj.bp[2] = 0
    TRICK_EXPECT_EQ( str(ball.obj.bp) , "[False, True, False, True]", test_suite , "1D ptr, partial assign, no units" )

    ball.obj.bp = None
    TRICK_EXPECT_EQ( str(ball.obj.bp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    ball.obj.baa = [[ 0 , 1 , 2] , [3, 4, 0]]
    TRICK_EXPECT_EQ( str(ball.obj.baa) , "[[False, True, True],[True, True, False]]",
     test_suite , "2D array, full assign, no units" )

    ball.obj.baa[1] = [ False , False , True]
    TRICK_EXPECT_EQ( str(ball.obj.baa) , "[[False, True, True],[False, False, True]]",
     test_suite , "2D array, partial assign, no units" )

    ball.obj.baa[1][1] = True
    TRICK_EXPECT_EQ( str(ball.obj.baa) , "[[False, True, True],[False, True, True]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(ball.obj.bap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    ball.obj.bap[0] = trick.alloc_type( 3 , "bool")
    ball.obj.bap[1] = trick.alloc_type( 4 , "bool")
    ball.obj.bap[2] = trick.alloc_type( 5 , "bool")
    ball.obj.bap[3] = trick.alloc_type( 6 , "bool")

    TRICK_EXPECT_EQ( str(ball.obj.bap[0]) , "[False, False, False]",
     test_suite , "2D array of ptr, single row access" )

    ball.obj.bap[3] = [ True , False , True, False ]
    TRICK_EXPECT_EQ( str(ball.obj.bap[3]) , "[True, False, True, False]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    ball.obj.bap[3][1] = 75
    ball.obj.bap[3][3] = trick.attach_units("--", int(ball.obj.bap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(ball.obj.bap[3]) , "[True, True, True, True]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    ball.obj.bpp = trick.alloc_type( 4 , "bool *")
    TRICK_EXPECT_EQ( str(ball.obj.bpp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    ball.obj.bpp[0] = trick.alloc_type( 5 , "bool")
    TRICK_EXPECT_EQ( str(ball.obj.bpp[0]) , "[False, False, False, False, False]", test_suite , "2D ptr of ptr, allocate 1 row" )

    ball.obj.bpp[0][1] = 85
    TRICK_EXPECT_EQ( str(ball.obj.bpp[0]) , "[False, True, False, False, False]", test_suite , "2D ptr of ptr, assign 1 value" )

    ball.obj.bpp[1] = [ True , False , True ]
    TRICK_EXPECT_EQ( str(ball.obj.bpp[1]) , "[True, False, True]", test_suite , "2D ptr of ptr, row assignment" )

    ball.obj.bpp = None
    TRICK_EXPECT_EQ( str(ball.obj.bpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "structure"

    ball.obj.cana[0].ii = 250
    ball.obj.cana[0].jj = 350
    ball.obj.cana[1].ii = 260
    ball.obj.cana[1].jj = 360
    TRICK_EXPECT_EQ( ball.obj.cana[0].ii , 250 , test_suite , "1D array access" )
    TRICK_EXPECT_EQ( ball.obj.cana[0].jj , 350 , test_suite , "1D array access" )
    TRICK_EXPECT_EQ( ball.obj.cana[1].ii , 260 , test_suite , "1D array access" )
    TRICK_EXPECT_EQ( ball.obj.cana[1].jj , 360 , test_suite , "1D array access" )
    trick.trick_test_add_parent( test_suite , "1D array access" , "2797105872")

    ball.obj.can.ii = 150
    ball.obj.can.jj = 160
    ball.obj.can2 = ball.obj.can
    TRICK_EXPECT_EQ( ball.obj.can2.ii , 150 , test_suite , "copy" )
    TRICK_EXPECT_EQ( ball.obj.can2.jj , 160 , test_suite , "copy" )

    #ball.obj.cana[3] = ball.obj.can

    ball.obj.canp = ball.obj.can
    TRICK_EXPECT_EQ( ball.obj.canp.ii , 150 , test_suite , "pointer assignment" )
    TRICK_EXPECT_EQ( ball.obj.canp.jj , 160 , test_suite , "pointer assignment" )

    ball.obj.canp = ball.obj.cana[1]
    TRICK_EXPECT_EQ( ball.obj.canp.ii , 260 , test_suite , "pointer to array element assignment" )
    TRICK_EXPECT_EQ( ball.obj.canp.jj , 360 , test_suite , "pointer to array element assignment" )

    ball.obj.canp = trick.alloc_type( 2 , "CanCopy" )
    ball.obj.canp[0].ii = 400
    ball.obj.canp[0].jj = 500
    ball.obj.canp[1].ii = 600
    ball.obj.canp[1].jj = 700
    TRICK_EXPECT_EQ( ball.obj.canp[0].ii , 400 , test_suite , "pointer to array element assignment" )
    TRICK_EXPECT_EQ( ball.obj.canp[0].jj , 500 , test_suite , "pointer to array element assignment" )
    TRICK_EXPECT_EQ( ball.obj.canp[1].ii , 600 , test_suite , "pointer to array element assignment" )
    TRICK_EXPECT_EQ( ball.obj.canp[1].jj , 700 , test_suite , "pointer to array element assignment" )
    #print ball.obj.canp[0]
    #print ball.obj.canp[1]

    ball.obj.canp = None
    TRICK_EXPECT_EQ( str(ball.obj.canp) , "None", test_suite , "1D ptr None (NULL) assignment" )

    # silently ignored! need to figure out how to flag this
    #ball.obj.cannot = ball.obj.cannot2

######################################################################################################################

    test_suite = "string"

    ball.obj.str = "hello"
    TRICK_EXPECT_EQ( ball.obj.str , "hello" , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "165635378")

    #TODO: make a std::string typemap to allow assignment of string from char *
    #ball.obj.str = ball.obj.cap[1]
    #TRICK_EXPECT_EQ( ball.obj.str , "cap1" , "IPtest" , "string assignment from char *" )
    #print "ball.obj.str = " , ball.obj.str

######################################################################################################################

    test_suite = "bitfield"

    ball.obj.bit_0 = 7
    TRICK_EXPECT_EQ( ball.obj.bit_0 , 7 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "1649805110")

    ball.obj.bit_1 = 17
    TRICK_EXPECT_EQ( ball.obj.bit_1 , -15 , test_suite , "assignment with overflow" )

    ball.obj.boolbit_0 = True
    TRICK_EXPECT_EQ( ball.obj.boolbit_0 , True , test_suite , "bool assignment" )

    ball.obj.boolbit_1 = 2
    TRICK_EXPECT_EQ( ball.obj.boolbit_1 , True , test_suite , "bool assignment with overflow" )

######################################################################################################################

    test_suite = "union"

    ball.obj.ut.i = 20
    TRICK_EXPECT_EQ( str(ball.obj.ut.i) , "20", test_suite , "test 1" )
    trick.trick_test_add_parent( test_suite , "test 1" , "3095148896")

    ball.obj.ut2.i = 30
    TRICK_EXPECT_EQ( str(ball.obj.ut2.i) , "30", test_suite , "test 2" )
    trick.trick_test_add_parent( test_suite , "test 2" , "3095148896")

######################################################################################################################

    # swig_int from swig_double
    test_suite = "swig_int"

    ball.obj.dlbm = 50
    ball.obj.ilbm = ball.obj.dlbm
    TRICK_EXPECT_EQ( ball.obj.ilbm , 50, test_suite , "assignment from swig_double" )
    trick.trick_test_add_parent( test_suite , "assignment from swig_double" , "2901141151")

    # addition

    ball.obj.ilbm = 50
    ball.obj.ikg = ball.obj.ilbm
    TRICK_EXPECT_EQ( ball.obj.ikg , 22, test_suite , "units conversion" )

    ball.obj.ikg = ball.obj.ilbm + 20
    TRICK_EXPECT_EQ( ball.obj.ikg , 31, test_suite , "addition with integer" )

    ball.obj.ikg = ball.obj.ilbm + 20.9
    TRICK_EXPECT_EQ( ball.obj.ikg , 32, test_suite , "addition with float" )

    ball.obj.ikg = ball.obj.ilbm + ball.obj.ilbm
    TRICK_EXPECT_EQ( ball.obj.ikg , 45, test_suite , "addition with swig_int" )

    ball.obj.ikg = 50
    ball.obj.ikg = ball.obj.ilbm + ball.obj.ikg
    TRICK_EXPECT_EQ( ball.obj.ikg , 72, test_suite , "addition with swig_int and unit conversion" )

    ball.obj.dlbm = 10
    ball.obj.ikg = ball.obj.ilbm + ball.obj.dlbm
    TRICK_EXPECT_EQ( ball.obj.ikg , 27, test_suite , "addition with swig_double" )

    ball.obj.dkg = 10
    ball.obj.ikg = ball.obj.ilbm + ball.obj.dkg
    TRICK_EXPECT_EQ( ball.obj.ikg , 32, test_suite , "addition with swig_double and unit conversion" )

    # subtraction

    ball.obj.ikg = ball.obj.ilbm - 20
    TRICK_EXPECT_EQ( ball.obj.ikg , 13, test_suite , "subtraction with integer" )

    ball.obj.ikg = ball.obj.ilbm - 20.9
    TRICK_EXPECT_EQ( ball.obj.ikg , 13, test_suite , "subtraction with float" )

    ball.obj.ikg = ball.obj.ilbm - ball.obj.ilbm
    TRICK_EXPECT_EQ( ball.obj.ikg , 0, test_suite , "subtraction with swig_int" )

    ball.obj.ikg = 10
    ball.obj.ikg = ball.obj.ilbm - ball.obj.ikg
    TRICK_EXPECT_EQ( ball.obj.ikg , 12, test_suite , "subtraction with swig_int and unit conversion" )

    ball.obj.dlbm = 10
    ball.obj.ikg = ball.obj.ilbm - ball.obj.dlbm
    TRICK_EXPECT_EQ( ball.obj.ikg , 18, test_suite , "subtraction with swig_double" )

    ball.obj.dkg = 10
    ball.obj.ikg = ball.obj.ilbm - ball.obj.dkg
    TRICK_EXPECT_EQ( ball.obj.ikg , 12, test_suite , "subtraction with swig_double and unit conversion" )

    # multiplication

    ball.obj.ilbm = 50
    ball.obj.ikg = ball.obj.ilbm * 3
    TRICK_EXPECT_EQ( ball.obj.ikg , 68, test_suite , "multiplication with int" )

    ball.obj.ikg = ball.obj.ilbm * 2.9
    TRICK_EXPECT_EQ( ball.obj.ikg , 65, test_suite , "multiplication with float" )

    ball.obj.ilbm = 50
    ball.obj.i = 2
    ball.obj.ikg = ball.obj.ilbm * ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.ikg , 45, test_suite , "multiplication with unitless swig_int" )

    ball.obj.ilbm = 50
    ball.obj.dno_units = 2.2
    ball.obj.ikg = ball.obj.ilbm * ball.obj.dno_units
    TRICK_EXPECT_EQ( ball.obj.ikg , 49, test_suite , "multiplication with unitless swig_double" )

    # division

    ball.obj.ilbm = 50
    ball.obj.ikg = ball.obj.ilbm / 3
    TRICK_EXPECT_EQ( ball.obj.ikg , 7, test_suite , "division with int" )

    ball.obj.ikg = ball.obj.ilbm / 2.9
    TRICK_EXPECT_EQ( ball.obj.ikg , 7, test_suite , "division with float" )

    # floor division

    ball.obj.ikg = 29
    ball.obj.ikg = ball.obj.ikg // 4
    TRICK_EXPECT_EQ( ball.obj.ikg , 7, test_suite , "floor division with int" )

    ball.obj.ikg = 29
    ball.obj.ikg = ball.obj.ikg // 4.5
    TRICK_EXPECT_EQ( ball.obj.ikg , 6, test_suite , "floor division with int" )

    # mod

    ball.obj.ikg = 29
    ball.obj.ikg = ball.obj.ikg % 4
    TRICK_EXPECT_EQ( ball.obj.ikg , 1, test_suite , "mod with int" )

    ball.obj.ikg = 29
    ball.obj.ikg = ball.obj.ikg % 4.5
    TRICK_EXPECT_EQ( ball.obj.ikg , 2, test_suite , "mod with float" )

    ball.obj.ilbm = 50
    ball.obj.i = 13
    ball.obj.ikg = ball.obj.ilbm % ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.ikg , 4, test_suite , "mod with unitless swig_int" )

    ball.obj.ilbm = 50
    ball.obj.dno_units = 13.5
    ball.obj.ikg = ball.obj.ilbm % ball.obj.dno_units
    TRICK_EXPECT_EQ( ball.obj.ikg , 4, test_suite , "mod with unitless swig_double" )

    # pow

    ball.obj.i = 5
    ball.obj.i = pow(ball.obj.i , 4)
    TRICK_EXPECT_EQ( ball.obj.i , 625, test_suite , "pow with int" )

    ball.obj.i = 5
    ball.obj.i = pow(ball.obj.i , 2.5)
    TRICK_EXPECT_EQ( ball.obj.i , 55, test_suite , "pow with float" )

    ball.obj.i = 5
    ball.obj.i = pow(ball.obj.i , ball.obj.i)
    TRICK_EXPECT_EQ( ball.obj.i , 3125, test_suite , "pow with unitless swig_int" )

    ball.obj.i = 5
    ball.obj.dno_units = 5.0
    ball.obj.i = pow(ball.obj.i , ball.obj.dno_units)
    TRICK_EXPECT_EQ( ball.obj.i , 3125, test_suite , "pow with unitless swig_double" )

    # left shift

    ball.obj.ikg = 16
    ball.obj.ikg = ball.obj.ikg << 2
    TRICK_EXPECT_EQ( ball.obj.ikg , 64, test_suite , "left shift with int" )

    ball.obj.ikg = 16
    ball.obj.i = 1
    ball.obj.ikg = ball.obj.ikg << ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.ikg , 32, test_suite , "left shift with unitless swig_int" )

    # right shift

    ball.obj.ikg = 16
    ball.obj.ikg = ball.obj.ikg >> 2
    TRICK_EXPECT_EQ( ball.obj.ikg , 4, test_suite , "right shift with int" )

    ball.obj.ikg = 16
    ball.obj.i = 1
    ball.obj.ikg = ball.obj.ikg >> ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.ikg , 8, test_suite , "left shift with unitless swig_int" )

    # and

    ball.obj.ikg = 12
    ball.obj.ikg = ball.obj.ikg & 5
    TRICK_EXPECT_EQ( ball.obj.ikg , 4, test_suite , "and with int" )

    ball.obj.ikg = 12
    ball.obj.i = 5
    ball.obj.ikg = ball.obj.ikg & ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.ikg , 4, test_suite , "and with unitless swig_int" )

    # xor

    ball.obj.ikg = 29
    ball.obj.ikg = ball.obj.ikg ^ 7
    TRICK_EXPECT_EQ( ball.obj.ikg , 26, test_suite , "xor with int" )

    ball.obj.ikg = 29
    ball.obj.i = 7
    ball.obj.ikg = ball.obj.ikg ^ ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.ikg , 26, test_suite , "xor with unitless swig_int" )

    # or

    ball.obj.ikg = 29
    ball.obj.ikg = ball.obj.ikg | 7
    TRICK_EXPECT_EQ( ball.obj.ikg , 31, test_suite , "or with int" )

    ball.obj.ikg = 29
    ball.obj.i = 7
    ball.obj.ikg = ball.obj.ikg | ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.ikg , 31, test_suite , "or with unitless swig_int" )

    # reverse addition

    ball.obj.ikg = 20 + ball.obj.ilbm
    TRICK_EXPECT_EQ( ball.obj.ikg , 31, test_suite , "reverse addition with integer" )

    ball.obj.ikg = 20.9 + ball.obj.ilbm
    TRICK_EXPECT_EQ( ball.obj.ikg , 32, test_suite , "reverse addition with float" )

    # reverse subtraction

    ball.obj.ikg = 120 - ball.obj.ilbm
    TRICK_EXPECT_EQ( ball.obj.ikg , 31, test_suite , "reverse subtraction with integer" )

    ball.obj.ikg = 120.9 - ball.obj.ilbm
    TRICK_EXPECT_EQ( ball.obj.ikg , 32, test_suite , "reverse subtraction with float" )

    # reverse multiplication

    ball.obj.ilbm = 50
    ball.obj.ikg = 3 * ball.obj.ilbm
    TRICK_EXPECT_EQ( ball.obj.ikg , 68, test_suite , "reverse multiplication with int" )

    ball.obj.ikg = 2.9 * ball.obj.ilbm
    TRICK_EXPECT_EQ( ball.obj.ikg , 65, test_suite , "reverse multiplication with float" )

    # reverse division

    ball.obj.i = 5
    ball.obj.i = 62 / ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.i , 12, test_suite , "reverse division with int" )

    ball.obj.i = 5
    ball.obj.i = 62.5 / ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.i , 12, test_suite , "reverse division with float" )

    # reverse mod

    ball.obj.i = 5
    ball.obj.i = 62 % ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.i , 2, test_suite , "reverse mod with int" )

    ball.obj.i = 5
    ball.obj.i = 62.5 % ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.i , 2, test_suite , "reverse mod with float" )

    # pow

    ball.obj.i = 4
    ball.obj.i = pow(4 , ball.obj.i)
    TRICK_EXPECT_EQ( ball.obj.i , 256, test_suite , "reverse pow with int" )

    ball.obj.i = 5
    ball.obj.i = pow(2.1 , ball.obj.i)
    TRICK_EXPECT_EQ( ball.obj.i , 40, test_suite , "reverse pow with float" )

    # reverse lshift
    ball.obj.i = 3
    ball.obj.i = 8 << ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.i , 64, test_suite , "reverse lshift with int" )

    # reverse rshift
    ball.obj.i = 2
    ball.obj.i = 8 >> ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.i , 2, test_suite , "reverse rshift with int" )

    # reverse and

    ball.obj.ikg = 12
    ball.obj.ikg = 5 & ball.obj.ikg
    TRICK_EXPECT_EQ( ball.obj.ikg , 4, test_suite , "reverse and with int" )

    # reverse xor

    ball.obj.ikg = 29
    ball.obj.ikg = 7 ^ ball.obj.ikg
    TRICK_EXPECT_EQ( ball.obj.ikg , 26, test_suite , "reverse xor with int" )

    # reverse or

    ball.obj.ikg = 29
    ball.obj.ikg = 7 | ball.obj.ikg
    TRICK_EXPECT_EQ( ball.obj.ikg , 31, test_suite , "reverse or with int" )

    # in-place addition

    ball.obj.ikg = 10
    ball.obj.ikg += 20
    TRICK_EXPECT_EQ( ball.obj.ikg , 30, test_suite , "in-place addition with integer" )

    ball.obj.ikg += 20.9
    TRICK_EXPECT_EQ( ball.obj.ikg , 51, test_suite , "in-place addition with float" )

    ball.obj.ilbm = 10
    ball.obj.ikg += ball.obj.ilbm
    TRICK_EXPECT_EQ( ball.obj.ikg , 55, test_suite , "in-place addition with swig_int" )

    ball.obj.dkg = 10
    ball.obj.ikg += ball.obj.dkg
    TRICK_EXPECT_EQ( ball.obj.ikg , 65, test_suite , "in-place addition with swig_double and unit conversion" )

    # in-place subtraction

    ball.obj.ikg = 10
    ball.obj.ikg -= 2
    TRICK_EXPECT_EQ( ball.obj.ikg , 8, test_suite , "in-place subtraction with integer" )

    ball.obj.ikg -= 2.9
    TRICK_EXPECT_EQ( ball.obj.ikg , 5, test_suite , "in-place subtraction with float" )

    ball.obj.ilbm = 10
    ball.obj.ikg -= ball.obj.ilbm
    TRICK_EXPECT_EQ( ball.obj.ikg , 1, test_suite , "in-place subtraction with swig_int" )

    ball.obj.dkg = 1
    ball.obj.ikg -= ball.obj.dkg
    TRICK_EXPECT_EQ( ball.obj.ikg , 0, test_suite , "in-place subtraction with swig_double and unit conversion" )

    # in-place multiplication

    ball.obj.ikg = 10
    ball.obj.ikg *= 2
    TRICK_EXPECT_EQ( ball.obj.ikg , 20, test_suite , "in-place multiplication with integer" )

    ball.obj.ikg *= 3.9
    TRICK_EXPECT_EQ( ball.obj.ikg , 78, test_suite , "in-place multiplication with float" )

    ball.obj.ikg = 10
    ball.obj.i = 2
    ball.obj.ikg *= ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.ikg , 20, test_suite , "in-place multiplication with unitless swig_int" )

    ball.obj.ikg = 10
    ball.obj.dno_units = 3.9
    ball.obj.ikg *= ball.obj.dno_units
    TRICK_EXPECT_EQ( ball.obj.ikg , 39, test_suite , "in-place multiplication with unitless swig_double" )
    # in-place division

    ball.obj.ikg = 10
    ball.obj.ikg /= 2
    TRICK_EXPECT_EQ( ball.obj.ikg , 5, test_suite , "in-place division with integer" )

    ball.obj.ikg = 10
    ball.obj.ikg /= 3.9
    TRICK_EXPECT_EQ( ball.obj.ikg , 3, test_suite , "in-place division with float" )

    ball.obj.ikg = 10
    ball.obj.i = 2
    ball.obj.ikg /= ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.ikg , 5, test_suite , "in-place division with unitless swig_int" )

    # in-place mod

    ball.obj.ikg = 10
    ball.obj.ikg %= 3
    TRICK_EXPECT_EQ( ball.obj.ikg , 1, test_suite , "in-place mod with integer" )

    ball.obj.ikg = 10
    ball.obj.ikg %= 3.9
    TRICK_EXPECT_EQ( ball.obj.ikg , 2, test_suite , "in-place mod with float" )

    ball.obj.ikg = 10
    ball.obj.i = 3
    ball.obj.ikg %= ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.ikg , 1, test_suite , "in-place mod with unitless swig_int" )

    # in-place pow

    ball.obj.i = 5
    ball.obj.i **= 4
    TRICK_EXPECT_EQ( ball.obj.i , 625, test_suite , "in-place pow with int" )

    ball.obj.i = 5
    ball.obj.i **= 2.5
    TRICK_EXPECT_EQ( ball.obj.i , 56, test_suite , "in-place pow with float" )

    ball.obj.i = 5
    ball.obj.i **= ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.i , 3125, test_suite , "in-place pow with unitless swig_int" )

    ball.obj.i = 5
    ball.obj.dno_units = 5.0
    ball.obj.i **= ball.obj.dno_units
    TRICK_EXPECT_EQ( ball.obj.i , 3125, test_suite , "in-place pow with unitless swig_double" )

    # in-place left shift

    ball.obj.ikg = 16
    ball.obj.ikg <<= 2
    TRICK_EXPECT_EQ( ball.obj.ikg , 64, test_suite , "left shift with int" )

    ball.obj.i = 16
    ball.obj.ia[0] = 1
    ball.obj.i <<= ball.obj.ia[0]
    TRICK_EXPECT_EQ( ball.obj.i , 32, test_suite , "left shift with unitless swig_int" )

    # in-place right shift

    ball.obj.ikg = 16
    ball.obj.ikg >>= 2
    TRICK_EXPECT_EQ( ball.obj.ikg , 4, test_suite , "right shift with int" )

    ball.obj.ikg = 16
    ball.obj.i = 1
    ball.obj.ikg >>= ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.ikg , 8, test_suite , "left shift with unitless swig_int" )

    # in-place and

    ball.obj.ikg = 12
    ball.obj.ikg &= 5
    TRICK_EXPECT_EQ( ball.obj.ikg , 4, test_suite , "and with int" )

    ball.obj.ikg = 12
    ball.obj.i = 5
    ball.obj.ikg &= ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.ikg , 4, test_suite , "and with unitless swig_int" )

    # in-place xor

    ball.obj.ikg = 29
    ball.obj.ikg ^= 7
    TRICK_EXPECT_EQ( ball.obj.ikg , 26, test_suite , "xor with int" )

    ball.obj.ikg = 29
    ball.obj.i = 7
    ball.obj.ikg ^= ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.ikg , 26, test_suite , "xor with unitless swig_int" )

    # in-place or

    ball.obj.ikg = 29
    ball.obj.ikg |= 7
    TRICK_EXPECT_EQ( ball.obj.ikg , 31, test_suite , "or with int" )

    ball.obj.ikg = 29
    ball.obj.i = 7
    ball.obj.ikg |= ball.obj.i
    TRICK_EXPECT_EQ( ball.obj.ikg , 31, test_suite , "or with unitless swig_int" )

    # less than

    ball.obj.ikg = 20
    ball.obj.ilbm = 20

    test = ball.obj.ikg < 21
    TRICK_EXPECT_EQ( test , True, test_suite , "lt with integer" )

    test = ball.obj.ikg < 20.5
    TRICK_EXPECT_EQ( test , True, test_suite , "lt with float" )

    test = ball.obj.ikg < ball.obj.ilbm
    TRICK_EXPECT_EQ( test , False, test_suite , "lt with swig_int and unit conversion" )

    ball.obj.dkg = 20.1
    test = ball.obj.ikg < ball.obj.dkg
    TRICK_EXPECT_EQ( test , True, test_suite , "lt with swig_double and unit conversion" )

    # less than or equal

    ball.obj.ikg = 20
    ball.obj.ilbm = 20

    test = ball.obj.ikg <= 21
    TRICK_EXPECT_EQ( test , True, test_suite , "le with integer" )

    test = ball.obj.ikg <= 20.5
    TRICK_EXPECT_EQ( test , True, test_suite , "le with float" )

    test = ball.obj.ikg <= ball.obj.ilbm
    TRICK_EXPECT_EQ( test , False, test_suite , "le with swig_int and unit conversion" )

    ball.obj.dkg = 20.1
    test = ball.obj.ikg <= ball.obj.dkg
    TRICK_EXPECT_EQ( test , True, test_suite , "le with swig_double and unit conversion" )

    # equal

    test = ball.obj.ikg == 21
    TRICK_EXPECT_EQ( test , False, test_suite , "eq with integer" )

    test = ball.obj.ikg == 20.5
    TRICK_EXPECT_EQ( test , False, test_suite , "eq with float" )

    test = ball.obj.ikg == ball.obj.ilbm
    TRICK_EXPECT_EQ( test , False, test_suite , "eq with swig_int and unit conversion" )

    ball.obj.dkg = 20.1
    test = ball.obj.ikg == ball.obj.dkg
    TRICK_EXPECT_EQ( test , False, test_suite , "eq with swig_double and unit conversion" )

    # not equal

    test = ball.obj.ikg != 21
    TRICK_EXPECT_EQ( test , True, test_suite , "ne with integer" )

    test = ball.obj.ikg != 20.5
    TRICK_EXPECT_EQ( test , True, test_suite , "ne with float" )

    test = ball.obj.ikg != ball.obj.ilbm
    TRICK_EXPECT_EQ( test , True, test_suite , "ne with swig_int and unit conversion" )

    ball.obj.dkg = 20.1
    test = ball.obj.ikg != ball.obj.dkg
    TRICK_EXPECT_EQ( test , True, test_suite , "ne with swig_double and unit conversion" )

    # greater than

    ball.obj.ikg = 20
    ball.obj.ilbm = 20

    test = ball.obj.ikg > 21
    TRICK_EXPECT_EQ( test , False, test_suite , "gt with integer" )

    test = ball.obj.ikg > 20.5
    TRICK_EXPECT_EQ( test , False, test_suite , "gt with float" )

    test = ball.obj.ikg > ball.obj.ilbm
    TRICK_EXPECT_EQ( test , True, test_suite , "gt with swig_int and unit conversion" )

    ball.obj.dkg = 20.1
    test = ball.obj.ikg > ball.obj.dkg
    TRICK_EXPECT_EQ( test , False, test_suite , "gt with swig_double and unit conversion" )

    # greater than or equal

    ball.obj.ikg = 20
    ball.obj.ilbm = 20

    test = ball.obj.ikg >= 21
    TRICK_EXPECT_EQ( test , False, test_suite , "ge with integer" )

    test = ball.obj.ikg >= 20.5
    TRICK_EXPECT_EQ( test , False, test_suite , "ge with float" )

    test = ball.obj.ikg >= ball.obj.ilbm
    TRICK_EXPECT_EQ( test , True, test_suite , "ge with swig_int and unit conversion" )

    ball.obj.dkg = 20.1
    test = ball.obj.ikg >= ball.obj.dkg
    TRICK_EXPECT_EQ( test , False, test_suite , "ge with swig_double and unit conversion" )

    # unary operators

    ball.obj.ikg = 20
    test = -ball.obj.ikg
    TRICK_EXPECT_EQ( test , -20, test_suite , "unary neg" )

    test = +ball.obj.ikg
    TRICK_EXPECT_EQ( test , 20, test_suite , "unary pos" )

    ball.obj.ikg = -20
    test = abs(ball.obj.ikg)
    TRICK_EXPECT_EQ( test , 20, test_suite , "unary abs" )

    ball.obj.ikg = 20
    test = ~ball.obj.ikg
    TRICK_EXPECT_EQ( test , -21, test_suite , "unary invert" )

    # conversion
    ball.obj.ikg = 20
    test = int(ball.obj.ikg)
    TRICK_EXPECT_EQ( test , 20, test_suite , "int" )

    ball.obj.ikg = 20
    test = long(ball.obj.ikg)
    TRICK_EXPECT_EQ( test , 20, test_suite , "long" )

    ball.obj.ikg = 20
    test = float(ball.obj.ikg)
    TRICK_EXPECT_EQ( test , 20, test_suite , "float" )

    ball.obj.ikg = 20
    test = oct(ball.obj.ikg)
    TRICK_EXPECT_EQ( test , "024", test_suite , "oct" )

    ball.obj.ikg = 20
    test = hex(ball.obj.ikg)
    TRICK_EXPECT_EQ( test , "0x14", test_suite , "hex" )

######################################################################################################################

    # swig_integer to swig_double assignment
    test_suite = "swig_double"

    ball.obj.ilbm = 50
    ball.obj.dlbm = ball.obj.ilbm

    # swig_double unitless to swig_double assignment

    ball.obj.dno_units = trick.attach_units("--" , 60.6)
    TRICK_EXPECT_EQ( ball.obj.dno_units , 60.6, test_suite , "assignment from unitless swig_double" )
    trick.trick_test_add_parent( test_suite , "assignment from unitless swig_double" , "1164062396")

    # addition

    ball.obj.dlbm = 50
    ball.obj.dkg = ball.obj.dlbm
    TRICK_EXPECT_NEAR( ball.obj.dkg , 22.6796, 0.0001, test_suite , "units conversion" )

    ball.obj.dkg = ball.obj.dlbm + 20
    TRICK_EXPECT_NEAR( ball.obj.dkg , 31.7515, 0.0001, test_suite , "addition with integer" )

    ball.obj.dkg = ball.obj.dlbm + 20.9
    TRICK_EXPECT_NEAR( ball.obj.dkg , 32.1597, 0.0001, test_suite , "addition with float" )

    ball.obj.ilbm = 50
    ball.obj.dkg = ball.obj.dlbm + ball.obj.ilbm
    TRICK_EXPECT_NEAR( ball.obj.dkg , 45.3592, 0.0001, test_suite , "addition with swig_int" )

    ball.obj.dkg = 50
    ball.obj.ikg = 50
    ball.obj.dkg = ball.obj.dlbm + ball.obj.ikg
    TRICK_EXPECT_NEAR( ball.obj.dkg , 72.6796, 0.0001, test_suite , "addition with swig_int and unit conversion" )

    ball.obj.dlbm = 10
    ball.obj.dkg = ball.obj.dlbm + ball.obj.dlbm
    TRICK_EXPECT_NEAR( ball.obj.dkg , 9.07185, 0.0001, test_suite , "addition with swig_double" )

    ball.obj.dkg = 10
    ball.obj.dkg = ball.obj.dlbm + ball.obj.dkg
    TRICK_EXPECT_NEAR( ball.obj.dkg , 14.5359, 0.0001, test_suite , "addition with swig_double and unit conversion" )

    # subtraction

    ball.obj.dlbm = 50
    ball.obj.dkg = ball.obj.dlbm - 20
    TRICK_EXPECT_NEAR( ball.obj.dkg , 13.6078, 0.0001, test_suite , "subtraction with integer" )

    ball.obj.dkg = ball.obj.dlbm - 20.9
    TRICK_EXPECT_NEAR( ball.obj.dkg , 13.1995, 0.0001, test_suite , "subtraction with float" )

    ball.obj.ilbm = 50
    ball.obj.dkg = ball.obj.dlbm - ball.obj.ilbm
    TRICK_EXPECT_NEAR( ball.obj.dkg , 0, 0.0001, test_suite , "subtraction with swig_int" )

    ball.obj.dkg = 50
    ball.obj.ikg = 50
    ball.obj.dkg = ball.obj.dlbm - ball.obj.ikg
    TRICK_EXPECT_NEAR( ball.obj.dkg , -27.3204, 0.0001, test_suite , "subtraction with swig_int and unit conversion" )

    ball.obj.dlbm = 10
    ball.obj.dkg = ball.obj.dlbm - ball.obj.dlbm
    TRICK_EXPECT_NEAR( ball.obj.dkg , 0, 0.0001, test_suite , "subtraction with swig_double" )

    ball.obj.dkg = 10
    ball.obj.dkg = ball.obj.dlbm - ball.obj.dkg
    TRICK_EXPECT_NEAR( ball.obj.dkg , -5.46408, 0.0001, test_suite , "subtraction with swig_double and unit conversion" )

    # multiplication

    ball.obj.dlbm = 50
    ball.obj.dkg = ball.obj.dlbm * 3
    TRICK_EXPECT_NEAR( ball.obj.dkg , 68.0389, 0.0001, test_suite , "multiplication with int" )

    ball.obj.dkg = ball.obj.ilbm * 2.9
    TRICK_EXPECT_NEAR( ball.obj.dkg , 65.7709, 0.0001, test_suite , "multiplication with float" )

    ball.obj.dlbm = 50
    ball.obj.i = 2
    ball.obj.dkg = ball.obj.dlbm * ball.obj.i
    TRICK_EXPECT_NEAR( ball.obj.dkg , 45.3592, 0.0001, test_suite , "multiplication with unitless swig_int" )

    ball.obj.dlbm = 50
    ball.obj.dno_units = 2.2
    ball.obj.dkg = ball.obj.dlbm * ball.obj.dno_units
    TRICK_EXPECT_NEAR( ball.obj.dkg , 49.8952, 0.0001, test_suite , "multiplication with unitless swig_double" )

    # division

    ball.obj.dlbm = 50
    ball.obj.dkg = ball.obj.dlbm / 3
    TRICK_EXPECT_NEAR( ball.obj.dkg , 7.55987, 0.0001, test_suite , "division with int" )

    ball.obj.dkg = ball.obj.dlbm / 2.9
    TRICK_EXPECT_NEAR( ball.obj.dkg , 7.82056, 0.0001, test_suite , "division with float" )

    ball.obj.i = 5
    ball.obj.dkg = ball.obj.dlbm / ball.obj.i
    TRICK_EXPECT_NEAR( ball.obj.dkg , 4.53592, 0.0001, test_suite , "division with unitless swig_int" )

    ball.obj.dno_units = 5.1
    ball.obj.dkg = ball.obj.dlbm / ball.obj.dno_units
    TRICK_EXPECT_NEAR( ball.obj.dkg , 4.44698, 0.0001, test_suite , "division with unitless swig_double" )

    # floor division

    ball.obj.dkg = 29
    ball.obj.dkg = ball.obj.dkg // 4
    TRICK_EXPECT_NEAR( ball.obj.dkg , 7, 0.0001, test_suite , "floor division with int" )

    ball.obj.dkg = 29
    ball.obj.dkg = ball.obj.dkg // 4.5
    TRICK_EXPECT_NEAR( ball.obj.dkg , 6, 0.0001, test_suite , "floor division with float" )

    ball.obj.dkg = 29
    ball.obj.i = 4
    ball.obj.dkg = ball.obj.dkg // ball.obj.i
    TRICK_EXPECT_NEAR( ball.obj.dkg , 7, 0.0001, test_suite , "floor division with unitless swig_int" )

    ball.obj.dkg = 29
    ball.obj.dno_units = 4.5
    ball.obj.dkg = ball.obj.dkg // ball.obj.dno_units
    TRICK_EXPECT_NEAR( ball.obj.dkg , 6, 0.0001, test_suite , "floor division with unitless swig_double" )


    # mod

    ball.obj.dkg = 29
    ball.obj.dkg = ball.obj.dkg % 4
    TRICK_EXPECT_NEAR( ball.obj.dkg , 1, 0.0001, test_suite , "mod with int" )

    ball.obj.dkg = 29
    ball.obj.dkg = ball.obj.dkg % 4.5
    TRICK_EXPECT_NEAR( ball.obj.dkg , 2, 0.0001, test_suite , "mod with float" )

    ball.obj.dlbm = 50
    ball.obj.i = 13
    ball.obj.dlbm = ball.obj.dlbm % ball.obj.i
    TRICK_EXPECT_NEAR( ball.obj.dlbm , 11, 0.0001, test_suite , "mod with unitless swig_int" )

    ball.obj.dlbm = 50
    ball.obj.dno_units = 13.5
    ball.obj.dlbm = ball.obj.dlbm % ball.obj.dno_units
    TRICK_EXPECT_NEAR( ball.obj.dlbm , 9.5, 0.0001, test_suite , "mod with unitless swig_double" )

    # pow

    ball.obj.dno_units = 5
    ball.obj.dno_units = pow(ball.obj.dno_units , 4)
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 625, 0.0001, test_suite , "pow with int" )

    ball.obj.dno_units = 5
    ball.obj.dno_units = pow(ball.obj.dno_units , 2.5)
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 55.9017, 0.0001, test_suite , "pow with float" )

    ball.obj.i = 5
    ball.obj.dno_units = 5.0
    ball.obj.dno_units = pow(ball.obj.dno_units , ball.obj.i)
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 3125, 0.0001, test_suite , "pow with unitless swig_int" )

    ball.obj.dno_units = 5.0
    ball.obj.dno_units = pow(ball.obj.dno_units , ball.obj.dno_units)
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 3125, 0.0001, test_suite , "pow with unitless swig_double" )

    # reverse addition

    ball.obj.dlbm = 10
    ball.obj.dkg = 20 + ball.obj.dlbm
    TRICK_EXPECT_NEAR( ball.obj.dkg , 13.6078, 0.0001, test_suite , "reverse addition with integer" )

    ball.obj.dkg = 20.9 + ball.obj.dlbm
    TRICK_EXPECT_NEAR( ball.obj.dkg , 14.016, 0.0001, test_suite , "reverse addition with float" )

    # reverse subtraction

    ball.obj.dkg = 120 - ball.obj.dlbm
    TRICK_EXPECT_NEAR( ball.obj.dkg , 49.8952, 0.0001, test_suite , "reverse subtraction with integer" )

    ball.obj.dkg = 120.9 - ball.obj.dlbm
    TRICK_EXPECT_NEAR( ball.obj.dkg , 50.3034, 0.0001, test_suite , "reverse subtraction with float" )

    # reverse multiplication

    ball.obj.dlbm = 50
    ball.obj.dkg = 3 * ball.obj.dlbm
    TRICK_EXPECT_NEAR( ball.obj.dkg , 68.0389, 0.0001, test_suite , "reverse multiplication with int" )

    ball.obj.dkg = 2.9 * ball.obj.dlbm
    TRICK_EXPECT_NEAR( ball.obj.dkg , 65.7709, 0.0001, test_suite , "reverse multiplication with float" )


    # reverse division

    ball.obj.dno_units = 5
    ball.obj.dno_units = 62 / ball.obj.dno_units
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 12.4, 0.0001, test_suite , "reverse division with int" )

    ball.obj.dno_units = 5
    ball.obj.dno_units = 62.5 / ball.obj.dno_units
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 12.5, 0.0001, test_suite , "reverse division with float" )

    # reverse floor division
    ball.obj.dno_units = 5
    ball.obj.dno_units = 62 // ball.obj.dno_units
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 12, 0.0001, test_suite , "reverse floor division with int" )

    ball.obj.dno_units = 5
    ball.obj.dno_units = 62.5 // ball.obj.dno_units
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 12, 0.0001, test_suite , "reverse floor division with float" )


    # reverse mod

    ball.obj.dno_units = 5
    ball.obj.dno_units = 62 % ball.obj.dno_units
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 2, 0.0001, test_suite , "reverse mod with int" )

    ball.obj.dno_units = 5
    ball.obj.dno_units = 62.5 % ball.obj.dno_units
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 2.5, 0.0001, test_suite , "reverse mod with float" )

    # reverse pow

    ball.obj.dno_units = 4
    ball.obj.dno_units = pow(4 , ball.obj.dno_units)
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 256, 0.0001, test_suite , "reverse pow with int" )

    ball.obj.dno_units = 5
    ball.obj.dno_units = pow(2.1 , ball.obj.dno_units)
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 40.841 , 0.0001, test_suite , "reverse pow with float" )

    # in-place addition

    ball.obj.dkg = 10
    ball.obj.dkg += 20
    TRICK_EXPECT_NEAR( ball.obj.dkg , 30, 0.0001, test_suite , "in-place addition with integer" )

    ball.obj.dkg += 20.9
    TRICK_EXPECT_NEAR( ball.obj.dkg , 50.9, 0.0001, test_suite , "in-place addition with float" )

    ball.obj.ilbm = 10
    ball.obj.dkg += ball.obj.ilbm
    TRICK_EXPECT_NEAR( ball.obj.dkg , 55.4359, 0.0001, test_suite , "in-place addition with swig_int" )

    ball.obj.dkg = 10
    ball.obj.dkg += ball.obj.dkg
    TRICK_EXPECT_NEAR( ball.obj.dkg , 20, 0.0001, test_suite , "in-place addition with swig_double and unit conversion" )

    # in-place subtraction

    ball.obj.dkg = 10
    ball.obj.dkg -= 2
    TRICK_EXPECT_NEAR( ball.obj.dkg , 8, 0.0001, test_suite , "in-place subtraction with integer" )

    ball.obj.dkg -= 2.9
    TRICK_EXPECT_NEAR( ball.obj.dkg , 5.1, 0.0001, test_suite , "in-place subtraction with float" )

    ball.obj.dlbm = 10
    ball.obj.dkg -= ball.obj.dlbm
    TRICK_EXPECT_NEAR( ball.obj.dkg , 0.564076, 0.0001, test_suite , "in-place subtraction with swig_int" )

    ball.obj.dkg = 1
    ball.obj.dkg -= ball.obj.dkg
    TRICK_EXPECT_NEAR( ball.obj.dkg , 0, 0.0001, test_suite , "in-place subtraction with swig_double and unit conversion" )

    # in-place multiplication

    ball.obj.dkg = 10
    ball.obj.dkg *= 2
    TRICK_EXPECT_NEAR( ball.obj.dkg , 20, 0.0001, test_suite , "in-place multiplication with integer" )

    ball.obj.dkg *= 3.9
    TRICK_EXPECT_NEAR( ball.obj.dkg , 78, 0.0001, test_suite , "in-place multiplication with float" )

    ball.obj.dkg = 10
    ball.obj.i = 2
    ball.obj.dkg *= ball.obj.i
    TRICK_EXPECT_NEAR( ball.obj.dkg , 20, 0.0001, test_suite , "in-place multiplication with unitless swig_int" )

    ball.obj.dkg = 10
    ball.obj.dno_units = 2
    ball.obj.dkg *= ball.obj.dno_units
    TRICK_EXPECT_NEAR( ball.obj.dkg , 20, 0.0001, test_suite , "in-place multiplication with unitless swig_int" )

    # in-place division

    ball.obj.dkg = 10
    ball.obj.dkg /= 2
    TRICK_EXPECT_NEAR( ball.obj.dkg , 5, 0.0001, test_suite , "in-place division with integer" )

    ball.obj.dkg = 10
    ball.obj.dkg /= 3.9
    TRICK_EXPECT_NEAR( ball.obj.dkg , 2.5641, 0.0001, test_suite , "in-place division with float" )

    ball.obj.dkg = 10
    ball.obj.i = 2
    ball.obj.dkg /= ball.obj.i
    TRICK_EXPECT_NEAR( ball.obj.dkg , 5, 0.0001, test_suite , "in-place division with unitless swig_int" )

    ball.obj.dkg = 10
    ball.obj.dno_units = 2
    ball.obj.dkg /= ball.obj.dno_units
    TRICK_EXPECT_NEAR( ball.obj.dkg , 5, 0.0001, test_suite , "in-place division with unitless swig_int" )

    # in-place floor division

    ball.obj.dkg = 10.1
    ball.obj.dkg //= 2
    TRICK_EXPECT_NEAR( ball.obj.dkg , 5, 0.0001, test_suite , "in-place division with integer" )

    ball.obj.dkg = 10.1
    ball.obj.dkg //= 3.9
    TRICK_EXPECT_NEAR( ball.obj.dkg , 2, 0.0001, test_suite , "in-place division with float" )

    ball.obj.dkg = 10.1
    ball.obj.i = 2
    ball.obj.dkg //= ball.obj.i
    TRICK_EXPECT_NEAR( ball.obj.dkg , 5, 0.0001, test_suite , "in-place division with unitless swig_int" )

    ball.obj.dkg = 10.1
    ball.obj.dno_units = 2
    ball.obj.dkg //= ball.obj.dno_units
    TRICK_EXPECT_NEAR( ball.obj.dkg , 5, 0.0001, test_suite , "in-place division with unitless swig_int" )

    # in-place mod

    ball.obj.dkg = 10.1
    ball.obj.dkg %= 3
    TRICK_EXPECT_NEAR( ball.obj.dkg , 1.1, 0.0001, test_suite , "in-place mod with integer" )

    ball.obj.dkg = 10.1
    ball.obj.dkg %= 3.9
    TRICK_EXPECT_NEAR( ball.obj.dkg , 2.3, 0.0001, test_suite , "in-place mod with float" )

    ball.obj.dkg = 10.1
    ball.obj.i = 3
    ball.obj.dkg %= ball.obj.i
    TRICK_EXPECT_NEAR( ball.obj.dkg , 1.1, 0.0001, test_suite , "in-place mod with unitless swig_int" )

    ball.obj.dkg = 10.1
    ball.obj.dno_units = 4
    ball.obj.dkg %= ball.obj.dno_units
    TRICK_EXPECT_NEAR( ball.obj.dkg , 2.1, 0.0001, test_suite , "in-place mod with unitless swig_double" )

    # in-place pow

    ball.obj.dno_units = 5
    ball.obj.dno_units **= 4
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 625, 0.0001, test_suite , "in-place pow with int" )

    ball.obj.dno_units = 5
    ball.obj.dno_units **= 2.5
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 55.9017, 0.0001, test_suite , "in-place pow with float" )

    ball.obj.i = 5
    ball.obj.dno_units = 5.0
    ball.obj.dno_units **= ball.obj.i
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 3125, 0.0001, test_suite , "in-place pow with unitless swig_int" )

    ball.obj.dno_units = 5.0
    ball.obj.dno_units **= ball.obj.dno_units
    TRICK_EXPECT_NEAR( ball.obj.dno_units , 3125, 0.0001, test_suite , "in-place pow with unitless swig_double" )

    # less than

    ball.obj.dkg = 20
    ball.obj.dlbm = 20

    test = ball.obj.dkg < 21
    TRICK_EXPECT_EQ( test , True, test_suite , "lt with integer" )

    test = ball.obj.dkg < 20.5
    TRICK_EXPECT_EQ( test , True, test_suite , "lt with float" )

    test = ball.obj.dkg < ball.obj.dlbm
    TRICK_EXPECT_EQ( test , False, test_suite , "lt with swig_int and unit conversion" )

    ball.obj.dkg = 20.1
    test = ball.obj.dkg < ball.obj.dkg
    TRICK_EXPECT_EQ( test , False, test_suite , "lt with swig_double and unit conversion" )

    # less than or equal

    ball.obj.dkg = 20
    ball.obj.dlbm = 20

    test = ball.obj.dkg <= 21
    TRICK_EXPECT_EQ( test , True, test_suite , "le with integer" )

    test = ball.obj.dkg <= 20.5
    TRICK_EXPECT_EQ( test , True, test_suite , "le with float" )

    test = ball.obj.dkg <= ball.obj.dlbm
    TRICK_EXPECT_EQ( test , False, test_suite , "le with swig_int and unit conversion" )

    ball.obj.dkg = 20.1
    test = ball.obj.dkg <= ball.obj.dkg
    TRICK_EXPECT_EQ( test , True, test_suite , "le with swig_double and unit conversion" )

    # equal

    test = ball.obj.dkg == 21
    TRICK_EXPECT_EQ( test , False, test_suite , "eq with integer" )

    test = ball.obj.dkg == 20.5
    TRICK_EXPECT_EQ( test , False, test_suite , "eq with float" )

    test = ball.obj.dkg == ball.obj.dlbm
    TRICK_EXPECT_EQ( test , False, test_suite , "eq with swig_int and unit conversion" )

    ball.obj.dkg = 20.1
    test = ball.obj.dkg == ball.obj.dkg
    TRICK_EXPECT_EQ( test , True, test_suite , "eq with swig_double and unit conversion" )

    # not equal

    test = ball.obj.dkg != 21
    TRICK_EXPECT_EQ( test , True, test_suite , "ne with integer" )

    test = ball.obj.dkg != 20.5
    TRICK_EXPECT_EQ( test , True, test_suite , "ne with float" )

    test = ball.obj.dkg != ball.obj.dlbm
    TRICK_EXPECT_EQ( test , True, test_suite , "ne with swig_int and unit conversion" )

    ball.obj.dkg = 20.1
    test = ball.obj.dkg != ball.obj.dkg
    TRICK_EXPECT_EQ( test , False, test_suite , "ne with swig_double and unit conversion" )

    # greater than

    ball.obj.dkg = 20
    ball.obj.dlbm = 20

    test = ball.obj.dkg > 21
    TRICK_EXPECT_EQ( test , False, test_suite , "gt with integer" )

    test = ball.obj.dkg > 20.5
    TRICK_EXPECT_EQ( test , False, test_suite , "gt with float" )

    test = ball.obj.dkg > ball.obj.dlbm
    TRICK_EXPECT_EQ( test , True, test_suite , "gt with swig_int and unit conversion" )

    ball.obj.dkg = 20.1
    test = ball.obj.dkg > ball.obj.dkg
    TRICK_EXPECT_EQ( test , False, test_suite , "gt with swig_double and unit conversion" )

    # greater than or equal

    ball.obj.dkg = 20
    ball.obj.dlbm = 20

    test = ball.obj.dkg >= 21
    TRICK_EXPECT_EQ( test , False, test_suite , "ge with integer" )

    test = ball.obj.dkg >= 20.5
    TRICK_EXPECT_EQ( test , False, test_suite , "ge with float" )

    test = ball.obj.dkg >= ball.obj.dlbm
    TRICK_EXPECT_EQ( test , True, test_suite , "ge with swig_int and unit conversion" )

    ball.obj.dkg = 20.1
    test = ball.obj.dkg >= ball.obj.dkg
    TRICK_EXPECT_EQ( test , True, test_suite , "ge with swig_double and unit conversion" )

    # unary operators

    ball.obj.dkg = 20
    test = -ball.obj.dkg
    TRICK_EXPECT_NEAR( test , -20, 0.0001, test_suite , "unary neg" )

    test = +ball.obj.dkg
    TRICK_EXPECT_NEAR( test , 20, 0.0001, test_suite , "unary pos" )

    ball.obj.dkg = -20
    test = abs(ball.obj.dkg)
    TRICK_EXPECT_NEAR( test , 20, 0.0001, test_suite , "unary abs" )

    # conversion
    ball.obj.dkg = 20
    test = int(ball.obj.dkg)
    TRICK_EXPECT_NEAR( test , 20, 0.0001, test_suite , "int" )

    ball.obj.dkg = 20
    test = long(ball.obj.dkg)
    TRICK_EXPECT_NEAR( test , 20, 0.0001, test_suite , "long" )

    ball.obj.dkg = 20
    test = float(ball.obj.dkg)
    TRICK_EXPECT_NEAR( test , 20, 0.0001, test_suite , "float" )

######################################################################################################################

    # Typedefed integers
    test_suite = "typedef"

    ball.obj.i = 40
    ball.obj.iii = ball.obj.i
    ball.obj.aiii = ball.obj.iii

    TRICK_EXPECT_EQ( ball.obj.iii , 40, test_suite , "integer from integer" )
    trick.trick_test_add_parent( test_suite , "integer from integer" , "1011083320")
    TRICK_EXPECT_EQ( ball.obj.aiii , 40, test_suite , "integer from another typedefed integer" )
    trick.trick_test_add_parent( test_suite , "integer from another typedefed integer" , "1011083320")

######################################################################################################################

    test_suite = "structure"

    # Structs with more than one name
    ball.t.i = 300
    TRICK_EXPECT_EQ( ball.t.i , 300, test_suite , "multi named structure no unit assignment" )

    ball.t.i = trick.attach_units("in", 300)
    TRICK_EXPECT_EQ( ball.t.i , 7, test_suite , "multi named structure with unit assignment" )

    ball.t.d = 300
    TRICK_EXPECT_NEAR( ball.t.d , 300, 0.0001 , test_suite , "multi named structure no unit assignment" )

    ball.t.d = trick.attach_units("in", 300)
    TRICK_EXPECT_NEAR( ball.t.d , 7.62 , 0.0001 , test_suite , "multi named structure with unit assignment" )

######################################################################################################################

    # Templates
    test_suite = "template"

    # simple template
    ball.obj.my_template_var.var1 = 30.0
    ball.obj.my_template_var.var2 = 40
    ball.obj.my_template_var.var3 = 50
    TRICK_EXPECT_NEAR( ball.obj.my_template_var.var1 , 30.0 , 0.000001 , test_suite , "double assignment" )
    TRICK_EXPECT_EQ( ball.obj.my_template_var.var2 , 40 , test_suite , "int assignment" )
    TRICK_EXPECT_EQ( ball.obj.my_template_var.var3 , 50 , test_suite , "short assignment" )
    trick.trick_test_add_parent( test_suite , "double assignment" , "2642836719")
    trick.trick_test_add_parent( test_suite , "int assignment" , "2642836719")
    trick.trick_test_add_parent( test_suite , "short assignment" , "2642836719")


    # using typedef from within template
    ball.obj.my_template_var_int = 66
    TRICK_EXPECT_EQ( ball.obj.my_template_var_int , 66, test_suite , "use typedef from within a template" )

    # a more convoluted template
    ball.obj.TTT_var.aa = 1000
    TRICK_EXPECT_EQ( ball.obj.TTT_var.aa , 1000, test_suite , "class complicated integer" )

    ball.obj.TTT_var.bb = 2000.0
    TRICK_EXPECT_NEAR( ball.obj.TTT_var.bb , 2000, 0.0001, test_suite , "class complicated double" )

######################################################################################################################

    # Namespace
    test_suite = "namespace"

    ball.ns_test.mass = trick.attach_units("lbm", 10)
    TRICK_EXPECT_NEAR( ball.ns_test.mass , 4.53592, 0.0001, test_suite , "Class variable with units" )
    trick.trick_test_add_parent( test_suite , "Class variable with units" , "2546878004")

    ball.ns_test.bbp = trick.alloc_type(2, "my_ns::BB")
    ball.ns_test.bbp[0].str = "hello"
    ball.ns_test.bbp[1].str = "there"
    temp = ball.ns_test.bbp[0].str + " " + ball.ns_test.bbp[1].str
    TRICK_EXPECT_EQ( str(temp) , "hello there", test_suite , "1D Class allocation" )
    trick.trick_test_add_parent( test_suite , "1D Class allocation" , "2546878004")

    ball.ns_test.bbpp = trick.alloc_type(4, "my_ns::BB *")
    ball.ns_test.bbpp[0] = trick.alloc_type(3, "my_ns::BB")
    ball.ns_test.bbpp[0][0].str = "bark"
    ball.ns_test.bbpp[0][1].str = "meow"
    ball.ns_test.bbpp[0][2].str = "quack"
    temp = ball.ns_test.bbpp[0][0].str + " " + ball.ns_test.bbpp[0][1].str + " " + ball.ns_test.bbpp[0][2].str
    TRICK_EXPECT_EQ( str(temp) , "bark meow quack", test_suite , "2D Class allocation" )
    trick.trick_test_add_parent( test_suite , "2D Class allocation" , "2546878004")

######################################################################################################################

    # Miscellaneous
    test_suite = "misc"

    ball.obj.d = 10
    ball.obj.ds = 15
    temp = ball.obj.d + float(ball.obj.ds)
    TRICK_EXPECT_NEAR( temp , 25 , 0.0001 , test_suite , "Forced remove units" )
    trick.trick_test_add_parent( test_suite , "Forced remove units" , "3339258059")

    temp = [ ball.obj.d , ball.obj.ds ]
    TRICK_EXPECT_EQ( str(temp) , "[10 kg, 15 s]", test_suite , "List with different objects" )

    ball.obj.dp = trick.get_address("ball.obj.d")
    TRICK_EXPECT_EQ( str(ball.obj.dp) , "[10 kg]", test_suite , "Get address" )

    ball.obj.d += 1
    TRICK_EXPECT_EQ( str(ball.obj.dp) , "[11 kg]", test_suite , "Get address verification" )

    ball.obj.da[2] = 45
    ball.obj.dp = trick.get_address("ball.obj.da[2]")
    TRICK_EXPECT_EQ( str(ball.obj.dp) , "[45 kg]", test_suite , "Get address mid-array" )

    ball.obj.dp = trick.get_address("ball.obj.daa[1][1]")
    TRICK_EXPECT_EQ( str(ball.obj.dp) , "[60 kg]", test_suite , "Get address multi-dimensional mid-array" )

    temp_array = ball.obj.daa[1]
    TRICK_EXPECT_EQ( str(temp_array) , "[50.1 kg, 60 kg, 52.3 kg]", test_suite , "Local variable reference to array" )

    # "const int & cir" and "int const & icr" are pointed to i in the Ball_alex constructor
    ball.obj.i = 55
    TRICK_EXPECT_EQ( ball.obj.cir , 55, test_suite , "Const reference" )
    TRICK_EXPECT_EQ( ball.obj.icr , 55, test_suite , "Const reference" )

    #ball.obj.cir = 99
    #TRICK_EXPECT_EQ( ball.obj.cir , 55, test_suite , "Const reference immutable test 1" )
    #TRICK_EXPECT_EQ( ball.obj.icr , 55, test_suite , "Const reference immutable test 1" )

    #ball.obj.icr = 98
    #TRICK_EXPECT_EQ( ball.obj.cir , 55, test_suite , "Const reference immutable test 2" )
    #TRICK_EXPECT_EQ( ball.obj.icr , 55, test_suite , "Const reference immutable test 2" )

    ball.obj.iiia = [ 300 , 400 , 500 , 600 , 700 ]
    TRICK_EXPECT_EQ( str(ball.obj.iiia) , "[300, 400, 500, 600, 700, 0]", test_suite , "Typedeffed integer type" )

    # scd = static const double, csd = const static double, sdc = static double const
    TRICK_EXPECT_NEAR( ball.obj.scd , 1.2345 , 0.0001 , test_suite , "Static const access" )
    TRICK_EXPECT_NEAR( ball.obj.csd , 6.7890 , 0.0001 , test_suite , "Static const access" )
    TRICK_EXPECT_NEAR( ball.obj.sdc , 9.8765 , 0.0001 , test_suite , "Static const access" )

    # Attempt to change a static const double
    #ball.obj.scd = 90.0 ;
    #TRICK_EXPECT_NEAR( ball.obj.scd , 1.2345 , 0.0001 , test_suite , "Static const immutable test 1" )
    #trick_mm.mmw.mm.read_checkpoint_from_string("ball.obj.scd = 2.2222 ;")
    #TRICK_EXPECT_NEAR( ball.obj.scd , 1.2345 , 0.0001 , test_suite , "Static const immutable test 2" )
    #ball.obj.csd = 90.0 ;
    #TRICK_EXPECT_NEAR( ball.obj.csd , 6.7890 , 0.0001 , test_suite , "Static const immutable test 3" )
    #trick_mm.mmw.mm.read_checkpoint_from_string("ball.obj.csd = 2.2222 ;")
    #TRICK_EXPECT_NEAR( ball.obj.csd , 6.7890 , 0.0001 , test_suite , "Static const immutable test 4" )
    #ball.obj.sdc = 90.0 ;
    #TRICK_EXPECT_NEAR( ball.obj.sdc , 9.8765 , 0.0001 , test_suite , "Static const immutable test 5" )
    #trick_mm.mmw.mm.read_checkpoint_from_string("ball.obj.sdc = 2.2222 ;")
    #TRICK_EXPECT_NEAR( ball.obj.sdc , 9.8765 , 0.0001 , test_suite , "Static const immutable test 6" )

    tester = trick.Test()
    output = tester.foo()
    TRICK_EXPECT_EQ( output , "called foo", test_suite , "Instantiate class and capture return value" )

    tester.t = trick.Test()
    output = tester.t.foo()
    TRICK_EXPECT_EQ( output , "called foo", test_suite , "Instantiate class pointer within class and capture return value" )

######################################################################################################################

    # Standard typedeffed integer types
    test_suite = "typedef_ints"

    ball.obj.i8t = 70 ;
    TRICK_EXPECT_EQ( ball.obj.i8t , 70, test_suite , "int8_t" )
    trick.trick_test_add_parent( test_suite , "int8_t" , "2939597198")

    ball.obj.ui8t = 71 ;
    TRICK_EXPECT_EQ( ball.obj.ui8t , 71, test_suite , "uint8_t" )

    ball.obj.i16t = 80 ;
    TRICK_EXPECT_EQ( ball.obj.i16t , 80, test_suite , "int16_t" )

    ball.obj.ui16t = 81 ;
    TRICK_EXPECT_EQ( ball.obj.ui16t , 81, test_suite , "uint16_t" )

    ball.obj.i32t = 90 ;
    TRICK_EXPECT_EQ( ball.obj.i32t , 90, test_suite , "int32_t" )

    ball.obj.ui32t = 91 ;
    TRICK_EXPECT_EQ( ball.obj.ui32t , 91, test_suite , "uint32_t" )

    ball.obj.i64t = 100 ;
    TRICK_EXPECT_EQ( ball.obj.i64t , 100, test_suite , "int64_t" )

    ball.obj.ui64t = 101 ;
    TRICK_EXPECT_EQ( ball.obj.ui64t , 101, test_suite , "uint64_t" )

    ball.obj.sizet = 111 ;
    TRICK_EXPECT_EQ( ball.obj.sizet , 111, test_suite , "size_t" )

    ball.obj.u_c = 121 ;
    TRICK_EXPECT_EQ( ball.obj.u_c , 121, test_suite , "u_char" )

    ball.obj.u_s = 131 ;
    TRICK_EXPECT_EQ( ball.obj.u_s , 131, test_suite , "u_short" )

    ball.obj.u_i = 141 ;
    TRICK_EXPECT_EQ( ball.obj.u_i , 141, test_suite , "u_int" )

    ball.obj.u_l = 151 ;
    TRICK_EXPECT_EQ( ball.obj.u_l , 151, test_suite , "u_long" )

    ball.obj.q = 161 ;
    TRICK_EXPECT_EQ( ball.obj.q , 161, test_suite , "quad_t" )

    ball.obj.uq = 171 ;
    TRICK_EXPECT_EQ( ball.obj.uq , 171, test_suite , "u_quad_t" )

######################################################################################################################

    # Exceptions

    test_suite = "exception"

    test_case = "Array index out of bounds"
    try:
        ball.obj.da[5] = 2.0
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

    test_case = "Double dimension array first index out of bounds"
    try:
        ball.obj.daa[20][0] = 2.0
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

    test_case = "Double dimension array second index out of bounds"
    try:
        ball.obj.daa[0][20] = 2.0
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

    test_case = "String too long"
    try:
        ball.obj.ca = "dfjdslfjdsajfldjalfjdslafjdlsajfdsd"
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

    test_case = "Units mismatch"
    try:
        ball.obj.da[2] = trick.attach_units("s" , 2.0)
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

    test_case = "unit-ed value assigned to unitless variable"
    try:
        ball.obj.dno_units = trick.attach_units("in" , 60.6)
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

    test_case = "unit-ed value assigned to unitless integer array variable"
    try:
        ball.obj.ia = trick.attach_units("in" , [60, 70])
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

    test_case = "unit-ed value assigned to unitless integer pointer variable"
    try:
        ball.obj.ip = trick.attach_units("in" , [60, 70])
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

######################################################################################################################

    # Polymorphic assignments and access
    test_suite = "polymorphism"

    ball.a = trick.Cat()
    TRICK_EXPECT_EQ( ball.a.id , 1, test_suite , "single abstract ptr" )
    trick.trick_test_add_parent( test_suite , "single abstract ptr" , "1770735610")
    #ball.a.speak()
    #ball.a[0].speak()
    ball.a = trick.Dog()
    TRICK_EXPECT_EQ( ball.a.id , 2, test_suite , "single abstract ptr" )

    ball.aarray[0] = trick.Cat()
    ball.aarray[1] = trick.Dog()
    ids = [ ball.aarray[0].id , ball.aarray[1].id ]
    TRICK_EXPECT_EQ( str(ids), "[1, 2]", test_suite , "fixed array of abstract ptrs" )
    #ball.aarray[0].speak()
    #ball.aarray[1].speak()

    ball.alist = trick.TMM_declare_var_1d("Abstract *", 4)

    ball.alist[0] = trick.TMM_declare_var_s("Cat")
    ball.alist[1] = trick.TMM_declare_var_s("Dog")
    ball.alist[2] = trick.Cat()
    ball.alist[3] = trick.Dog()
    ids = [ ball.alist[0].id , ball.alist[1].id , ball.alist[2].id , ball.alist[3].id ]
    TRICK_EXPECT_EQ( str(ids), "[1, 2, 1, 2]", test_suite , "fixed array of abstract ptrs" )
    #ball.alist[0].speak()
    #ball.alist[1].speak()
    #ball.alist[2].speak()
    #ball.alist[3].speak()

    # test vector of abstract pointers
    new_cat = trick.TMM_declare_var(trick.TRICK_STRUCTURED,"Cat",0,"my_cat",0,None)
    ball.vap.push_back(new_cat)
    TRICK_EXPECT_EQ( ball.vap[0].id , 1, test_suite , "std::vector of abstract ptrs" )

    new_dog = trick.TMM_declare_var(trick.TRICK_STRUCTURED,"Dog",0,"my_dog",0,None)
    ball.vap.push_back(new_dog)
    TRICK_EXPECT_EQ( ball.vap[1].id , 2, test_suite , "std::vector of abstract ptrs" )

    #ball.vap[0].speak()
    #ball.vap[1].speak()

    #drg0 = trick.DRAscii("cat_stuff")
    #drg0.add_variable("my_cat.id")
    #drg0.add_variable("my_dog.id")
    #drg0.set_cycle(0.1)
    #drg0.freq = trick.DR_Always
    #drg0.thisown = 0
    #trick.add_data_record_group(drg0, trick.DR_Buffer)

    ball.vap2.push_back(new_dog)
    ball.vap2.push_back(new_cat)

    # vector of vectors (experimental.  It does work!)
    ball.vvap.push_back(ball.vap)
    ball.vvap.push_back(ball.vap2)
    #ball.vvap[0][0].speak()
    #ball.vvap[0][1].speak()
    #ball.vvap[1][0].speak()
    #ball.vvap[1][1].speak()

######################################################################################################################

    test_suite = "array_sclicing"

    # fixed array
    ball.obj.ia = [ 10 , 20 , 30 ]
    TRICK_EXPECT_EQ( str(ball.obj.ia[:]), "[10, 20, 30]", test_suite , "full slice, fixed array" )
    TRICK_EXPECT_EQ( str(ball.obj.ia[1:]), "[20, 30]", test_suite , "slice with start value, fixed array" )
    TRICK_EXPECT_EQ( str(ball.obj.ia[:2]), "[10, 20]", test_suite , "slice with end value, fixed array" )
    TRICK_EXPECT_EQ( str(ball.obj.ia[::2]), "[10, 30]", test_suite , "sclice with step value, fixed array" )
    TRICK_EXPECT_EQ( str(ball.obj.ia[-3:-1]), "[10, 20]", test_suite , "slice with negative start and end value, fixed array" )
    TRICK_EXPECT_EQ( str(ball.obj.ia[::-2]), "[30, 10]", test_suite , "slice with negative step, fixed array" )

    ball.obj.ia = [ 10 , 20 , 30]
    ball.obj.ia[1:1] = 400
    TRICK_EXPECT_EQ( str(ball.obj.ia), "[10, 400, 20]", test_suite , "slice insertion with scalar value, fixed array" )

    ball.obj.ia = [ 10 , 20 , 30]
    ball.obj.ia[1:1] = [400 , 500]
    TRICK_EXPECT_EQ( str(ball.obj.ia), "[10, 400, 500]", test_suite , "slice insertion of list, fixed array" )

    ball.obj.ia = [ 10 , 20 , 30]
    ball.obj.ia[1:2] = 400
    TRICK_EXPECT_EQ( str(ball.obj.ia), "[10, 400, 30]", test_suite , "slice replacement with scalar value, fixed array" )

    ball.obj.ia = [ 10 , 20 , 30]
    ball.obj.ia[0:1] = [400 , 500]
    TRICK_EXPECT_EQ( str(ball.obj.ia), "[400, 500, 20]", test_suite , "slice replacement list larger than sclice, fixed array" )

    ball.obj.ia = [ 10 , 20 , 30]
    ball.obj.ia[0:2] = [400 , 500]
    TRICK_EXPECT_EQ( str(ball.obj.ia), "[400, 500, 30]", test_suite , "slice replacement list same size as sclice, fixed array" )

    ball.obj.ia = [ 10 , 20 , 30]
    ball.obj.ia[0:3] = [400 , 500]
    TRICK_EXPECT_EQ( str(ball.obj.ia), "[400, 500, 0]", test_suite , "slice replacement list smaller than slice, fixed array" )

    ball.obj.ia = [ 10 , 20 , 30]
    ball.obj.ia[:] = [400 , 500]
    TRICK_EXPECT_EQ( str(ball.obj.ia), "[400, 500, 0]", test_suite , "slice replacement full array, fixed array" )

    ball.obj.ia = [ 10 , 20 , 30]
    ball.obj.ia[0:2:2] = 400
    TRICK_EXPECT_EQ( str(ball.obj.ia), "[400, 20, 30]", test_suite , "slice replacement with scalar and step, fixed array" )

    ball.obj.ia = [ 10 , 20 , 30]
    ball.obj.ia[0:3:2] = [400 , 500]
    TRICK_EXPECT_EQ( str(ball.obj.ia), "[400, 20, 500]", test_suite , "slice replacement with list and step, fixed array" )

    # pointer
    ball.obj.ip = [ 10 , 20 , 30 , 40 ]
    TRICK_EXPECT_EQ( str(ball.obj.ip[:]), "[10, 20, 30, 40]", test_suite , "full slice, pointer" )
    TRICK_EXPECT_EQ( str(ball.obj.ip[1:]), "[20, 30, 40]", test_suite , "slice with start value, pointer" )
    TRICK_EXPECT_EQ( str(ball.obj.ip[:2]), "[10, 20]", test_suite , "slice with end value, pointer" )
    TRICK_EXPECT_EQ( str(ball.obj.ip[::2]), "[10, 30]", test_suite , "sclice with step value, pointer" )
    TRICK_EXPECT_EQ( str(ball.obj.ip[-3:-1]), "[20, 30]", test_suite , "slice with negative start and end value, pointer" )
    TRICK_EXPECT_EQ( str(ball.obj.ip[::-2]), "[40, 20]", test_suite , "slice with negative step, pointer" )

    ball.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    ball.obj.ip[1:1] = 400
    TRICK_EXPECT_EQ( str(ball.obj.ip), "[10, 400, 20, 30, 40]", test_suite , "slice insertion with scalar value, pointer" )

    ball.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    ball.obj.ip[1:1] = [400 , 500]
    TRICK_EXPECT_EQ( str(ball.obj.ip), "[10, 400, 500, 20, 30]", test_suite , "slice insertion of list, pointer" )

    ball.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    ball.obj.ip[1:2] = 400
    TRICK_EXPECT_EQ( str(ball.obj.ip), "[10, 400, 30, 40, 50]", test_suite , "slice replacement with scalar value, pointer" )

    ball.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    ball.obj.ip[1:2] = [400 , 500]
    TRICK_EXPECT_EQ( str(ball.obj.ip), "[10, 400, 500, 30, 40]", test_suite , "slice replacement list larger than sclice, pointer" )

    ball.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    ball.obj.ip[1:3] = [400 , 500]
    TRICK_EXPECT_EQ( str(ball.obj.ip), "[10, 400, 500, 40, 50]", test_suite , "slice replacement list same size as sclice, pointer" )

    ball.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    ball.obj.ip[1:4] = [400 , 500]
    TRICK_EXPECT_EQ( str(ball.obj.ip), "[10, 400, 500, 50, 0]", test_suite , "slice replacement list smaller than slice, pointer" )

    ball.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    ball.obj.ip[:] = [400 , 500]
    TRICK_EXPECT_EQ( str(ball.obj.ip), "[400, 500, 0, 0, 0]", test_suite , "slice replacement full array, pointer" )

    ball.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    ball.obj.ip[1:3:2] = 400
    TRICK_EXPECT_EQ( str(ball.obj.ip), "[10, 400, 30, 40, 50]", test_suite , "slice replacement with scalar and step, pointer" )

    ball.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    ball.obj.ip[1:4:2] = [400 , 500]
    TRICK_EXPECT_EQ( str(ball.obj.ip), "[10, 400, 30, 500, 50]", test_suite , "slice replacement with list and step, pointer" )

    ball.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    ball.obj.ip[1:5:2] = [400 , 500]
    TRICK_EXPECT_EQ( str(ball.obj.ip), "[10, 400, 30, 500, 50]", test_suite , "slice replacement with list and step, pointer" )

######################################################################################################################

    test_suite = "STL list support"

    #print dir(ball.obj.ls)

    TRICK_EXPECT_EQ( ball.obj.ls.empty(), 1, test_suite , "STL list empty true" )
    ball.obj.ls.push_back('string 1')
    ball.obj.ls.push_front('string 2')
    ball.obj.ls.push_back('string 3')
    TRICK_EXPECT_EQ( ball.obj.ls.empty(), 0, test_suite , "STL list empty false" )
    TRICK_EXPECT_EQ( ball.obj.ls.front(), "string 2", test_suite , "STL list front access" )
    TRICK_EXPECT_EQ( ball.obj.ls.back(), "string 3", test_suite , "STL list back access" )
    TRICK_EXPECT_EQ( ball.obj.ls.size(), 3, test_suite , "STL list size command" )

    #ball.obj.ls.insert(ball.obj.ls.begin(), 'string 4')
    #ball.obj.ls.pop_front()
    #ball.obj.ls.erase(ball.obj.ls.begin())
    #for l in ball.obj.ls:
    #   print l

######################################################################################################################

    test_suite = "STL map support"

    TRICK_EXPECT_EQ( ball.obj.msi.empty(), 1, test_suite , "STL map empty true" )

    ball.obj.msi['key1'] = 50
    ball.obj.msi['key2'] = 60
    ball.obj.msi['key3'] = 70

    TRICK_EXPECT_EQ( ball.obj.msi.empty(), 0, test_suite , "STL map empty false" )
    TRICK_EXPECT_EQ( ball.obj.msi['key1'], 50, test_suite , "STL map key/data insertion/access" )
    TRICK_EXPECT_EQ( str(ball.obj.msi.keys()), "['key1', 'key2', 'key3']", test_suite , "STL map keys command" )
    TRICK_EXPECT_EQ( str(ball.obj.msi.values()), "[50, 60, 70]", test_suite , "STL map values command" )
    TRICK_EXPECT_EQ( ball.obj.msi.has_key('key1'), 1, test_suite , "STL map has_key true" )
    TRICK_EXPECT_EQ( ball.obj.msi.has_key('key4'), 0, test_suite , "STL map has_key false" )
    TRICK_EXPECT_EQ( ball.obj.msi.size(), 3, test_suite , "STL map size command" )
    #print dict(ball.obj.msi)

######################################################################################################################

    test_suite = "Templated SimObject"

    TRICK_EXPECT_EQ( tso.t, 25, test_suite , "templated sim_object access member" )
    TRICK_EXPECT_EQ( iftso.t, 25, test_suite , "inherit from templated sim_object access member" )

######################################################################################################################

    test_suite = "Templated SimObject"
    TRICK_EXPECT_TRUE( ball.test_true(), test_suite , "boolean function return" )
    TRICK_EXPECT_FALSE( ball.test_false(), test_suite , "boolean function return" )

######################################################################################################################

if __name__ == "__main__":
    main()


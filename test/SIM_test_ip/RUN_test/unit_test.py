
import math
from trick.unit_test import *

def main():

    # These are here as a reference for the add_collect syntax... I have changed the code to not use collect
    # An example of removing collect in the input file (original collect added in S_define file)
    #test_so.obj.state.work.external_force = trick.delete_collect(test_so.obj.state.work.external_force, test_so.obj.force.output.force)
    # An example of adding a collect in the input file
    #test_so.obj.state.work.external_force = trick.add_collect(test_so.obj.state.work.external_force, test_so.obj.force.output.force)

    # An example of turning off a sim_object
    trick.exec_set_sim_object_onoff("disabled_obj" , False)

    trick.exec_set_terminate_time(1.0)

    trick_utest.unit_tests.enable()
    trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_ip.xml" )
    trick_utest.unit_tests.set_test_name( "IPtest" )

######################################################################################################################

    test_suite = "double"

    test_so.obj.d = 2
    TRICK_EXPECT_NEAR( test_so.obj.d , 2.0 , 0.000001 , test_suite , "no units" )
    trick.trick_test_add_parent( test_suite , "no units" , "910635102")

    test_so.obj.d = trick.attach_units("lb" , 2)
    TRICK_EXPECT_NEAR( test_so.obj.d , 0.907185 , 0.000001 , test_suite , "units convert" )

    test_so.obj.da = [ 20 , 21 , 22 ]
    TRICK_EXPECT_NEAR( test_so.obj.da[0] , 20 , 0.000001 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_NEAR( test_so.obj.da[1] , 21 , 0.000001 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_NEAR( test_so.obj.da[2] , 22 , 0.000001 , test_suite , "1D array, integer value, no units" )

    test_so.obj.da = [ 30.1 , 31.1 , 32.1 ]

    TRICK_EXPECT_NEAR( test_so.obj.da[0] , 30.1 , 0.000001 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_NEAR( test_so.obj.da[1] , 31.1 , 0.000001 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_NEAR( test_so.obj.da[2] , 32.1 , 0.000001 , test_suite , "1D array, float value, no units" )

    test_so.obj.da = 40.1 , 41.1 , 42.1

    TRICK_EXPECT_NEAR( test_so.obj.da[0] , 40.1 , 0.000001 , test_suite , "1D array, tuple float value, no units" )
    TRICK_EXPECT_NEAR( test_so.obj.da[1] , 41.1 , 0.000001 , test_suite , "1D array, tuple float value, no units" )
    TRICK_EXPECT_NEAR( test_so.obj.da[2] , 42.1 , 0.000001 , test_suite , "1D array, tuple float value, no units" )

    test_so.obj.da = trick.attach_units("lb" , [2 , 3 , 4])
    TRICK_EXPECT_NEAR( test_so.obj.da[0] , 0.907185 , 0.000001 , test_suite , "1D array, float value, units convert" )
    TRICK_EXPECT_NEAR( test_so.obj.da[1] , 1.36078 ,  0.00001  , test_suite , "1D array, float value, units convert" )
    TRICK_EXPECT_NEAR( test_so.obj.da[2] , 1.81437 ,  0.00001  , test_suite , "1D array, float value, units convert" )

    test_so.obj.dp = trick.TMM_declare_var_s("double[6]")
    TRICK_EXPECT_NEAR( test_so.obj.dp[0] , 0 ,  0.00001  , test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_NEAR( test_so.obj.dp[5] , 0 ,  0.00001  , test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(test_so.obj.dp) , "[0 kg, 0 kg, 0 kg, 0 kg, 0 kg, 0 kg]", test_suite , "1D ptr, allocation" )

    test_so.obj.dp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(test_so.obj.dp) , "[30 kg, 31 kg, 32 kg, 33 kg]", test_suite , "1D ptr, list assign, no units" )

    test_so.obj.dp[2] = 62
    TRICK_EXPECT_EQ( str(test_so.obj.dp) , "[30 kg, 31 kg, 62 kg, 33 kg]", test_suite , "1D ptr, partial assign, no units" )

    TRICK_EXPECT_EQ( test_so.obj.dp[-1], 33, test_suite , "negative index integer value" )
    TRICK_EXPECT_EQ( test_so.obj.dp[-1.0], 33, test_suite , "negative index float value" )

    test_so.obj.dp[-1] = 55
    test_so.obj.dp[-2] = 54
    TRICK_EXPECT_EQ( str(test_so.obj.dp) , "[30 kg, 31 kg, 54 kg, 55 kg]", test_suite , "negative index assignments" )

    test_so.obj.dp = None
    TRICK_EXPECT_EQ( str(test_so.obj.dp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    # Mixed tuple/list notation
    test_so.obj.daa = trick.attach_units( "kg", (( 50 , 51 , 52) , [53, 54, 55]) )
    TRICK_EXPECT_EQ( str(test_so.obj.daa) , "[[50 kg, 51 kg, 52 kg],[53 kg, 54 kg, 55 kg]]",
     test_suite , "2D array, full assign, no units" )

    test_so.obj.daa = trick.attach_units( "kg", [[ 40 , 41 , 42] , [43, 44, 45]] )
    TRICK_EXPECT_EQ( str(test_so.obj.daa) , "[[40 kg, 41 kg, 42 kg],[43 kg, 44 kg, 45 kg]]",
     test_suite , "2D array, full assign, no units" )

    test_so.obj.daa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(test_so.obj.daa) , "[[40 kg, 41 kg, 42 kg],[50 kg, 51 kg, 52 kg]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.daa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(test_so.obj.daa) , "[[40 kg, 41 kg, 42 kg],[50.1 kg, 51.2 kg, 52.3 kg]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.daa[1][1] =  60
    TRICK_EXPECT_EQ( str(test_so.obj.daa) , "[[40 kg, 41 kg, 42 kg],[50.1 kg, 60 kg, 52.3 kg]]",
     test_suite , "2D array, single assign, no units" )

    test_so.obj.daa[0] = trick.attach_units( "lb",[ 4.0, 5.0, 6.0])
    TRICK_EXPECT_EQ( str(test_so.obj.daa[0]) , "[1.81436948 kg, 2.26796185 kg, 2.72155422 kg]",
     test_suite , "2D array, single single row assignment with units conversion" )

    TRICK_EXPECT_EQ( str(test_so.obj.dap) , "[NULL, NULL, NULL, NULL]", test_suite , "2D array of ptr, initial value" )

    test_so.obj.dap[0] = trick.TMM_declare_var_1d( "double", 3)
    test_so.obj.dap[1] = trick.TMM_declare_var_1d( "double", 4)
    test_so.obj.dap[2] = trick.TMM_declare_var_1d( "double", 5)
    test_so.obj.dap[3] = trick.TMM_declare_var_1d( "double", 6)

    TRICK_EXPECT_EQ( str(test_so.obj.dap[0]) , "[0 kg, 0 kg, 0 kg]", test_suite , "2D array of ptr, single row access" )

    test_so.obj.dap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(test_so.obj.dap[3]) , "[60 kg, 61 kg, 62 kg, 63 kg]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    test_so.obj.dap[3][1] = 75
    test_so.obj.dap[3][3] = trick.attach_units("lb", float(test_so.obj.dap[3][3]) + 1.0)

    TRICK_EXPECT_EQ( str(test_so.obj.dap[3]) , "[60 kg, 75 kg, 62 kg, 29.02991168 kg]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    test_so.obj.dpp = trick.TMM_declare_var_s("double *[4]")
    TRICK_EXPECT_EQ( str(test_so.obj.dpp) , "[NULL, NULL, NULL, NULL]", test_suite , "2D ptr of ptr, initial value" )

    test_so.obj.dpp[0] = trick.TMM_declare_var_1d( "double", 5)
    TRICK_EXPECT_EQ( str(test_so.obj.dpp[0]) , "[0 kg, 0 kg, 0 kg, 0 kg, 0 kg]", test_suite , "2D ptr of ptr, allocate 1 row" )

    test_so.obj.dpp[0][1] = 85
    TRICK_EXPECT_EQ( str(test_so.obj.dpp[0]) , "[0 kg, 85 kg, 0 kg, 0 kg, 0 kg]",
     test_suite , "2D ptr of ptr, scalar assignment" )

    test_so.obj.dpp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(test_so.obj.dpp[1]) , "[91 kg, 92 kg, 93 kg]",
     test_suite , "2D ptr of ptr, row assignment" )

    test_so.obj.dpp[2] = trick.attach_units("lb" , [ 91 , 92 , 93 , 94 , 95])
    TRICK_EXPECT_NEAR( test_so.obj.dpp[2][0] , 41.276905 , 0.000001 ,
     test_suite , "2D ptr of ptr, united value" )

    test_so.obj.dpp = None
    TRICK_EXPECT_EQ( str(test_so.obj.dpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

    test_so.obj.daaa[0][0] = [1, 2, 3, 4]
    test_so.obj.daaa[0][1] = [5, 6, 7, 8]
    test_so.obj.daaa[0][2][0] = 9
    test_so.obj.daaa[0][2][1] = 10
    test_so.obj.daaa[0][2][2] = 11
    test_so.obj.daaa[0][2][3] = 12
    # 2D assignment fails with error message but does not exit sim. :(
    #test_so.obj.daaa[1][0] = [[101, 102, 103, 104] , [105, 106, 107, 108] , [109, 110, 111, 112]]
    TRICK_EXPECT_EQ( str(test_so.obj.daaa[0]) , "[[1 kg, 2 kg, 3 kg, 4 kg],[5 kg, 6 kg, 7 kg, 8 kg],[9 kg, 10 kg, 11 kg, 12 kg]]",
     test_suite , "3D array, list and scalar assignment" )

    # 4D assignment array is not supported yet
    #test_so.obj.daaaa[0][0][0] = [51, 52, 53, 54, 55]

######################################################################################################################

    test_suite = "float"

    test_so.obj.f = 2
    TRICK_EXPECT_NEAR( test_so.obj.f , 2.0 , 0.000001 , test_suite , "no units" )
    trick.trick_test_add_parent( test_suite , "no units" , "1532242077")

    test_so.obj.f = trick.attach_units("lb" , 2)
    TRICK_EXPECT_NEAR( test_so.obj.f , 0.907185 , 0.000001 , test_suite , "units convert" )

    test_so.obj.fa = [ 20 , 21 , 22 ]
    TRICK_EXPECT_NEAR( test_so.obj.fa[0] , 20 , 0.000001 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_NEAR( test_so.obj.fa[1] , 21 , 0.000001 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_NEAR( test_so.obj.fa[2] , 22 , 0.000001 , test_suite , "1D array, integer value, no units" )

    test_so.obj.fa = [ 30.1 , 31.1 , 32.1 ]

    TRICK_EXPECT_NEAR( test_so.obj.fa[0] , 30.1 , 0.0001 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_NEAR( test_so.obj.fa[1] , 31.1 , 0.0001 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_NEAR( test_so.obj.fa[2] , 32.1 , 0.0001 , test_suite , "1D array, float value, no units" )

    test_so.obj.fa = trick.attach_units("lb" , [2 , 3 , 4])
    TRICK_EXPECT_NEAR( test_so.obj.fa[0] , 0.907185 , 0.000001 , test_suite , "1D array, float value, units convert" )
    TRICK_EXPECT_NEAR( test_so.obj.fa[1] , 1.36078 ,  0.00001  , test_suite , "1D array, float value, units convert" )
    TRICK_EXPECT_NEAR( test_so.obj.fa[2] , 1.81437 ,  0.00001  , test_suite , "1D array, float value, units convert" )

    test_so.obj.fp = trick.alloc_type( 6 , "float")
    TRICK_EXPECT_NEAR( test_so.obj.fp[0] , 0 ,  0.00001  , test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_NEAR( test_so.obj.fp[5] , 0 ,  0.00001  , test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(test_so.obj.fp) , "[0 kg, 0 kg, 0 kg, 0 kg, 0 kg, 0 kg]", test_suite , "1D ptr, allocation" )

    test_so.obj.fp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(test_so.obj.fp) , "[30 kg, 31 kg, 32 kg, 33 kg]", test_suite , "1D ptr, list assign, no units" )

    test_so.obj.fp[2] = 62
    TRICK_EXPECT_EQ( str(test_so.obj.fp) , "[30 kg, 31 kg, 62 kg, 33 kg]", test_suite , "1D ptr, partial assign, no units" )

    test_so.obj.fp = None
    TRICK_EXPECT_EQ( str(test_so.obj.fp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    test_so.obj.faa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(test_so.obj.faa) , "[[40 kg, 41 kg, 42 kg],[43 kg, 44 kg, 45 kg]]",
     test_suite , "2D array, full assign, no units" )

    test_so.obj.faa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(test_so.obj.faa) , "[[40 kg, 41 kg, 42 kg],[50 kg, 51 kg, 52 kg]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.faa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(test_so.obj.faa) , "[[40 kg, 41 kg, 42 kg],[50.099998 kg, 51.200001 kg, 52.299999 kg]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.faa[1][1] =  60
    TRICK_EXPECT_EQ( str(test_so.obj.faa) , "[[40 kg, 41 kg, 42 kg],[50.099998 kg, 60 kg, 52.299999 kg]]",
     test_suite , "2D array, partial assign, no units" )

    TRICK_EXPECT_EQ( str(test_so.obj.fap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    test_so.obj.fap[0] = trick.alloc_type( 3 , "float")
    test_so.obj.fap[1] = trick.alloc_type( 4 , "float")
    test_so.obj.fap[2] = trick.alloc_type( 5 , "float")
    test_so.obj.fap[3] = trick.alloc_type( 6 , "float")
    TRICK_EXPECT_EQ( str(test_so.obj.fap[0]) , "[0 kg, 0 kg, 0 kg]",
     test_suite , "2D array of ptr, single row access" )

    test_so.obj.fap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(test_so.obj.fap[3]) , "[60 kg, 61 kg, 62 kg, 63 kg]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    test_so.obj.fap[3][1] = 75
    test_so.obj.fap[3][3] = trick.attach_units("lb", float(test_so.obj.fap[3][3]) + 1.0)

    TRICK_EXPECT_EQ( str(test_so.obj.fap[3]) , "[60 kg, 75 kg, 62 kg, 29.029911 kg]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    test_so.obj.fpp = trick.alloc_type( 4 , "float *")
    TRICK_EXPECT_EQ( str(test_so.obj.fpp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    test_so.obj.fpp[0] = trick.alloc_type( 5 , "float")
    TRICK_EXPECT_EQ( str(test_so.obj.fpp[0]) , "[0 kg, 0 kg, 0 kg, 0 kg, 0 kg]", test_suite , "2D ptr of ptr, allocate 1 row" )

    test_so.obj.fpp[0][1] = 85
    TRICK_EXPECT_EQ( str(test_so.obj.fpp[0]) , "[0 kg, 85 kg, 0 kg, 0 kg, 0 kg]", test_suite , "2D ptr of ptr, assign 1 value" )

    test_so.obj.fpp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(test_so.obj.fpp[1]) , "[91 kg, 92 kg, 93 kg]", test_suite , "2D ptr of ptr, row assignment" )

    test_so.obj.fpp = None
    TRICK_EXPECT_EQ( str(test_so.obj.fpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

    test_so.obj.f_rad = 2.0
    TRICK_EXPECT_NEAR( test_so.obj.f_rad , 2.0 , 0.000001 , test_suite , "no units" )

    test_so.obj.f_rad = trick.attach_units("degree" , 45.0)
    TRICK_EXPECT_NEAR( test_so.obj.f_rad , 0.785398 , 0.000001 , test_suite , "unit conv" )

    test_so.obj.d_deg = test_so.obj.f_rad
    TRICK_EXPECT_NEAR( test_so.obj.d_deg , 45.0 , 0.00001 , test_suite , "value to value assign with conversion" )

######################################################################################################################

    test_suite = "char"

    test_so.obj.c = 'g'
    TRICK_EXPECT_EQ( str(test_so.obj.c) , "103", test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "2896569040")

    test_so.obj.c = 123
    TRICK_EXPECT_EQ( str(test_so.obj.c) , "123", test_suite , "assignment" )

    test_so.obj.ca = "Trick is great"
    TRICK_EXPECT_EQ( str(test_so.obj.ca) , "Trick is great", test_suite , "arrray assignment" )

    test_so.obj.ca = [65, 66, 67, 68, 69]
    TRICK_EXPECT_EQ( str(test_so.obj.ca) , "ABCDE", test_suite , "arrray assignment" )

    test_so.obj.ca = [65.1, 66.2, 67.3, 68.4, 69.6]
    TRICK_EXPECT_EQ( str(test_so.obj.ca) , "ABCDE", test_suite , "arrray assignment" )
    TRICK_EXPECT_EQ( test_so.obj.ca[3] , 68 , test_suite , "arrray assignment" )

    test_so.obj.cp = trick.alloc_type( 6 , "char")
    TRICK_EXPECT_EQ( test_so.obj.cp[0] , 0 , test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( test_so.obj.cp[5] , 0 , test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(test_so.obj.cp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    test_so.obj.cp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(test_so.obj.cp) , "[30, 31, ' ', '!']", test_suite , "float 1D ptr, list assign, no units" )

    test_so.obj.cp[2] = 62
    TRICK_EXPECT_EQ( str(test_so.obj.cp) , "[30, 31, '>', '!']", test_suite , "float 1D ptr, partial assign, no units" )


    test_so.obj.cp = "testing"
    TRICK_EXPECT_EQ( str(test_so.obj.cp) , "testing", test_suite , "ptr assignment" )

    test_so.obj.cp = None
    TRICK_EXPECT_EQ( str(test_so.obj.cp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    test_so.obj.caa = [ "abcdefg" , "hijklmn" ]
    TRICK_EXPECT_EQ( str(test_so.obj.caa) , "[\"abcdefg\",\"hijklmn\",[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]]", test_suite , "2D array string assignment" )
    TRICK_EXPECT_EQ( str(test_so.obj.caa[1]) , "hijklmn", test_suite , "2D array item access" )
    TRICK_EXPECT_EQ( test_so.obj.caa[1][4] , 108, test_suite , "2D array single char access" )

    TRICK_EXPECT_EQ( str(test_so.obj.cap) , """[NULL, NULL, NULL, NULL]""", test_suite , "2D array of ptr initial value" )

    test_so.obj.cap[0] = "cap0"
    test_so.obj.cap[1] = "cap1"
    test_so.obj.cap[2] = "cap2"
    test_so.obj.cap[3] = "cap3"
    TRICK_EXPECT_EQ( str(test_so.obj.cap) , "[\"cap0\", \"cap1\", \"cap2\", \"cap3\"]", test_suite , "2D array of ptr single item assignment" )
    TRICK_EXPECT_EQ( str(test_so.obj.cap[0]) , "cap0", test_suite , "2D array of ptr single item assignment" )

    TRICK_EXPECT_EQ( test_so.obj.cap[3][2] , 112 , test_suite , "2D array of ptr single item assignment" )

    test_so.obj.cpp = trick.alloc_type( 4 , "char *")
    TRICK_EXPECT_EQ( str(test_so.obj.cpp) , """[NULL, NULL, NULL, NULL]""", test_suite , "2D ptr of ptr initial value" )

    test_so.obj.cpp[0] = "cpp_string_0"
    test_so.obj.cpp[1] = "cpp_string_1"
    test_so.obj.cpp[2] = "cpp_string_2"
    test_so.obj.cpp[3] = "cpp_string_3"
    TRICK_EXPECT_EQ( str(test_so.obj.cpp[2]) , "cpp_string_2", test_suite , "2D ptr of ptr single string access" )
    TRICK_EXPECT_EQ( test_so.obj.cpp[2][3] , 95 , test_suite , "2D ptr of ptr single character access" )

    test_so.obj.cpp = None
    TRICK_EXPECT_EQ( str(test_so.obj.cpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "unsigned char"

    test_so.obj.uc = 95
    TRICK_EXPECT_EQ( test_so.obj.uc , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "219444977")

    test_so.obj.uc += 1
    TRICK_EXPECT_EQ( test_so.obj.uc , 96 , test_suite , "increment" )

    test_so.obj.uc = test_so.obj.uc + 1
    TRICK_EXPECT_EQ( test_so.obj.uc , 97 , test_suite , "increment" )

    test_so.obj.uc = 1 + test_so.obj.uc
    TRICK_EXPECT_EQ( test_so.obj.uc , 98 , test_suite , "increment" )

    test_so.obj.uca = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( test_so.obj.uca[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.uca[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.uca[2] , 22 , test_suite , "1D array, integer value, no units" )

    test_so.obj.uca = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( test_so.obj.uca[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.uca[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.uca[2] , 32 , test_suite , "1D array, float value, no units" )

    test_so.obj.ucp = trick.alloc_type( 6 , "unsigned char")
    TRICK_EXPECT_EQ( test_so.obj.ucp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( test_so.obj.ucp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(test_so.obj.ucp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    test_so.obj.ucp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ucp) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    test_so.obj.ucp[2] = 62
    TRICK_EXPECT_EQ( str(test_so.obj.ucp) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    test_so.obj.ucp = None
    TRICK_EXPECT_EQ( str(test_so.obj.ucp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    test_so.obj.ucaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(test_so.obj.ucaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    test_so.obj.ucaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(test_so.obj.ucaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.ucaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ucaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.ucaa[1][1] =  60
    TRICK_EXPECT_EQ( str(test_so.obj.ucaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(test_so.obj.ucap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    test_so.obj.ucap[0] = trick.alloc_type( 3 , "unsigned char")
    test_so.obj.ucap[1] = trick.alloc_type( 4 , "unsigned char")
    test_so.obj.ucap[2] = trick.alloc_type( 5 , "unsigned char")
    test_so.obj.ucap[3] = trick.alloc_type( 6 , "unsigned char")

    TRICK_EXPECT_EQ( str(test_so.obj.ucap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    test_so.obj.ucap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ucap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    test_so.obj.ucap[3][1] = 75
    test_so.obj.ucap[3][3] = trick.attach_units("--", int(test_so.obj.ucap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(test_so.obj.ucap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    test_so.obj.ucpp = trick.alloc_type( 4 , "unsigned char *")
    TRICK_EXPECT_EQ( str(test_so.obj.ucpp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    test_so.obj.ucpp[0] = trick.alloc_type( 5 , "unsigned char")
    TRICK_EXPECT_EQ( str(test_so.obj.ucpp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    test_so.obj.ucpp[0][1] = 85
    TRICK_EXPECT_EQ( str(test_so.obj.ucpp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    test_so.obj.ucpp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ucpp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    test_so.obj.ucpp = None
    TRICK_EXPECT_EQ( str(test_so.obj.ucpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "short"

    test_so.obj.s = 95
    TRICK_EXPECT_EQ( test_so.obj.s , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "2880907803")

    test_so.obj.s += 1
    TRICK_EXPECT_EQ( test_so.obj.s , 96 , test_suite , "increment" )

    test_so.obj.s = test_so.obj.s + 1
    TRICK_EXPECT_EQ( test_so.obj.s , 97 , test_suite , "increment" )

    test_so.obj.s = 1 + test_so.obj.s
    TRICK_EXPECT_EQ( test_so.obj.s , 98 , test_suite , "increment" )

    test_so.obj.sa = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( test_so.obj.sa[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.sa[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.sa[2] , 22 , test_suite , "1D array, integer value, no units" )

    test_so.obj.sa = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( test_so.obj.sa[0] , 30 , test_suite , "short 1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.sa[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.sa[2] , 32 , test_suite , "1D array, float value, no units" )

    test_so.obj.sp = trick.alloc_type( 6 , "short")
    TRICK_EXPECT_EQ( test_so.obj.sp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( test_so.obj.sp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(test_so.obj.sp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    test_so.obj.sp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(test_so.obj.sp) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    test_so.obj.sp[2] = 62
    TRICK_EXPECT_EQ( str(test_so.obj.sp) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    test_so.obj.sp = None
    TRICK_EXPECT_EQ( str(test_so.obj.sp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    test_so.obj.saa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(test_so.obj.saa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    test_so.obj.saa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(test_so.obj.saa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.saa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(test_so.obj.saa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.saa[1][1] =  60
    TRICK_EXPECT_EQ( str(test_so.obj.saa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(test_so.obj.sap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    test_so.obj.sap[0] = trick.alloc_type( 3 , "short")
    test_so.obj.sap[1] = trick.alloc_type( 4 , "short")
    test_so.obj.sap[2] = trick.alloc_type( 5 , "short")
    test_so.obj.sap[3] = trick.alloc_type( 6 , "short")

    TRICK_EXPECT_EQ( str(test_so.obj.sap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    test_so.obj.sap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(test_so.obj.sap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    test_so.obj.sap[3][1] = 75
    test_so.obj.sap[3][3] = trick.attach_units("--", int(test_so.obj.sap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(test_so.obj.sap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    test_so.obj.spp = trick.alloc_type( 4 , "short *")
    TRICK_EXPECT_EQ( str(test_so.obj.spp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    test_so.obj.spp[0] = trick.alloc_type( 5 , "short")
    TRICK_EXPECT_EQ( str(test_so.obj.spp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    test_so.obj.spp[0][1] = 85
    TRICK_EXPECT_EQ( str(test_so.obj.spp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    test_so.obj.spp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(test_so.obj.spp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    test_so.obj.spp = None
    TRICK_EXPECT_EQ( str(test_so.obj.spp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "unsigned short"

    test_so.obj.us = 95
    TRICK_EXPECT_EQ( test_so.obj.us , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "217750348")

    test_so.obj.us += 1
    TRICK_EXPECT_EQ( test_so.obj.us , 96 , test_suite , "increment" )

    test_so.obj.us = test_so.obj.us + 1
    TRICK_EXPECT_EQ( test_so.obj.us , 97 , test_suite , "increment" )

    test_so.obj.us = 1 + test_so.obj.us
    TRICK_EXPECT_EQ( test_so.obj.us , 98 , test_suite , "increment" )

    test_so.obj.usa = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( test_so.obj.usa[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.usa[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.usa[2] , 22 , test_suite , "1D array, integer value, no units" )

    test_so.obj.usa = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( test_so.obj.usa[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.usa[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.usa[2] , 32 , test_suite , "1D array, float value, no units" )

    test_so.obj.usp = trick.alloc_type( 6 , "unsigned short")
    TRICK_EXPECT_EQ( test_so.obj.usp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( test_so.obj.usp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(test_so.obj.usp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    test_so.obj.usp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(test_so.obj.usp) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    test_so.obj.usp[2] = 62
    TRICK_EXPECT_EQ( str(test_so.obj.usp) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    test_so.obj.usp = None
    TRICK_EXPECT_EQ( str(test_so.obj.usp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    test_so.obj.usaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(test_so.obj.usaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    test_so.obj.usaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(test_so.obj.usaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.usaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(test_so.obj.usaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.usaa[1][1] =  60
    TRICK_EXPECT_EQ( str(test_so.obj.usaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(test_so.obj.usap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    test_so.obj.usap[0] = trick.alloc_type( 3 , "unsigned short")
    test_so.obj.usap[1] = trick.alloc_type( 4 , "unsigned short")
    test_so.obj.usap[2] = trick.alloc_type( 5 , "unsigned short")
    test_so.obj.usap[3] = trick.alloc_type( 6 , "unsigned short")

    TRICK_EXPECT_EQ( str(test_so.obj.usap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    test_so.obj.usap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(test_so.obj.usap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    test_so.obj.usap[3][1] = 75
    test_so.obj.usap[3][3] = trick.attach_units("--", int(test_so.obj.usap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(test_so.obj.usap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    test_so.obj.uspp = trick.alloc_type( 4 , "unsigned short *")
    TRICK_EXPECT_EQ( str(test_so.obj.uspp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    test_so.obj.uspp[0] = trick.alloc_type( 5 , "unsigned short")
    TRICK_EXPECT_EQ( str(test_so.obj.uspp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    test_so.obj.uspp[0][1] = 85
    TRICK_EXPECT_EQ( str(test_so.obj.uspp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    test_so.obj.uspp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(test_so.obj.uspp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    test_so.obj.uspp = None
    TRICK_EXPECT_EQ( str(test_so.obj.uspp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "enum"

    test_so.obj.e = trick.THIRD
    TRICK_EXPECT_EQ( test_so.obj.e , trick.THIRD , test_suite , "scalar, integer value, no units" )
    trick.trick_test_add_parent( test_suite , "scalar, integer value, no units" , "3331066868")

    test_so.obj.ea = [ trick.THIRD , trick.SECOND , trick.FIRST ]
    TRICK_EXPECT_EQ( test_so.obj.ea[0] , trick.THIRD , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ea[1] , trick.SECOND , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ea[2] , trick.FIRST , test_suite , "1D array, integer value, no units" )

    test_so.obj.ea[1] = trick.THIRD
    test_so.obj.ea[2] = trick.SECOND
    TRICK_EXPECT_EQ( test_so.obj.ea[1] , trick.THIRD , test_suite , "1D array single value assignment" )
    TRICK_EXPECT_EQ( test_so.obj.ea[2] , trick.SECOND , test_suite , "1D array single value assignment" )

    test_so.obj.ep = trick.alloc_type( 6 , "MY_ENUM")
    TRICK_EXPECT_EQ( test_so.obj.ep[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( test_so.obj.ep[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(test_so.obj.ep) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    test_so.obj.ep = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ep) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    test_so.obj.ep[2] = 62
    TRICK_EXPECT_EQ( str(test_so.obj.ep) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    test_so.obj.ep = None
    TRICK_EXPECT_EQ( str(test_so.obj.ep) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    test_so.obj.eaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(test_so.obj.eaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    test_so.obj.eaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(test_so.obj.eaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.eaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(test_so.obj.eaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.eaa[1][1] =  60
    TRICK_EXPECT_EQ( str(test_so.obj.eaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(test_so.obj.eap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    test_so.obj.eap[0] = trick.alloc_type( 3 , "MY_ENUM")
    test_so.obj.eap[1] = trick.alloc_type( 4 , "MY_ENUM")
    test_so.obj.eap[2] = trick.alloc_type( 5 , "MY_ENUM")
    test_so.obj.eap[3] = trick.alloc_type( 6 , "MY_ENUM")

    TRICK_EXPECT_EQ( str(test_so.obj.eap[0]) , "[0, 0, 0]", test_suite , "2D array of ptr, single row access" )

    test_so.obj.eap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(test_so.obj.eap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    test_so.obj.eap[3][1] = 75
    test_so.obj.eap[3][3] = trick.attach_units("--", int(test_so.obj.eap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(test_so.obj.eap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    test_so.obj.epp = trick.alloc_type( 4 , "MY_ENUM *")
    TRICK_EXPECT_EQ( str(test_so.obj.epp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "int 2D ptr of ptr, initial value" )

    test_so.obj.epp[0] = trick.alloc_type( 5 , "MY_ENUM")
    TRICK_EXPECT_EQ( str(test_so.obj.epp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    test_so.obj.epp[0][1] = 85
    TRICK_EXPECT_EQ( str(test_so.obj.epp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    test_so.obj.epp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(test_so.obj.epp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    test_so.obj.epp = None
    TRICK_EXPECT_EQ( str(test_so.obj.epp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "int"

    test_so.obj.i = 95
    TRICK_EXPECT_EQ( test_so.obj.i , 95 , test_suite , "assignment" )
    #print "test_so.obj.i = " , test_so.obj.i
    trick.trick_test_add_parent( test_suite , "assignment" , "3074788233")

    test_so.obj.i += 1
    TRICK_EXPECT_EQ( test_so.obj.i , 96 , test_suite , "increment" )

    test_so.obj.i = test_so.obj.i + 1
    TRICK_EXPECT_EQ( test_so.obj.i , 97 , test_suite , "increment" )

    test_so.obj.i = 1 + test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.i , 98 , test_suite , "increment" )

    test_so.obj.ia = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( test_so.obj.ia[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ia[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ia[2] , 22 , test_suite , "1D array, integer value, no units" )

    test_so.obj.ia = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( test_so.obj.ia[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ia[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ia[2] , 32 , test_suite , "1D array, float value, no units" )

    test_so.obj.ia = trick.attach_units("--" , [60, 70])
    TRICK_EXPECT_EQ( test_so.obj.ia[0] , 60 , test_suite , "1D array, -- units" )
    TRICK_EXPECT_EQ( test_so.obj.ia[1] , 70 , test_suite , "1D array, -- units" )

    test_so.obj.ip = trick.alloc_type( 6 , "int")
    TRICK_EXPECT_EQ( test_so.obj.ip[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( test_so.obj.ip[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(test_so.obj.ip) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    test_so.obj.ip = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ip) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    test_so.obj.ip[2] = 62
    TRICK_EXPECT_EQ( str(test_so.obj.ip) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    test_so.obj.ip = trick.attach_units("--" , [60, 70])
    TRICK_EXPECT_EQ( str(test_so.obj.ip) , "[60, 70]", test_suite , "1D ptr, assign list -- unit-ed values" )

    test_so.obj.ip = None
    TRICK_EXPECT_EQ( str(test_so.obj.ip) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    test_so.obj.iaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(test_so.obj.iaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    test_so.obj.iaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(test_so.obj.iaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.iaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(test_so.obj.iaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.iaa[1][1] =  60
    TRICK_EXPECT_EQ( str(test_so.obj.iaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(test_so.obj.iap) , "[NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    test_so.obj.iap[0] = trick.alloc_type( 3 , "int")
    test_so.obj.iap[1] = trick.alloc_type( 4 , "int")
    test_so.obj.iap[2] = trick.alloc_type( 5 , "int")
    test_so.obj.iap[3] = trick.alloc_type( 6 , "int")
    TRICK_EXPECT_EQ( str(test_so.obj.iap[0]) , "[0, 0, 0]", test_suite , "2D array of ptr, single row access" )

    test_so.obj.iap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(test_so.obj.iap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    test_so.obj.iap[3][1] = 75
    test_so.obj.iap[3][3] = trick.attach_units("--", int(test_so.obj.iap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(test_so.obj.iap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    test_so.obj.ipp = trick.alloc_type( 4 , "int *")
    TRICK_EXPECT_EQ( str(test_so.obj.ipp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    test_so.obj.ipp[0] = trick.alloc_type( 5 , "int")
    TRICK_EXPECT_EQ( str(test_so.obj.ipp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    test_so.obj.ipp[0][1] = 85
    TRICK_EXPECT_EQ( str(test_so.obj.ipp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    test_so.obj.ipp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ipp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    test_so.obj.ipp = None
    TRICK_EXPECT_EQ( str(test_so.obj.ipp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "unsigned int"

    test_so.obj.ui = 95
    TRICK_EXPECT_EQ( test_so.obj.ui , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "1873736978")

    test_so.obj.ui += 1
    TRICK_EXPECT_EQ( test_so.obj.ui , 96 , test_suite , "increment" )

    test_so.obj.ui = test_so.obj.ui + 1
    TRICK_EXPECT_EQ( test_so.obj.ui , 97 , test_suite , "increment" )

    test_so.obj.ui = 1 + test_so.obj.ui
    TRICK_EXPECT_EQ( test_so.obj.ui , 98 , test_suite , "increment" )

    test_so.obj.uia = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( test_so.obj.uia[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.uia[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.uia[2] , 22 , test_suite , "1D array, integer value, no units" )

    test_so.obj.uia = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( test_so.obj.uia[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.uia[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.uia[2] , 32 , test_suite , "1D array, float value, no units" )

    test_so.obj.uip = trick.alloc_type( 6 , "unsigned int")
    TRICK_EXPECT_EQ( test_so.obj.uip[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( test_so.obj.uip[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(test_so.obj.uip) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    test_so.obj.uip = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(test_so.obj.uip) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    test_so.obj.uip[2] = 62
    TRICK_EXPECT_EQ( str(test_so.obj.uip) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    test_so.obj.uip = None
    TRICK_EXPECT_EQ( str(test_so.obj.uip) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    test_so.obj.uiaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(test_so.obj.uiaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    test_so.obj.uiaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(test_so.obj.uiaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.uiaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(test_so.obj.uiaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.uiaa[1][1] =  60
    TRICK_EXPECT_EQ( str(test_so.obj.uiaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(test_so.obj.uiap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    test_so.obj.uiap[0] = trick.alloc_type( 3 , "unsigned int")
    test_so.obj.uiap[1] = trick.alloc_type( 4 , "unsigned int")
    test_so.obj.uiap[2] = trick.alloc_type( 5 , "unsigned int")
    test_so.obj.uiap[3] = trick.alloc_type( 6 , "unsigned int")

    TRICK_EXPECT_EQ( str(test_so.obj.uiap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    test_so.obj.uiap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(test_so.obj.uiap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    test_so.obj.uiap[3][1] = 75
    test_so.obj.uiap[3][3] = trick.attach_units("--", int(test_so.obj.uiap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(test_so.obj.uiap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    test_so.obj.uipp = trick.alloc_type( 4 , "unsigned int *")
    TRICK_EXPECT_EQ( str(test_so.obj.uipp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    test_so.obj.uipp[0] = trick.alloc_type( 5 , "unsigned int")
    TRICK_EXPECT_EQ( str(test_so.obj.uipp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    test_so.obj.uipp[0][1] = 85
    TRICK_EXPECT_EQ( str(test_so.obj.uipp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    test_so.obj.uipp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(test_so.obj.uipp[1]) , "[91, 92, 93]", test_suite , "unsigned int 2D ptr of ptr, row assignment" )

    test_so.obj.uipp = None
    TRICK_EXPECT_EQ( str(test_so.obj.uipp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "long"

    test_so.obj.l = 95
    TRICK_EXPECT_EQ( test_so.obj.l , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "3338288463")

    test_so.obj.l += 1
    TRICK_EXPECT_EQ( test_so.obj.l , 96 , test_suite , "increment" )

    test_so.obj.l = test_so.obj.l + 1
    TRICK_EXPECT_EQ( test_so.obj.l , 97 , test_suite , "increment" )

    test_so.obj.l = 1 + test_so.obj.l
    TRICK_EXPECT_EQ( test_so.obj.l , 98 , test_suite , "increment" )

    test_so.obj.la = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( test_so.obj.la[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.la[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.la[2] , 22 , test_suite , "1D array, integer value, no units" )

    test_so.obj.la = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( test_so.obj.la[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.la[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.la[2] , 32 , test_suite , "1D array, float value, no units" )

    test_so.obj.lp = trick.alloc_type( 6 , "long")
    TRICK_EXPECT_EQ( test_so.obj.lp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( test_so.obj.lp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(test_so.obj.lp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    test_so.obj.lp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(test_so.obj.lp) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    test_so.obj.lp[2] = 62
    TRICK_EXPECT_EQ( str(test_so.obj.lp) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    test_so.obj.lp = None
    TRICK_EXPECT_EQ( str(test_so.obj.lp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    test_so.obj.laa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(test_so.obj.laa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    test_so.obj.laa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(test_so.obj.laa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.laa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(test_so.obj.laa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.laa[1][1] =  60
    TRICK_EXPECT_EQ( str(test_so.obj.laa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(test_so.obj.lap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    test_so.obj.lap[0] = trick.alloc_type( 3 , "long")
    test_so.obj.lap[1] = trick.alloc_type( 4 , "long")
    test_so.obj.lap[2] = trick.alloc_type( 5 , "long")
    test_so.obj.lap[3] = trick.alloc_type( 6 , "long")

    TRICK_EXPECT_EQ( str(test_so.obj.lap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    test_so.obj.lap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(test_so.obj.lap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    test_so.obj.lap[3][1] = 75
    test_so.obj.lap[3][3] = trick.attach_units("--", int(test_so.obj.lap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(test_so.obj.lap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    test_so.obj.lpp = trick.alloc_type( 4 , "long *")
    TRICK_EXPECT_EQ( str(test_so.obj.lpp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    test_so.obj.lpp[0] = trick.alloc_type( 5 , "long")
    TRICK_EXPECT_EQ( str(test_so.obj.lpp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    test_so.obj.lpp[0][1] = 85
    TRICK_EXPECT_EQ( str(test_so.obj.lpp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    test_so.obj.lpp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(test_so.obj.lpp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    test_so.obj.lpp = None
    TRICK_EXPECT_EQ( str(test_so.obj.lpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "unsinged long"

    test_so.obj.ul = 95
    TRICK_EXPECT_EQ( test_so.obj.ul , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "2844151852")

    test_so.obj.ul += 1
    TRICK_EXPECT_EQ( test_so.obj.ul , 96 , test_suite , "increment" )

    test_so.obj.ul = test_so.obj.ul + 1
    TRICK_EXPECT_EQ( test_so.obj.ul , 97 , test_suite , "increment" )

    test_so.obj.ul = 1 + test_so.obj.ul
    TRICK_EXPECT_EQ( test_so.obj.ul , 98 , test_suite , "increment" )

    test_so.obj.ula = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( test_so.obj.ula[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ula[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ula[2] , 22 , test_suite , "1D array, integer value, no units" )

    test_so.obj.ula = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( test_so.obj.ula[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ula[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ula[2] , 32 , test_suite , "1D array, float value, no units" )

    test_so.obj.ulp = trick.alloc_type( 6 , "unsigned long")
    TRICK_EXPECT_EQ( test_so.obj.ulp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( test_so.obj.ulp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(test_so.obj.ulp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    test_so.obj.ulp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ulp) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    test_so.obj.ulp[2] = 62
    TRICK_EXPECT_EQ( str(test_so.obj.ulp) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    test_so.obj.ulp = None
    TRICK_EXPECT_EQ( str(test_so.obj.ulp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    test_so.obj.ulaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(test_so.obj.ulaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    test_so.obj.ulaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(test_so.obj.ulaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.ulaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ulaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.ulaa[1][1] =  60
    TRICK_EXPECT_EQ( str(test_so.obj.ulaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(test_so.obj.ulap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    test_so.obj.ulap[0] = trick.alloc_type( 3 , "unsigned long")
    test_so.obj.ulap[1] = trick.alloc_type( 4 , "unsigned long")
    test_so.obj.ulap[2] = trick.alloc_type( 5 , "unsigned long")
    test_so.obj.ulap[3] = trick.alloc_type( 6 , "unsigned long")

    TRICK_EXPECT_EQ( str(test_so.obj.ulap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    test_so.obj.ulap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ulap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    test_so.obj.ulap[3][1] = 75
    test_so.obj.ulap[3][3] = trick.attach_units("--", int(test_so.obj.ulap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(test_so.obj.ulap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    test_so.obj.ulpp = trick.alloc_type( 4 , "unsigned long *")
    TRICK_EXPECT_EQ( str(test_so.obj.ulpp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    test_so.obj.ulpp[0] = trick.alloc_type( 5 , "unsigned long")
    TRICK_EXPECT_EQ( str(test_so.obj.ulpp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    test_so.obj.ulpp[0][1] = 85
    TRICK_EXPECT_EQ( str(test_so.obj.ulpp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    test_so.obj.ulpp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ulpp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    test_so.obj.ulpp = None
    TRICK_EXPECT_EQ( str(test_so.obj.ulpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "long long"

    test_so.obj.ll = 95
    TRICK_EXPECT_EQ( test_so.obj.ll , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "2165977787")

    test_so.obj.ll += 1
    TRICK_EXPECT_EQ( test_so.obj.ll , 96 , test_suite , "increment" )

    test_so.obj.ll = test_so.obj.ll + 1
    TRICK_EXPECT_EQ( test_so.obj.ll , 97 , test_suite , "increment" )

    test_so.obj.ll = 1 + test_so.obj.ll
    TRICK_EXPECT_EQ( test_so.obj.ll , 98 , test_suite , "increment" )

    test_so.obj.lla = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( test_so.obj.lla[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.lla[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.lla[2] , 22 , test_suite , "1D array, integer value, no units" )

    test_so.obj.lla = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( test_so.obj.lla[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.lla[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.lla[2] , 32 , test_suite , "1D array, float value, no units" )

    test_so.obj.llp = trick.alloc_type( 6 , "long long")
    TRICK_EXPECT_EQ( test_so.obj.llp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( test_so.obj.llp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(test_so.obj.llp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    test_so.obj.llp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(test_so.obj.llp) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    test_so.obj.llp[2] = 62
    TRICK_EXPECT_EQ( str(test_so.obj.llp) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    test_so.obj.llp = None
    TRICK_EXPECT_EQ( str(test_so.obj.llp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    test_so.obj.llaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(test_so.obj.llaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    test_so.obj.llaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(test_so.obj.llaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.llaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(test_so.obj.llaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.llaa[1][1] =  60
    TRICK_EXPECT_EQ( str(test_so.obj.llaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(test_so.obj.llap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    test_so.obj.llap[0] = trick.alloc_type( 3 , "long long")
    test_so.obj.llap[1] = trick.alloc_type( 4 , "long long")
    test_so.obj.llap[2] = trick.alloc_type( 5 , "long long")
    test_so.obj.llap[3] = trick.alloc_type( 6 , "long long")

    TRICK_EXPECT_EQ( str(test_so.obj.llap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    test_so.obj.llap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(test_so.obj.llap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    test_so.obj.llap[3][1] = 75
    test_so.obj.llap[3][3] = trick.attach_units("--", int(test_so.obj.llap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(test_so.obj.llap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    test_so.obj.llpp = trick.alloc_type( 4 , "long long *")
    TRICK_EXPECT_EQ( str(test_so.obj.llpp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    test_so.obj.llpp[0] = trick.alloc_type( 5 , "long long")
    TRICK_EXPECT_EQ( str(test_so.obj.llpp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    test_so.obj.llpp[0][1] = 85
    TRICK_EXPECT_EQ( str(test_so.obj.llpp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    test_so.obj.llpp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(test_so.obj.llpp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    test_so.obj.llpp = None
    TRICK_EXPECT_EQ( str(test_so.obj.llpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "unsigned long long"

    test_so.obj.ull = 95
    TRICK_EXPECT_EQ( test_so.obj.ull , 95 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "3783821020")	

    test_so.obj.ull += 1
    TRICK_EXPECT_EQ( test_so.obj.ull , 96 , test_suite , "increment" )

    test_so.obj.ull = test_so.obj.ull + 1
    TRICK_EXPECT_EQ( test_so.obj.ull , 97 , test_suite , "increment" )

    test_so.obj.ull = 1 + test_so.obj.ull
    TRICK_EXPECT_EQ( test_so.obj.ull , 98 , test_suite , "increment" )

    test_so.obj.ulla = [ 20 , 21 , 22 ]
    TRICK_EXPECT_EQ( test_so.obj.ulla[0] , 20 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ulla[1] , 21 , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ulla[2] , 22 , test_suite , "1D array, integer value, no units" )

    test_so.obj.ulla = [ 30.1 , 31.1 , 32.1 ]
    TRICK_EXPECT_EQ( test_so.obj.ulla[0] , 30 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ulla[1] , 31 , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ulla[2] , 32 , test_suite , "1D array, float value, no units" )

    test_so.obj.ullp = trick.alloc_type( 6 , "unsigned long long")
    TRICK_EXPECT_EQ( test_so.obj.ullp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( test_so.obj.ullp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(test_so.obj.ullp) , "[0, 0, 0, 0, 0, 0]", test_suite , "1D ptr, allocation" )

    test_so.obj.ullp = [ 30 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ullp) , "[30, 31, 32, 33]", test_suite , "1D ptr, list assign, no units" )

    test_so.obj.ullp[2] = 62
    TRICK_EXPECT_EQ( str(test_so.obj.ullp) , "[30, 31, 62, 33]", test_suite , "1D ptr, partial assign, no units" )

    test_so.obj.ullp = None
    TRICK_EXPECT_EQ( str(test_so.obj.ullp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    test_so.obj.ullaa = [[ 40 , 41 , 42] , [43, 44, 45]]
    TRICK_EXPECT_EQ( str(test_so.obj.ullaa) , "[[40, 41, 42],[43, 44, 45]]",
     test_suite , "2D array, full assign, no units" )

    test_so.obj.ullaa[1] = [ 50 , 51 , 52]
    TRICK_EXPECT_EQ( str(test_so.obj.ullaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.ullaa[1] = [ 50.1 , 51.2 , 52.3 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ullaa) , "[[40, 41, 42],[50, 51, 52]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.ullaa[1][1] =  60
    TRICK_EXPECT_EQ( str(test_so.obj.ullaa) , "[[40, 41, 42],[50, 60, 52]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(test_so.obj.ullap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    test_so.obj.ullap[0] = trick.alloc_type( 3 , "unsigned long long")
    test_so.obj.ullap[1] = trick.alloc_type( 4 , "unsigned long long")
    test_so.obj.ullap[2] = trick.alloc_type( 5 , "unsigned long long")
    test_so.obj.ullap[3] = trick.alloc_type( 6 , "unsigned long long")

    TRICK_EXPECT_EQ( str(test_so.obj.ullap[0]) , "[0, 0, 0]",
     test_suite , "2D array of ptr, single row access" )

    test_so.obj.ullap[3] = [ 60 , 61 , 62, 63 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ullap[3]) , "[60, 61, 62, 63]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    test_so.obj.ullap[3][1] = 75
    test_so.obj.ullap[3][3] = trick.attach_units("--", int(test_so.obj.ullap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(test_so.obj.ullap[3]) , "[60, 75, 62, 64]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    test_so.obj.ullpp = trick.alloc_type( 4 , "unsigned long long *")
    TRICK_EXPECT_EQ( str(test_so.obj.ullpp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    test_so.obj.ullpp[0] = trick.alloc_type( 5 , "unsigned long long")
    TRICK_EXPECT_EQ( str(test_so.obj.ullpp[0]) , "[0, 0, 0, 0, 0]", test_suite , "2D ptr of ptr, allocate 1 row" )

    test_so.obj.ullpp[0][1] = 85
    TRICK_EXPECT_EQ( str(test_so.obj.ullpp[0]) , "[0, 85, 0, 0, 0]", test_suite , "2D ptr of ptr, assign 1 value" )

    test_so.obj.ullpp[1] = [ 91 , 92 , 93 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ullpp[1]) , "[91, 92, 93]", test_suite , "2D ptr of ptr, row assignment" )

    test_so.obj.ullpp = None
    TRICK_EXPECT_EQ( str(test_so.obj.ullpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "bool"

    test_so.obj.b = True
    TRICK_EXPECT_EQ( test_so.obj.b , True , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "4134211307")

    test_so.obj.b += 1
    TRICK_EXPECT_EQ( test_so.obj.b , True , test_suite , "increment" )

    test_so.obj.b = test_so.obj.b + 1
    TRICK_EXPECT_EQ( test_so.obj.b , True , test_suite , "increment" )

    test_so.obj.ull = 1 + test_so.obj.ull
    TRICK_EXPECT_EQ( test_so.obj.b , True , test_suite , "increment" )

    test_so.obj.ba = [ False , True , True ]
    TRICK_EXPECT_EQ( test_so.obj.ba[0] , False , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ba[1] , True , test_suite , "1D array, integer value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ba[2] , True , test_suite , "1D array, integer value, no units" )

    test_so.obj.ba = [ 2.2 , 1.1 , 0 ]
    TRICK_EXPECT_EQ( test_so.obj.ba[0] , True , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ba[1] , True , test_suite , "1D array, float value, no units" )
    TRICK_EXPECT_EQ( test_so.obj.ba[2] , False , test_suite , "1D array, float value, no units" )

    test_so.obj.bp = trick.alloc_type( 6 , "bool" )
    TRICK_EXPECT_EQ( test_so.obj.bp[0] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( test_so.obj.bp[5] , 0 ,  test_suite , "1D ptr, allocation" )
    TRICK_EXPECT_EQ( str(test_so.obj.bp) , "[False, False, False, False, False, False]", test_suite , "1D ptr, allocation" )

    test_so.obj.bp = [ 0 , 31 , 32 , 33 ]
    TRICK_EXPECT_EQ( str(test_so.obj.bp) , "[False, True, True, True]", test_suite , "1D ptr, list assign, no units" )

    test_so.obj.bp[2] = 0
    TRICK_EXPECT_EQ( str(test_so.obj.bp) , "[False, True, False, True]", test_suite , "1D ptr, partial assign, no units" )

    test_so.obj.bp = None
    TRICK_EXPECT_EQ( str(test_so.obj.bp) , "NULL", test_suite , "1D ptr None (NULL) assignment" )

    test_so.obj.baa = [[ 0 , 1 , 2] , [3, 4, 0]]
    TRICK_EXPECT_EQ( str(test_so.obj.baa) , "[[False, True, True],[True, True, False]]",
     test_suite , "2D array, full assign, no units" )

    test_so.obj.baa[1] = [ False , False , True]
    TRICK_EXPECT_EQ( str(test_so.obj.baa) , "[[False, True, True],[False, False, True]]",
     test_suite , "2D array, partial assign, no units" )

    test_so.obj.baa[1][1] = True
    TRICK_EXPECT_EQ( str(test_so.obj.baa) , "[[False, True, True],[False, True, True]]",
     test_suite , "2D array, single assign, no units" )

    TRICK_EXPECT_EQ( str(test_so.obj.bap) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D array of ptr, initial value" )

    test_so.obj.bap[0] = trick.alloc_type( 3 , "bool")
    test_so.obj.bap[1] = trick.alloc_type( 4 , "bool")
    test_so.obj.bap[2] = trick.alloc_type( 5 , "bool")
    test_so.obj.bap[3] = trick.alloc_type( 6 , "bool")

    TRICK_EXPECT_EQ( str(test_so.obj.bap[0]) , "[False, False, False]",
     test_suite , "2D array of ptr, single row access" )

    test_so.obj.bap[3] = [ True , False , True, False ]
    TRICK_EXPECT_EQ( str(test_so.obj.bap[3]) , "[True, False, True, False]",
     test_suite , "2D array of ptr, single row realloc and assignment" )

    test_so.obj.bap[3][1] = 75
    test_so.obj.bap[3][3] = trick.attach_units("--", int(test_so.obj.bap[3][3]) + 1)

    TRICK_EXPECT_EQ( str(test_so.obj.bap[3]) , "[True, True, True, True]",
     test_suite , "2D array of ptr, single item assignment with unit conversion" )

    test_so.obj.bpp = trick.alloc_type( 4 , "bool *")
    TRICK_EXPECT_EQ( str(test_so.obj.bpp) , "[NULL, NULL, NULL, NULL]",
     test_suite , "2D ptr of ptr, initial value" )

    test_so.obj.bpp[0] = trick.alloc_type( 5 , "bool")
    TRICK_EXPECT_EQ( str(test_so.obj.bpp[0]) , "[False, False, False, False, False]", test_suite , "2D ptr of ptr, allocate 1 row" )

    test_so.obj.bpp[0][1] = 85
    TRICK_EXPECT_EQ( str(test_so.obj.bpp[0]) , "[False, True, False, False, False]", test_suite , "2D ptr of ptr, assign 1 value" )

    test_so.obj.bpp[1] = [ True , False , True ]
    TRICK_EXPECT_EQ( str(test_so.obj.bpp[1]) , "[True, False, True]", test_suite , "2D ptr of ptr, row assignment" )

    test_so.obj.bpp = None
    TRICK_EXPECT_EQ( str(test_so.obj.bpp) , "NULL", test_suite , "2D ptr None (NULL) assignment" )

######################################################################################################################

    test_suite = "structure"

    test_so.obj.cana[0].ii = 250
    test_so.obj.cana[0].jj = 350
    test_so.obj.cana[1].ii = 260
    test_so.obj.cana[1].jj = 360
    TRICK_EXPECT_EQ( test_so.obj.cana[0].ii , 250 , test_suite , "1D array access" )
    TRICK_EXPECT_EQ( test_so.obj.cana[0].jj , 350 , test_suite , "1D array access" )
    TRICK_EXPECT_EQ( test_so.obj.cana[1].ii , 260 , test_suite , "1D array access" )
    TRICK_EXPECT_EQ( test_so.obj.cana[1].jj , 360 , test_suite , "1D array access" )
    trick.trick_test_add_parent( test_suite , "1D array access" , "2797105872")

    test_so.obj.can.ii = 150
    test_so.obj.can.jj = 160
    test_so.obj.can2 = test_so.obj.can
    TRICK_EXPECT_EQ( test_so.obj.can2.ii , 150 , test_suite , "copy" )
    TRICK_EXPECT_EQ( test_so.obj.can2.jj , 160 , test_suite , "copy" )

    #test_so.obj.cana[3] = test_so.obj.can

    test_so.obj.canp = test_so.obj.can
    TRICK_EXPECT_EQ( test_so.obj.canp.ii , 150 , test_suite , "pointer assignment" )
    TRICK_EXPECT_EQ( test_so.obj.canp.jj , 160 , test_suite , "pointer assignment" )

    test_so.obj.canp = test_so.obj.cana[1]
    TRICK_EXPECT_EQ( test_so.obj.canp.ii , 260 , test_suite , "pointer to array element assignment" )
    TRICK_EXPECT_EQ( test_so.obj.canp.jj , 360 , test_suite , "pointer to array element assignment" )

    test_so.obj.canp = trick.alloc_type( 2 , "CanCopy" )
    test_so.obj.canp[0].ii = 400
    test_so.obj.canp[0].jj = 500
    test_so.obj.canp[1].ii = 600
    test_so.obj.canp[1].jj = 700
    TRICK_EXPECT_EQ( test_so.obj.canp[0].ii , 400 , test_suite , "pointer to array element assignment" )
    TRICK_EXPECT_EQ( test_so.obj.canp[0].jj , 500 , test_suite , "pointer to array element assignment" )
    TRICK_EXPECT_EQ( test_so.obj.canp[1].ii , 600 , test_suite , "pointer to array element assignment" )
    TRICK_EXPECT_EQ( test_so.obj.canp[1].jj , 700 , test_suite , "pointer to array element assignment" )
    #print test_so.obj.canp[0]
    #print test_so.obj.canp[1]

    test_so.obj.canp = None
    TRICK_EXPECT_EQ( str(test_so.obj.canp) , "None", test_suite , "1D ptr None (NULL) assignment" )

    # silently ignored! need to figure out how to flag this
    #test_so.obj.cannot = test_so.obj.cannot2

######################################################################################################################

    test_suite = "string"

    test_so.obj.str = "hello"
    TRICK_EXPECT_EQ( test_so.obj.str , "hello" , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "165635378")

    #TODO: make a std::string typemap to allow assignment of string from char *
    #test_so.obj.str = test_so.obj.cap[1]
    #TRICK_EXPECT_EQ( test_so.obj.str , "cap1" , "IPtest" , "string assignment from char *" )
    #print "test_so.obj.str = " , test_so.obj.str

######################################################################################################################

    test_suite = "bitfield"

    test_so.obj.bit_0 = 7
    TRICK_EXPECT_EQ( test_so.obj.bit_0 , 7 , test_suite , "assignment" )
    trick.trick_test_add_parent( test_suite , "assignment" , "1649805110")

    test_so.obj.bit_1 = 17
    TRICK_EXPECT_EQ( test_so.obj.bit_1 , -15 , test_suite , "assignment with overflow" )

    test_so.obj.boolbit_0 = True
    TRICK_EXPECT_EQ( test_so.obj.boolbit_0 , True , test_suite , "bool assignment" )

    test_so.obj.boolbit_1 = 2
    TRICK_EXPECT_EQ( test_so.obj.boolbit_1 , True , test_suite , "bool assignment with overflow" )

######################################################################################################################

    test_suite = "union"

    test_so.obj.ut.i = 20
    TRICK_EXPECT_EQ( str(test_so.obj.ut.i) , "20", test_suite , "test 1" )
    trick.trick_test_add_parent( test_suite , "test 1" , "3095148896")

    test_so.obj.ut2.i = 30
    TRICK_EXPECT_EQ( str(test_so.obj.ut2.i) , "30", test_suite , "test 2" )
    trick.trick_test_add_parent( test_suite , "test 2" , "3095148896")

######################################################################################################################

    # swig_int from swig_double
    test_suite = "swig_int"

    test_so.obj.dlbm = 50
    test_so.obj.ilbm = test_so.obj.dlbm
    TRICK_EXPECT_EQ( test_so.obj.ilbm , 50, test_suite , "assignment from swig_double" )
    trick.trick_test_add_parent( test_suite , "assignment from swig_double" , "2901141151")

    # addition

    test_so.obj.ilbm = 50
    test_so.obj.ikg = test_so.obj.ilbm
    TRICK_EXPECT_EQ( test_so.obj.ikg , 22, test_suite , "units conversion" )

    test_so.obj.ikg = test_so.obj.ilbm + 20
    TRICK_EXPECT_EQ( test_so.obj.ikg , 31, test_suite , "addition with integer" )

    test_so.obj.ikg = test_so.obj.ilbm + 20.9
    TRICK_EXPECT_EQ( test_so.obj.ikg , 32, test_suite , "addition with float" )

    test_so.obj.ikg = test_so.obj.ilbm + test_so.obj.ilbm
    TRICK_EXPECT_EQ( test_so.obj.ikg , 45, test_suite , "addition with swig_int" )

    test_so.obj.ikg = 50
    test_so.obj.ikg = test_so.obj.ilbm + test_so.obj.ikg
    TRICK_EXPECT_EQ( test_so.obj.ikg , 72, test_suite , "addition with swig_int and unit conversion" )

    test_so.obj.dlbm = 10
    test_so.obj.ikg = test_so.obj.ilbm + test_so.obj.dlbm
    TRICK_EXPECT_EQ( test_so.obj.ikg , 27, test_suite , "addition with swig_double" )

    test_so.obj.dkg = 10
    test_so.obj.ikg = test_so.obj.ilbm + test_so.obj.dkg
    TRICK_EXPECT_EQ( test_so.obj.ikg , 32, test_suite , "addition with swig_double and unit conversion" )

    # subtraction

    test_so.obj.ikg = test_so.obj.ilbm - 20
    TRICK_EXPECT_EQ( test_so.obj.ikg , 13, test_suite , "subtraction with integer" )

    test_so.obj.ikg = test_so.obj.ilbm - 20.9
    TRICK_EXPECT_EQ( test_so.obj.ikg , 13, test_suite , "subtraction with float" )

    test_so.obj.ikg = test_so.obj.ilbm - test_so.obj.ilbm
    TRICK_EXPECT_EQ( test_so.obj.ikg , 0, test_suite , "subtraction with swig_int" )

    test_so.obj.ikg = 10
    test_so.obj.ikg = test_so.obj.ilbm - test_so.obj.ikg
    TRICK_EXPECT_EQ( test_so.obj.ikg , 12, test_suite , "subtraction with swig_int and unit conversion" )

    test_so.obj.dlbm = 10
    test_so.obj.ikg = test_so.obj.ilbm - test_so.obj.dlbm
    TRICK_EXPECT_EQ( test_so.obj.ikg , 18, test_suite , "subtraction with swig_double" )

    test_so.obj.dkg = 10
    test_so.obj.ikg = test_so.obj.ilbm - test_so.obj.dkg
    TRICK_EXPECT_EQ( test_so.obj.ikg , 12, test_suite , "subtraction with swig_double and unit conversion" )

    # multiplication

    test_so.obj.ilbm = 50
    test_so.obj.ikg = test_so.obj.ilbm * 3
    TRICK_EXPECT_EQ( test_so.obj.ikg , 68, test_suite , "multiplication with int" )

    test_so.obj.ikg = test_so.obj.ilbm * 2.9
    TRICK_EXPECT_EQ( test_so.obj.ikg , 65, test_suite , "multiplication with float" )

    test_so.obj.ilbm = 50
    test_so.obj.i = 2
    test_so.obj.ikg = test_so.obj.ilbm * test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.ikg , 45, test_suite , "multiplication with unitless swig_int" )

    test_so.obj.ilbm = 50
    test_so.obj.dno_units = 2.2
    test_so.obj.ikg = test_so.obj.ilbm * test_so.obj.dno_units
    TRICK_EXPECT_EQ( test_so.obj.ikg , 49, test_suite , "multiplication with unitless swig_double" )

    # division

    test_so.obj.ilbm = 50
    test_so.obj.ikg = test_so.obj.ilbm / 3
    TRICK_EXPECT_EQ( test_so.obj.ikg , 7, test_suite , "division with int" )

    test_so.obj.ikg = test_so.obj.ilbm / 2.9
    TRICK_EXPECT_EQ( test_so.obj.ikg , 7, test_suite , "division with float" )

    # floor division

    test_so.obj.ikg = 29
    test_so.obj.ikg = test_so.obj.ikg // 4
    TRICK_EXPECT_EQ( test_so.obj.ikg , 7, test_suite , "floor division with int" )

    test_so.obj.ikg = 29
    test_so.obj.ikg = test_so.obj.ikg // 4.5
    TRICK_EXPECT_EQ( test_so.obj.ikg , 6, test_suite , "floor division with int" )

    # mod

    test_so.obj.ikg = 29
    test_so.obj.ikg = test_so.obj.ikg % 4
    TRICK_EXPECT_EQ( test_so.obj.ikg , 1, test_suite , "mod with int" )

    test_so.obj.ikg = 29
    test_so.obj.ikg = test_so.obj.ikg % 4.5
    TRICK_EXPECT_EQ( test_so.obj.ikg , 2, test_suite , "mod with float" )

    test_so.obj.ilbm = 50
    test_so.obj.i = 13
    test_so.obj.ikg = test_so.obj.ilbm % test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.ikg , 4, test_suite , "mod with unitless swig_int" )

    test_so.obj.ilbm = 50
    test_so.obj.dno_units = 13.5
    test_so.obj.ikg = test_so.obj.ilbm % test_so.obj.dno_units
    TRICK_EXPECT_EQ( test_so.obj.ikg , 4, test_suite , "mod with unitless swig_double" )

    # pow

    test_so.obj.i = 5
    test_so.obj.i = pow(test_so.obj.i , 4)
    TRICK_EXPECT_EQ( test_so.obj.i , 625, test_suite , "pow with int" )

    test_so.obj.i = 5
    test_so.obj.i = pow(test_so.obj.i , 2.5)
    TRICK_EXPECT_EQ( test_so.obj.i , 55, test_suite , "pow with float" )

    test_so.obj.i = 5
    test_so.obj.i = pow(test_so.obj.i , test_so.obj.i)
    TRICK_EXPECT_EQ( test_so.obj.i , 3125, test_suite , "pow with unitless swig_int" )

    test_so.obj.i = 5
    test_so.obj.dno_units = 5.0
    test_so.obj.i = pow(test_so.obj.i , test_so.obj.dno_units)
    TRICK_EXPECT_EQ( test_so.obj.i , 3125, test_suite , "pow with unitless swig_double" )

    # left shift

    test_so.obj.ikg = 16
    test_so.obj.ikg = test_so.obj.ikg << 2
    TRICK_EXPECT_EQ( test_so.obj.ikg , 64, test_suite , "left shift with int" )

    test_so.obj.ikg = 16
    test_so.obj.i = 1
    test_so.obj.ikg = test_so.obj.ikg << test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.ikg , 32, test_suite , "left shift with unitless swig_int" )

    # right shift

    test_so.obj.ikg = 16
    test_so.obj.ikg = test_so.obj.ikg >> 2
    TRICK_EXPECT_EQ( test_so.obj.ikg , 4, test_suite , "right shift with int" )

    test_so.obj.ikg = 16
    test_so.obj.i = 1
    test_so.obj.ikg = test_so.obj.ikg >> test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.ikg , 8, test_suite , "left shift with unitless swig_int" )

    # and

    test_so.obj.ikg = 12
    test_so.obj.ikg = test_so.obj.ikg & 5
    TRICK_EXPECT_EQ( test_so.obj.ikg , 4, test_suite , "and with int" )

    test_so.obj.ikg = 12
    test_so.obj.i = 5
    test_so.obj.ikg = test_so.obj.ikg & test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.ikg , 4, test_suite , "and with unitless swig_int" )

    # xor

    test_so.obj.ikg = 29
    test_so.obj.ikg = test_so.obj.ikg ^ 7
    TRICK_EXPECT_EQ( test_so.obj.ikg , 26, test_suite , "xor with int" )

    test_so.obj.ikg = 29
    test_so.obj.i = 7
    test_so.obj.ikg = test_so.obj.ikg ^ test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.ikg , 26, test_suite , "xor with unitless swig_int" )

    # or

    test_so.obj.ikg = 29
    test_so.obj.ikg = test_so.obj.ikg | 7
    TRICK_EXPECT_EQ( test_so.obj.ikg , 31, test_suite , "or with int" )

    test_so.obj.ikg = 29
    test_so.obj.i = 7
    test_so.obj.ikg = test_so.obj.ikg | test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.ikg , 31, test_suite , "or with unitless swig_int" )

    # reverse addition

    test_so.obj.ikg = 20 + test_so.obj.ilbm
    TRICK_EXPECT_EQ( test_so.obj.ikg , 31, test_suite , "reverse addition with integer" )

    test_so.obj.ikg = 20.9 + test_so.obj.ilbm
    TRICK_EXPECT_EQ( test_so.obj.ikg , 32, test_suite , "reverse addition with float" )

    # reverse subtraction

    test_so.obj.ikg = 120 - test_so.obj.ilbm
    TRICK_EXPECT_EQ( test_so.obj.ikg , 31, test_suite , "reverse subtraction with integer" )

    test_so.obj.ikg = 120.9 - test_so.obj.ilbm
    TRICK_EXPECT_EQ( test_so.obj.ikg , 32, test_suite , "reverse subtraction with float" )

    # reverse multiplication

    test_so.obj.ilbm = 50
    test_so.obj.ikg = 3 * test_so.obj.ilbm
    TRICK_EXPECT_EQ( test_so.obj.ikg , 68, test_suite , "reverse multiplication with int" )

    test_so.obj.ikg = 2.9 * test_so.obj.ilbm
    TRICK_EXPECT_EQ( test_so.obj.ikg , 65, test_suite , "reverse multiplication with float" )

    # reverse division

    test_so.obj.i = 5
    test_so.obj.i = 62 / test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.i , 12, test_suite , "reverse division with int" )

    test_so.obj.i = 5
    test_so.obj.i = 62.5 / test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.i , 12, test_suite , "reverse division with float" )

    # reverse mod

    test_so.obj.i = 5
    test_so.obj.i = 62 % test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.i , 2, test_suite , "reverse mod with int" )

    test_so.obj.i = 5
    test_so.obj.i = 62.5 % test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.i , 2, test_suite , "reverse mod with float" )

    # pow

    test_so.obj.i = 4
    test_so.obj.i = pow(4 , test_so.obj.i)
    TRICK_EXPECT_EQ( test_so.obj.i , 256, test_suite , "reverse pow with int" )

    test_so.obj.i = 5
    test_so.obj.i = pow(2.1 , test_so.obj.i)
    TRICK_EXPECT_EQ( test_so.obj.i , 40, test_suite , "reverse pow with float" )

    # reverse lshift
    test_so.obj.i = 3
    test_so.obj.i = 8 << test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.i , 64, test_suite , "reverse lshift with int" )

    # reverse rshift
    test_so.obj.i = 2
    test_so.obj.i = 8 >> test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.i , 2, test_suite , "reverse rshift with int" )

    # reverse and

    test_so.obj.ikg = 12
    test_so.obj.ikg = 5 & test_so.obj.ikg
    TRICK_EXPECT_EQ( test_so.obj.ikg , 4, test_suite , "reverse and with int" )

    # reverse xor

    test_so.obj.ikg = 29
    test_so.obj.ikg = 7 ^ test_so.obj.ikg
    TRICK_EXPECT_EQ( test_so.obj.ikg , 26, test_suite , "reverse xor with int" )

    # reverse or

    test_so.obj.ikg = 29
    test_so.obj.ikg = 7 | test_so.obj.ikg
    TRICK_EXPECT_EQ( test_so.obj.ikg , 31, test_suite , "reverse or with int" )

    # in-place addition

    test_so.obj.ikg = 10
    test_so.obj.ikg += 20
    TRICK_EXPECT_EQ( test_so.obj.ikg , 30, test_suite , "in-place addition with integer" )

    test_so.obj.ikg += 20.9
    TRICK_EXPECT_EQ( test_so.obj.ikg , 51, test_suite , "in-place addition with float" )

    test_so.obj.ilbm = 10
    test_so.obj.ikg += test_so.obj.ilbm
    TRICK_EXPECT_EQ( test_so.obj.ikg , 55, test_suite , "in-place addition with swig_int" )

    test_so.obj.dkg = 10
    test_so.obj.ikg += test_so.obj.dkg
    TRICK_EXPECT_EQ( test_so.obj.ikg , 65, test_suite , "in-place addition with swig_double and unit conversion" )

    # in-place subtraction

    test_so.obj.ikg = 10
    test_so.obj.ikg -= 2
    TRICK_EXPECT_EQ( test_so.obj.ikg , 8, test_suite , "in-place subtraction with integer" )

    test_so.obj.ikg -= 2.9
    TRICK_EXPECT_EQ( test_so.obj.ikg , 5, test_suite , "in-place subtraction with float" )

    test_so.obj.ilbm = 10
    test_so.obj.ikg -= test_so.obj.ilbm
    TRICK_EXPECT_EQ( test_so.obj.ikg , 1, test_suite , "in-place subtraction with swig_int" )

    test_so.obj.dkg = 1
    test_so.obj.ikg -= test_so.obj.dkg
    TRICK_EXPECT_EQ( test_so.obj.ikg , 0, test_suite , "in-place subtraction with swig_double and unit conversion" )

    # in-place multiplication

    test_so.obj.ikg = 10
    test_so.obj.ikg *= 2
    TRICK_EXPECT_EQ( test_so.obj.ikg , 20, test_suite , "in-place multiplication with integer" )

    test_so.obj.ikg *= 3.9
    TRICK_EXPECT_EQ( test_so.obj.ikg , 78, test_suite , "in-place multiplication with float" )

    test_so.obj.ikg = 10
    test_so.obj.i = 2
    test_so.obj.ikg *= test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.ikg , 20, test_suite , "in-place multiplication with unitless swig_int" )

    test_so.obj.ikg = 10
    test_so.obj.dno_units = 3.9
    test_so.obj.ikg *= test_so.obj.dno_units
    TRICK_EXPECT_EQ( test_so.obj.ikg , 39, test_suite , "in-place multiplication with unitless swig_double" )
    # in-place division

    test_so.obj.ikg = 10
    test_so.obj.ikg /= 2
    TRICK_EXPECT_EQ( test_so.obj.ikg , 5, test_suite , "in-place division with integer" )

    test_so.obj.ikg = 10
    test_so.obj.ikg /= 3.9
    TRICK_EXPECT_EQ( test_so.obj.ikg , 3, test_suite , "in-place division with float" )

    test_so.obj.ikg = 10
    test_so.obj.i = 2
    test_so.obj.ikg /= test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.ikg , 5, test_suite , "in-place division with unitless swig_int" )

    # in-place mod

    test_so.obj.ikg = 10
    test_so.obj.ikg %= 3
    TRICK_EXPECT_EQ( test_so.obj.ikg , 1, test_suite , "in-place mod with integer" )

    test_so.obj.ikg = 10
    test_so.obj.ikg %= 3.9
    TRICK_EXPECT_EQ( test_so.obj.ikg , 2, test_suite , "in-place mod with float" )

    test_so.obj.ikg = 10
    test_so.obj.i = 3
    test_so.obj.ikg %= test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.ikg , 1, test_suite , "in-place mod with unitless swig_int" )

    # in-place pow

    test_so.obj.i = 5
    test_so.obj.i **= 4
    TRICK_EXPECT_EQ( test_so.obj.i , 625, test_suite , "in-place pow with int" )

    test_so.obj.i = 5
    test_so.obj.i **= 2.5
    TRICK_EXPECT_EQ( test_so.obj.i , 56, test_suite , "in-place pow with float" )

    test_so.obj.i = 5
    test_so.obj.i **= test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.i , 3125, test_suite , "in-place pow with unitless swig_int" )

    test_so.obj.i = 5
    test_so.obj.dno_units = 5.0
    test_so.obj.i **= test_so.obj.dno_units
    TRICK_EXPECT_EQ( test_so.obj.i , 3125, test_suite , "in-place pow with unitless swig_double" )

    # in-place left shift

    test_so.obj.ikg = 16
    test_so.obj.ikg <<= 2
    TRICK_EXPECT_EQ( test_so.obj.ikg , 64, test_suite , "left shift with int" )

    test_so.obj.i = 16
    test_so.obj.ia[0] = 1
    test_so.obj.i <<= test_so.obj.ia[0]
    TRICK_EXPECT_EQ( test_so.obj.i , 32, test_suite , "left shift with unitless swig_int" )

    # in-place right shift

    test_so.obj.ikg = 16
    test_so.obj.ikg >>= 2
    TRICK_EXPECT_EQ( test_so.obj.ikg , 4, test_suite , "right shift with int" )

    test_so.obj.ikg = 16
    test_so.obj.i = 1
    test_so.obj.ikg >>= test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.ikg , 8, test_suite , "left shift with unitless swig_int" )

    # in-place and

    test_so.obj.ikg = 12
    test_so.obj.ikg &= 5
    TRICK_EXPECT_EQ( test_so.obj.ikg , 4, test_suite , "and with int" )

    test_so.obj.ikg = 12
    test_so.obj.i = 5
    test_so.obj.ikg &= test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.ikg , 4, test_suite , "and with unitless swig_int" )

    # in-place xor

    test_so.obj.ikg = 29
    test_so.obj.ikg ^= 7
    TRICK_EXPECT_EQ( test_so.obj.ikg , 26, test_suite , "xor with int" )

    test_so.obj.ikg = 29
    test_so.obj.i = 7
    test_so.obj.ikg ^= test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.ikg , 26, test_suite , "xor with unitless swig_int" )

    # in-place or

    test_so.obj.ikg = 29
    test_so.obj.ikg |= 7
    TRICK_EXPECT_EQ( test_so.obj.ikg , 31, test_suite , "or with int" )

    test_so.obj.ikg = 29
    test_so.obj.i = 7
    test_so.obj.ikg |= test_so.obj.i
    TRICK_EXPECT_EQ( test_so.obj.ikg , 31, test_suite , "or with unitless swig_int" )

    # less than

    test_so.obj.ikg = 20
    test_so.obj.ilbm = 20

    test = test_so.obj.ikg < 21
    TRICK_EXPECT_EQ( test , True, test_suite , "lt with integer" )

    test = test_so.obj.ikg < 20.5
    TRICK_EXPECT_EQ( test , True, test_suite , "lt with float" )

    test = test_so.obj.ikg < test_so.obj.ilbm
    TRICK_EXPECT_EQ( test , False, test_suite , "lt with swig_int and unit conversion" )

    test_so.obj.dkg = 20.1
    test = test_so.obj.ikg < test_so.obj.dkg
    TRICK_EXPECT_EQ( test , True, test_suite , "lt with swig_double and unit conversion" )

    # less than or equal

    test_so.obj.ikg = 20
    test_so.obj.ilbm = 20

    test = test_so.obj.ikg <= 21
    TRICK_EXPECT_EQ( test , True, test_suite , "le with integer" )

    test = test_so.obj.ikg <= 20.5
    TRICK_EXPECT_EQ( test , True, test_suite , "le with float" )

    test = test_so.obj.ikg <= test_so.obj.ilbm
    TRICK_EXPECT_EQ( test , False, test_suite , "le with swig_int and unit conversion" )

    test_so.obj.dkg = 20.1
    test = test_so.obj.ikg <= test_so.obj.dkg
    TRICK_EXPECT_EQ( test , True, test_suite , "le with swig_double and unit conversion" )

    # equal

    test = test_so.obj.ikg == 21
    TRICK_EXPECT_EQ( test , False, test_suite , "eq with integer" )

    test = test_so.obj.ikg == 20.5
    TRICK_EXPECT_EQ( test , False, test_suite , "eq with float" )

    test = test_so.obj.ikg == test_so.obj.ilbm
    TRICK_EXPECT_EQ( test , False, test_suite , "eq with swig_int and unit conversion" )

    test_so.obj.dkg = 20.1
    test = test_so.obj.ikg == test_so.obj.dkg
    TRICK_EXPECT_EQ( test , False, test_suite , "eq with swig_double and unit conversion" )

    # not equal

    test = test_so.obj.ikg != 21
    TRICK_EXPECT_EQ( test , True, test_suite , "ne with integer" )

    test = test_so.obj.ikg != 20.5
    TRICK_EXPECT_EQ( test , True, test_suite , "ne with float" )

    test = test_so.obj.ikg != test_so.obj.ilbm
    TRICK_EXPECT_EQ( test , True, test_suite , "ne with swig_int and unit conversion" )

    test_so.obj.dkg = 20.1
    test = test_so.obj.ikg != test_so.obj.dkg
    TRICK_EXPECT_EQ( test , True, test_suite , "ne with swig_double and unit conversion" )

    # greater than

    test_so.obj.ikg = 20
    test_so.obj.ilbm = 20

    test = test_so.obj.ikg > 21
    TRICK_EXPECT_EQ( test , False, test_suite , "gt with integer" )

    test = test_so.obj.ikg > 20.5
    TRICK_EXPECT_EQ( test , False, test_suite , "gt with float" )

    test = test_so.obj.ikg > test_so.obj.ilbm
    TRICK_EXPECT_EQ( test , True, test_suite , "gt with swig_int and unit conversion" )

    test_so.obj.dkg = 20.1
    test = test_so.obj.ikg > test_so.obj.dkg
    TRICK_EXPECT_EQ( test , False, test_suite , "gt with swig_double and unit conversion" )

    # greater than or equal

    test_so.obj.ikg = 20
    test_so.obj.ilbm = 20

    test = test_so.obj.ikg >= 21
    TRICK_EXPECT_EQ( test , False, test_suite , "ge with integer" )

    test = test_so.obj.ikg >= 20.5
    TRICK_EXPECT_EQ( test , False, test_suite , "ge with float" )

    test = test_so.obj.ikg >= test_so.obj.ilbm
    TRICK_EXPECT_EQ( test , True, test_suite , "ge with swig_int and unit conversion" )

    test_so.obj.dkg = 20.1
    test = test_so.obj.ikg >= test_so.obj.dkg
    TRICK_EXPECT_EQ( test , False, test_suite , "ge with swig_double and unit conversion" )

    # unary operators

    test_so.obj.ikg = 20
    test = -test_so.obj.ikg
    TRICK_EXPECT_EQ( test , -20, test_suite , "unary neg" )

    test = +test_so.obj.ikg
    TRICK_EXPECT_EQ( test , 20, test_suite , "unary pos" )

    test_so.obj.ikg = -20
    test = abs(test_so.obj.ikg)
    TRICK_EXPECT_EQ( test , 20, test_suite , "unary abs" )

    test_so.obj.ikg = 20
    test = ~test_so.obj.ikg
    TRICK_EXPECT_EQ( test , -21, test_suite , "unary invert" )

    # conversion
    test_so.obj.ikg = 20
    test = int(test_so.obj.ikg)
    TRICK_EXPECT_EQ( test , 20, test_suite , "int" )

    if sys.version_info < (3,0):
        test_so.obj.ikg = 20
        test = long(test_so.obj.ikg)
        TRICK_EXPECT_EQ( test , 20, test_suite , "long" )

    test_so.obj.ikg = 20
    test = float(test_so.obj.ikg)
    TRICK_EXPECT_EQ( test , 20, test_suite , "float" )

    test_so.obj.ikg = 20
    test = oct(test_so.obj.ikg)
    if sys.version_info >= (3,0):
        TRICK_EXPECT_EQ( test , "0o24", test_suite , "oct" )
    else:
        TRICK_EXPECT_EQ( test , "024", test_suite , "oct" )

    test_so.obj.ikg = 20
    test = hex(test_so.obj.ikg)
    TRICK_EXPECT_EQ( test , "0x14", test_suite , "hex" )

######################################################################################################################

    # swig_integer to swig_double assignment
    test_suite = "swig_double"

    test_so.obj.ilbm = 50
    test_so.obj.dlbm = test_so.obj.ilbm

    # swig_double unitless to swig_double assignment

    test_so.obj.dno_units = trick.attach_units("--" , 60.6)
    TRICK_EXPECT_EQ( test_so.obj.dno_units , 60.6, test_suite , "assignment from unitless swig_double" )
    trick.trick_test_add_parent( test_suite , "assignment from unitless swig_double" , "1164062396")

    # addition

    test_so.obj.dlbm = 50
    test_so.obj.dkg = test_so.obj.dlbm
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 22.6796, 0.0001, test_suite , "units conversion" )

    test_so.obj.dkg = test_so.obj.dlbm + 20
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 31.7515, 0.0001, test_suite , "addition with integer" )

    test_so.obj.dkg = test_so.obj.dlbm + 20.9
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 32.1597, 0.0001, test_suite , "addition with float" )

    test_so.obj.ilbm = 50
    test_so.obj.dkg = test_so.obj.dlbm + test_so.obj.ilbm
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 45.3592, 0.0001, test_suite , "addition with swig_int" )

    test_so.obj.dkg = 50
    test_so.obj.ikg = 50
    test_so.obj.dkg = test_so.obj.dlbm + test_so.obj.ikg
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 72.6796, 0.0001, test_suite , "addition with swig_int and unit conversion" )

    test_so.obj.dlbm = 10
    test_so.obj.dkg = test_so.obj.dlbm + test_so.obj.dlbm
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 9.07185, 0.0001, test_suite , "addition with swig_double" )

    test_so.obj.dkg = 10
    test_so.obj.dkg = test_so.obj.dlbm + test_so.obj.dkg
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 14.5359, 0.0001, test_suite , "addition with swig_double and unit conversion" )

    # subtraction

    test_so.obj.dlbm = 50
    test_so.obj.dkg = test_so.obj.dlbm - 20
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 13.6078, 0.0001, test_suite , "subtraction with integer" )

    test_so.obj.dkg = test_so.obj.dlbm - 20.9
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 13.1995, 0.0001, test_suite , "subtraction with float" )

    test_so.obj.ilbm = 50
    test_so.obj.dkg = test_so.obj.dlbm - test_so.obj.ilbm
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 0, 0.0001, test_suite , "subtraction with swig_int" )

    test_so.obj.dkg = 50
    test_so.obj.ikg = 50
    test_so.obj.dkg = test_so.obj.dlbm - test_so.obj.ikg
    TRICK_EXPECT_NEAR( test_so.obj.dkg , -27.3204, 0.0001, test_suite , "subtraction with swig_int and unit conversion" )

    test_so.obj.dlbm = 10
    test_so.obj.dkg = test_so.obj.dlbm - test_so.obj.dlbm
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 0, 0.0001, test_suite , "subtraction with swig_double" )

    test_so.obj.dkg = 10
    test_so.obj.dkg = test_so.obj.dlbm - test_so.obj.dkg
    TRICK_EXPECT_NEAR( test_so.obj.dkg , -5.46408, 0.0001, test_suite , "subtraction with swig_double and unit conversion" )

    # multiplication

    test_so.obj.dlbm = 50
    test_so.obj.dkg = test_so.obj.dlbm * 3
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 68.0389, 0.0001, test_suite , "multiplication with int" )

    test_so.obj.dkg = test_so.obj.ilbm * 2.9
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 65.7709, 0.0001, test_suite , "multiplication with float" )

    test_so.obj.dlbm = 50
    test_so.obj.i = 2
    test_so.obj.dkg = test_so.obj.dlbm * test_so.obj.i
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 45.3592, 0.0001, test_suite , "multiplication with unitless swig_int" )

    test_so.obj.dlbm = 50
    test_so.obj.dno_units = 2.2
    test_so.obj.dkg = test_so.obj.dlbm * test_so.obj.dno_units
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 49.8952, 0.0001, test_suite , "multiplication with unitless swig_double" )

    # division

    test_so.obj.dlbm = 50
    test_so.obj.dkg = test_so.obj.dlbm / 3
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 7.55987, 0.0001, test_suite , "division with int" )

    test_so.obj.dkg = test_so.obj.dlbm / 2.9
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 7.82056, 0.0001, test_suite , "division with float" )

    test_so.obj.i = 5
    test_so.obj.dkg = test_so.obj.dlbm / test_so.obj.i
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 4.53592, 0.0001, test_suite , "division with unitless swig_int" )

    test_so.obj.dno_units = 5.1
    test_so.obj.dkg = test_so.obj.dlbm / test_so.obj.dno_units
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 4.44698, 0.0001, test_suite , "division with unitless swig_double" )

    # floor division

    test_so.obj.dkg = 29
    test_so.obj.dkg = test_so.obj.dkg // 4
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 7, 0.0001, test_suite , "floor division with int" )

    test_so.obj.dkg = 29
    test_so.obj.dkg = test_so.obj.dkg // 4.5
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 6, 0.0001, test_suite , "floor division with float" )

    test_so.obj.dkg = 29
    test_so.obj.i = 4
    test_so.obj.dkg = test_so.obj.dkg // test_so.obj.i
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 7, 0.0001, test_suite , "floor division with unitless swig_int" )

    test_so.obj.dkg = 29
    test_so.obj.dno_units = 4.5
    test_so.obj.dkg = test_so.obj.dkg // test_so.obj.dno_units
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 6, 0.0001, test_suite , "floor division with unitless swig_double" )


    # mod

    test_so.obj.dkg = 29
    test_so.obj.dkg = test_so.obj.dkg % 4
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 1, 0.0001, test_suite , "mod with int" )

    test_so.obj.dkg = 29
    test_so.obj.dkg = test_so.obj.dkg % 4.5
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 2, 0.0001, test_suite , "mod with float" )

    test_so.obj.dlbm = 50
    test_so.obj.i = 13
    test_so.obj.dlbm = test_so.obj.dlbm % test_so.obj.i
    TRICK_EXPECT_NEAR( test_so.obj.dlbm , 11, 0.0001, test_suite , "mod with unitless swig_int" )

    test_so.obj.dlbm = 50
    test_so.obj.dno_units = 13.5
    test_so.obj.dlbm = test_so.obj.dlbm % test_so.obj.dno_units
    TRICK_EXPECT_NEAR( test_so.obj.dlbm , 9.5, 0.0001, test_suite , "mod with unitless swig_double" )

    # pow

    test_so.obj.dno_units = 5
    test_so.obj.dno_units = pow(test_so.obj.dno_units , 4)
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 625, 0.0001, test_suite , "pow with int" )

    test_so.obj.dno_units = 5
    test_so.obj.dno_units = pow(test_so.obj.dno_units , 2.5)
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 55.9017, 0.0001, test_suite , "pow with float" )

    test_so.obj.i = 5
    test_so.obj.dno_units = 5.0
    test_so.obj.dno_units = pow(test_so.obj.dno_units , test_so.obj.i)
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 3125, 0.0001, test_suite , "pow with unitless swig_int" )

    test_so.obj.dno_units = 5.0
    test_so.obj.dno_units = pow(test_so.obj.dno_units , test_so.obj.dno_units)
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 3125, 0.0001, test_suite , "pow with unitless swig_double" )

    # reverse addition

    test_so.obj.dlbm = 10
    test_so.obj.dkg = 20 + test_so.obj.dlbm
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 13.6078, 0.0001, test_suite , "reverse addition with integer" )

    test_so.obj.dkg = 20.9 + test_so.obj.dlbm
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 14.016, 0.0001, test_suite , "reverse addition with float" )

    # reverse subtraction

    test_so.obj.dkg = 120 - test_so.obj.dlbm
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 49.8952, 0.0001, test_suite , "reverse subtraction with integer" )

    test_so.obj.dkg = 120.9 - test_so.obj.dlbm
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 50.3034, 0.0001, test_suite , "reverse subtraction with float" )

    # reverse multiplication

    test_so.obj.dlbm = 50
    test_so.obj.dkg = 3 * test_so.obj.dlbm
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 68.0389, 0.0001, test_suite , "reverse multiplication with int" )

    test_so.obj.dkg = 2.9 * test_so.obj.dlbm
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 65.7709, 0.0001, test_suite , "reverse multiplication with float" )


    # reverse division

    test_so.obj.dno_units = 5
    test_so.obj.dno_units = 62 / test_so.obj.dno_units
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 12.4, 0.0001, test_suite , "reverse division with int" )

    test_so.obj.dno_units = 5
    test_so.obj.dno_units = 62.5 / test_so.obj.dno_units
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 12.5, 0.0001, test_suite , "reverse division with float" )

    # reverse floor division
    test_so.obj.dno_units = 5
    test_so.obj.dno_units = 62 // test_so.obj.dno_units
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 12, 0.0001, test_suite , "reverse floor division with int" )

    test_so.obj.dno_units = 5
    test_so.obj.dno_units = 62.5 // test_so.obj.dno_units
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 12, 0.0001, test_suite , "reverse floor division with float" )


    # reverse mod

    test_so.obj.dno_units = 5
    test_so.obj.dno_units = 62 % test_so.obj.dno_units
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 2, 0.0001, test_suite , "reverse mod with int" )

    test_so.obj.dno_units = 5
    test_so.obj.dno_units = 62.5 % test_so.obj.dno_units
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 2.5, 0.0001, test_suite , "reverse mod with float" )

    # reverse pow

    test_so.obj.dno_units = 4
    test_so.obj.dno_units = pow(4 , test_so.obj.dno_units)
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 256, 0.0001, test_suite , "reverse pow with int" )

    test_so.obj.dno_units = 5
    test_so.obj.dno_units = pow(2.1 , test_so.obj.dno_units)
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 40.841 , 0.0001, test_suite , "reverse pow with float" )

    # in-place addition

    test_so.obj.dkg = 10
    test_so.obj.dkg += 20
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 30, 0.0001, test_suite , "in-place addition with integer" )

    test_so.obj.dkg += 20.9
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 50.9, 0.0001, test_suite , "in-place addition with float" )

    test_so.obj.ilbm = 10
    test_so.obj.dkg += test_so.obj.ilbm
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 55.4359, 0.0001, test_suite , "in-place addition with swig_int" )

    test_so.obj.dkg = 10
    test_so.obj.dkg += test_so.obj.dkg
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 20, 0.0001, test_suite , "in-place addition with swig_double and unit conversion" )

    # in-place subtraction

    test_so.obj.dkg = 10
    test_so.obj.dkg -= 2
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 8, 0.0001, test_suite , "in-place subtraction with integer" )

    test_so.obj.dkg -= 2.9
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 5.1, 0.0001, test_suite , "in-place subtraction with float" )

    test_so.obj.dlbm = 10
    test_so.obj.dkg -= test_so.obj.dlbm
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 0.564076, 0.0001, test_suite , "in-place subtraction with swig_int" )

    test_so.obj.dkg = 1
    test_so.obj.dkg -= test_so.obj.dkg
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 0, 0.0001, test_suite , "in-place subtraction with swig_double and unit conversion" )

    # in-place multiplication

    test_so.obj.dkg = 10
    test_so.obj.dkg *= 2
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 20, 0.0001, test_suite , "in-place multiplication with integer" )

    test_so.obj.dkg *= 3.9
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 78, 0.0001, test_suite , "in-place multiplication with float" )

    test_so.obj.dkg = 10
    test_so.obj.i = 2
    test_so.obj.dkg *= test_so.obj.i
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 20, 0.0001, test_suite , "in-place multiplication with unitless swig_int" )

    test_so.obj.dkg = 10
    test_so.obj.dno_units = 2
    test_so.obj.dkg *= test_so.obj.dno_units
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 20, 0.0001, test_suite , "in-place multiplication with unitless swig_int" )

    # in-place division

    test_so.obj.dkg = 10
    test_so.obj.dkg /= 2
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 5, 0.0001, test_suite , "in-place division with integer" )

    test_so.obj.dkg = 10
    test_so.obj.dkg /= 3.9
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 2.5641, 0.0001, test_suite , "in-place division with float" )

    test_so.obj.dkg = 10
    test_so.obj.i = 2
    test_so.obj.dkg /= test_so.obj.i
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 5, 0.0001, test_suite , "in-place division with unitless swig_int" )

    test_so.obj.dkg = 10
    test_so.obj.dno_units = 2
    test_so.obj.dkg /= test_so.obj.dno_units
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 5, 0.0001, test_suite , "in-place division with unitless swig_int" )

    # in-place floor division

    test_so.obj.dkg = 10.1
    test_so.obj.dkg //= 2
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 5, 0.0001, test_suite , "in-place division with integer" )

    test_so.obj.dkg = 10.1
    test_so.obj.dkg //= 3.9
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 2, 0.0001, test_suite , "in-place division with float" )

    test_so.obj.dkg = 10.1
    test_so.obj.i = 2
    test_so.obj.dkg //= test_so.obj.i
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 5, 0.0001, test_suite , "in-place division with unitless swig_int" )

    test_so.obj.dkg = 10.1
    test_so.obj.dno_units = 2
    test_so.obj.dkg //= test_so.obj.dno_units
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 5, 0.0001, test_suite , "in-place division with unitless swig_int" )

    # in-place mod

    test_so.obj.dkg = 10.1
    test_so.obj.dkg %= 3
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 1.1, 0.0001, test_suite , "in-place mod with integer" )

    test_so.obj.dkg = 10.1
    test_so.obj.dkg %= 3.9
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 2.3, 0.0001, test_suite , "in-place mod with float" )

    test_so.obj.dkg = 10.1
    test_so.obj.i = 3
    test_so.obj.dkg %= test_so.obj.i
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 1.1, 0.0001, test_suite , "in-place mod with unitless swig_int" )

    test_so.obj.dkg = 10.1
    test_so.obj.dno_units = 4
    test_so.obj.dkg %= test_so.obj.dno_units
    TRICK_EXPECT_NEAR( test_so.obj.dkg , 2.1, 0.0001, test_suite , "in-place mod with unitless swig_double" )

    # in-place pow

    test_so.obj.dno_units = 5
    test_so.obj.dno_units **= 4
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 625, 0.0001, test_suite , "in-place pow with int" )

    test_so.obj.dno_units = 5
    test_so.obj.dno_units **= 2.5
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 55.9017, 0.0001, test_suite , "in-place pow with float" )

    test_so.obj.i = 5
    test_so.obj.dno_units = 5.0
    test_so.obj.dno_units **= test_so.obj.i
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 3125, 0.0001, test_suite , "in-place pow with unitless swig_int" )

    test_so.obj.dno_units = 5.0
    test_so.obj.dno_units **= test_so.obj.dno_units
    TRICK_EXPECT_NEAR( test_so.obj.dno_units , 3125, 0.0001, test_suite , "in-place pow with unitless swig_double" )

    # less than

    test_so.obj.dkg = 20
    test_so.obj.dlbm = 20

    test = test_so.obj.dkg < 21
    TRICK_EXPECT_EQ( test , True, test_suite , "lt with integer" )

    test = test_so.obj.dkg < 20.5
    TRICK_EXPECT_EQ( test , True, test_suite , "lt with float" )

    test = test_so.obj.dkg < test_so.obj.dlbm
    TRICK_EXPECT_EQ( test , False, test_suite , "lt with swig_int and unit conversion" )

    test_so.obj.dkg = 20.1
    test = test_so.obj.dkg < test_so.obj.dkg
    TRICK_EXPECT_EQ( test , False, test_suite , "lt with swig_double and unit conversion" )

    # less than or equal

    test_so.obj.dkg = 20
    test_so.obj.dlbm = 20

    test = test_so.obj.dkg <= 21
    TRICK_EXPECT_EQ( test , True, test_suite , "le with integer" )

    test = test_so.obj.dkg <= 20.5
    TRICK_EXPECT_EQ( test , True, test_suite , "le with float" )

    test = test_so.obj.dkg <= test_so.obj.dlbm
    TRICK_EXPECT_EQ( test , False, test_suite , "le with swig_int and unit conversion" )

    test_so.obj.dkg = 20.1
    test = test_so.obj.dkg <= test_so.obj.dkg
    TRICK_EXPECT_EQ( test , True, test_suite , "le with swig_double and unit conversion" )

    # equal

    test = test_so.obj.dkg == 21
    TRICK_EXPECT_EQ( test , False, test_suite , "eq with integer" )

    test = test_so.obj.dkg == 20.5
    TRICK_EXPECT_EQ( test , False, test_suite , "eq with float" )

    test = test_so.obj.dkg == test_so.obj.dlbm
    TRICK_EXPECT_EQ( test , False, test_suite , "eq with swig_int and unit conversion" )

    test_so.obj.dkg = 20.1
    test = test_so.obj.dkg == test_so.obj.dkg
    TRICK_EXPECT_EQ( test , True, test_suite , "eq with swig_double and unit conversion" )

    # not equal

    test = test_so.obj.dkg != 21
    TRICK_EXPECT_EQ( test , True, test_suite , "ne with integer" )

    test = test_so.obj.dkg != 20.5
    TRICK_EXPECT_EQ( test , True, test_suite , "ne with float" )

    test = test_so.obj.dkg != test_so.obj.dlbm
    TRICK_EXPECT_EQ( test , True, test_suite , "ne with swig_int and unit conversion" )

    test_so.obj.dkg = 20.1
    test = test_so.obj.dkg != test_so.obj.dkg
    TRICK_EXPECT_EQ( test , False, test_suite , "ne with swig_double and unit conversion" )

    # greater than

    test_so.obj.dkg = 20
    test_so.obj.dlbm = 20

    test = test_so.obj.dkg > 21
    TRICK_EXPECT_EQ( test , False, test_suite , "gt with integer" )

    test = test_so.obj.dkg > 20.5
    TRICK_EXPECT_EQ( test , False, test_suite , "gt with float" )

    test = test_so.obj.dkg > test_so.obj.dlbm
    TRICK_EXPECT_EQ( test , True, test_suite , "gt with swig_int and unit conversion" )

    test_so.obj.dkg = 20.1
    test = test_so.obj.dkg > test_so.obj.dkg
    TRICK_EXPECT_EQ( test , False, test_suite , "gt with swig_double and unit conversion" )

    # greater than or equal

    test_so.obj.dkg = 20
    test_so.obj.dlbm = 20

    test = test_so.obj.dkg >= 21
    TRICK_EXPECT_EQ( test , False, test_suite , "ge with integer" )

    test = test_so.obj.dkg >= 20.5
    TRICK_EXPECT_EQ( test , False, test_suite , "ge with float" )

    test = test_so.obj.dkg >= test_so.obj.dlbm
    TRICK_EXPECT_EQ( test , True, test_suite , "ge with swig_int and unit conversion" )

    test_so.obj.dkg = 20.1
    test = test_so.obj.dkg >= test_so.obj.dkg
    TRICK_EXPECT_EQ( test , True, test_suite , "ge with swig_double and unit conversion" )

    # unary operators

    test_so.obj.dkg = 20
    test = -test_so.obj.dkg
    TRICK_EXPECT_NEAR( test , -20, 0.0001, test_suite , "unary neg" )

    test = +test_so.obj.dkg
    TRICK_EXPECT_NEAR( test , 20, 0.0001, test_suite , "unary pos" )

    test_so.obj.dkg = -20
    test = abs(test_so.obj.dkg)
    TRICK_EXPECT_NEAR( test , 20, 0.0001, test_suite , "unary abs" )

    # conversion
    test_so.obj.dkg = 20
    test = int(test_so.obj.dkg)
    TRICK_EXPECT_NEAR( test , 20, 0.0001, test_suite , "int" )

    if sys.version_info < (3,0):
        test_so.obj.dkg = 20
        test = long(test_so.obj.dkg)
        TRICK_EXPECT_NEAR( test , 20, 0.0001, test_suite , "long" )

    test_so.obj.dkg = 20
    test = float(test_so.obj.dkg)
    TRICK_EXPECT_NEAR( test , 20, 0.0001, test_suite , "float" )

######################################################################################################################

    # Typedefed integers
    test_suite = "typedef"

    test_so.obj.i = 40
    test_so.obj.iii = test_so.obj.i
    test_so.obj.aiii = test_so.obj.iii

    TRICK_EXPECT_EQ( test_so.obj.iii , 40, test_suite , "integer from integer" )
    trick.trick_test_add_parent( test_suite , "integer from integer" , "1011083320")
    TRICK_EXPECT_EQ( test_so.obj.aiii , 40, test_suite , "integer from another typedefed integer" )
    trick.trick_test_add_parent( test_suite , "integer from another typedefed integer" , "1011083320")

######################################################################################################################

    test_suite = "structure"

    # Structs with more than one name
    test_so.t.i = 300
    TRICK_EXPECT_EQ( test_so.t.i , 300, test_suite , "multi named structure no unit assignment" )

    test_so.t.i = trick.attach_units("in", 300)
    TRICK_EXPECT_EQ( test_so.t.i , 7, test_suite , "multi named structure with unit assignment" )

    test_so.t.d = 300
    TRICK_EXPECT_NEAR( test_so.t.d , 300, 0.0001 , test_suite , "multi named structure no unit assignment" )

    test_so.t.d = trick.attach_units("in", 300)
    TRICK_EXPECT_NEAR( test_so.t.d , 7.62 , 0.0001 , test_suite , "multi named structure with unit assignment" )

######################################################################################################################

    # Templates
    test_suite = "template"

    # simple template
    test_so.obj.my_template_var.var1 = 30.0
    test_so.obj.my_template_var.var2 = 40
    test_so.obj.my_template_var.var3 = 50
    TRICK_EXPECT_NEAR( test_so.obj.my_template_var.var1 , 30.0 , 0.000001 , test_suite , "double assignment" )
    TRICK_EXPECT_EQ( test_so.obj.my_template_var.var2 , 40 , test_suite , "int assignment" )
    TRICK_EXPECT_EQ( test_so.obj.my_template_var.var3 , 50 , test_suite , "short assignment" )
    trick.trick_test_add_parent( test_suite , "double assignment" , "2642836719")
    trick.trick_test_add_parent( test_suite , "int assignment" , "2642836719")
    trick.trick_test_add_parent( test_suite , "short assignment" , "2642836719")


    # using typedef from within template
    test_so.obj.my_template_var_int = 66
    TRICK_EXPECT_EQ( test_so.obj.my_template_var_int , 66, test_suite , "use typedef from within a template" )

    # a more convoluted template
    test_so.obj.TTT_var.aa = 1000
    TRICK_EXPECT_EQ( test_so.obj.TTT_var.aa , 1000, test_suite , "class complicated integer" )

    test_so.obj.TTT_var.bb = 2000.0
    TRICK_EXPECT_NEAR( test_so.obj.TTT_var.bb , 2000, 0.0001, test_suite , "class complicated double" )

######################################################################################################################

    # Namespace
    test_suite = "namespace"

    test_so.ns_test.mass = trick.attach_units("lb", 10)
    TRICK_EXPECT_NEAR( test_so.ns_test.mass , 4.53592, 0.0001, test_suite , "Class variable with units" )
    trick.trick_test_add_parent( test_suite , "Class variable with units" , "2546878004")

    test_so.ns_test.bbp = trick.alloc_type(2, "my_ns::BB")
    test_so.ns_test.bbp[0].str = "hello"
    test_so.ns_test.bbp[1].str = "there"
    temp = test_so.ns_test.bbp[0].str + " " + test_so.ns_test.bbp[1].str
    TRICK_EXPECT_EQ( str(temp) , "hello there", test_suite , "1D Class allocation" )
    trick.trick_test_add_parent( test_suite , "1D Class allocation" , "2546878004")

    test_so.ns_test.bbpp = trick.alloc_type(4, "my_ns::BB *")
    test_so.ns_test.bbpp[0] = trick.alloc_type(3, "my_ns::BB")
    test_so.ns_test.bbpp[0][0].str = "bark"
    test_so.ns_test.bbpp[0][1].str = "meow"
    test_so.ns_test.bbpp[0][2].str = "quack"
    temp = test_so.ns_test.bbpp[0][0].str + " " + test_so.ns_test.bbpp[0][1].str + " " + test_so.ns_test.bbpp[0][2].str
    TRICK_EXPECT_EQ( str(temp) , "bark meow quack", test_suite , "2D Class allocation" )
    trick.trick_test_add_parent( test_suite , "2D Class allocation" , "2546878004")

######################################################################################################################

    # Miscellaneous
    test_suite = "misc"

    test_so.obj.d = 10
    test_so.obj.ds = 15
    temp = test_so.obj.d + float(test_so.obj.ds)
    TRICK_EXPECT_NEAR( temp , 25 , 0.0001 , test_suite , "Forced remove units" )
    trick.trick_test_add_parent( test_suite , "Forced remove units" , "3339258059")

    temp = [ test_so.obj.d , test_so.obj.ds ]
    TRICK_EXPECT_EQ( str(temp) , "[10 kg, 15 s]", test_suite , "List with different objects" )

    test_so.obj.dp = trick.get_address("test_so.obj.d")
    TRICK_EXPECT_EQ( str(test_so.obj.dp) , "[10 kg]", test_suite , "Get address" )

    test_so.obj.d += 1
    TRICK_EXPECT_EQ( str(test_so.obj.dp) , "[11 kg]", test_suite , "Get address verification" )

    test_so.obj.da[2] = 45
    test_so.obj.dp = trick.get_address("test_so.obj.da[2]")
    TRICK_EXPECT_EQ( str(test_so.obj.dp) , "[45 kg]", test_suite , "Get address mid-array" )

    test_so.obj.dp = trick.get_address("test_so.obj.daa[1][1]")
    TRICK_EXPECT_EQ( str(test_so.obj.dp) , "[60 kg]", test_suite , "Get address multi-dimensional mid-array" )

    temp_array = test_so.obj.daa[1]
    TRICK_EXPECT_EQ( str(temp_array) , "[50.1 kg, 60 kg, 52.3 kg]", test_suite , "Local variable reference to array" )

    # "const int & cir" and "int const & icr" are pointed to i in the Ball_alex constructor
    test_so.obj.i = 55
    TRICK_EXPECT_EQ( test_so.obj.cir , 55, test_suite , "Const reference" )
    TRICK_EXPECT_EQ( test_so.obj.icr , 55, test_suite , "Const reference" )

    #test_so.obj.cir = 99
    #TRICK_EXPECT_EQ( test_so.obj.cir , 55, test_suite , "Const reference immutable test 1" )
    #TRICK_EXPECT_EQ( test_so.obj.icr , 55, test_suite , "Const reference immutable test 1" )

    #test_so.obj.icr = 98
    #TRICK_EXPECT_EQ( test_so.obj.cir , 55, test_suite , "Const reference immutable test 2" )
    #TRICK_EXPECT_EQ( test_so.obj.icr , 55, test_suite , "Const reference immutable test 2" )

    test_so.obj.iiia = [ 300 , 400 , 500 , 600 , 700 ]
    TRICK_EXPECT_EQ( str(test_so.obj.iiia) , "[300, 400, 500, 600, 700, 0]", test_suite , "Typedeffed integer type" )

    # scd = static const double, csd = const static double, sdc = static double const
    TRICK_EXPECT_NEAR( test_so.obj.scd , 1.2345 , 0.0001 , test_suite , "Static const access" )
    TRICK_EXPECT_NEAR( test_so.obj.csd , 6.7890 , 0.0001 , test_suite , "Static const access" )
    TRICK_EXPECT_NEAR( test_so.obj.sdc , 9.8765 , 0.0001 , test_suite , "Static const access" )

    # Attempt to change a static const double
    #test_so.obj.scd = 90.0 ;
    #TRICK_EXPECT_NEAR( test_so.obj.scd , 1.2345 , 0.0001 , test_suite , "Static const immutable test 1" )
    #trick_mm.mmw.mm.read_checkpoint_from_string("test_so.obj.scd = 2.2222 ;")
    #TRICK_EXPECT_NEAR( test_so.obj.scd , 1.2345 , 0.0001 , test_suite , "Static const immutable test 2" )
    #test_so.obj.csd = 90.0 ;
    #TRICK_EXPECT_NEAR( test_so.obj.csd , 6.7890 , 0.0001 , test_suite , "Static const immutable test 3" )
    #trick_mm.mmw.mm.read_checkpoint_from_string("test_so.obj.csd = 2.2222 ;")
    #TRICK_EXPECT_NEAR( test_so.obj.csd , 6.7890 , 0.0001 , test_suite , "Static const immutable test 4" )
    #test_so.obj.sdc = 90.0 ;
    #TRICK_EXPECT_NEAR( test_so.obj.sdc , 9.8765 , 0.0001 , test_suite , "Static const immutable test 5" )
    #trick_mm.mmw.mm.read_checkpoint_from_string("test_so.obj.sdc = 2.2222 ;")
    #TRICK_EXPECT_NEAR( test_so.obj.sdc , 9.8765 , 0.0001 , test_suite , "Static const immutable test 6" )

    tester = trick.Test()
    output = tester.foo()
    TRICK_EXPECT_EQ( output , "called foo", test_suite , "Instantiate class and capture return value" )

    tester.t = trick.Test()
    output = tester.t.foo()
    TRICK_EXPECT_EQ( output , "called foo", test_suite , "Instantiate class pointer within class and capture return value" )

######################################################################################################################

    # Standard typedeffed integer types
    test_suite = "typedef_ints"

    test_so.obj.i8t = 70 ;
    TRICK_EXPECT_EQ( test_so.obj.i8t , 70, test_suite , "int8_t" )
    trick.trick_test_add_parent( test_suite , "int8_t" , "2939597198")

    test_so.obj.ui8t = 71 ;
    TRICK_EXPECT_EQ( test_so.obj.ui8t , 71, test_suite , "uint8_t" )

    test_so.obj.i16t = 80 ;
    TRICK_EXPECT_EQ( test_so.obj.i16t , 80, test_suite , "int16_t" )

    test_so.obj.ui16t = 81 ;
    TRICK_EXPECT_EQ( test_so.obj.ui16t , 81, test_suite , "uint16_t" )

    test_so.obj.i32t = 90 ;
    TRICK_EXPECT_EQ( test_so.obj.i32t , 90, test_suite , "int32_t" )

    test_so.obj.ui32t = 91 ;
    TRICK_EXPECT_EQ( test_so.obj.ui32t , 91, test_suite , "uint32_t" )

    test_so.obj.i64t = 100 ;
    TRICK_EXPECT_EQ( test_so.obj.i64t , 100, test_suite , "int64_t" )

    test_so.obj.ui64t = 101 ;
    TRICK_EXPECT_EQ( test_so.obj.ui64t , 101, test_suite , "uint64_t" )

    test_so.obj.sizet = 111 ;
    TRICK_EXPECT_EQ( test_so.obj.sizet , 111, test_suite , "size_t" )

    test_so.obj.u_c = 121 ;
    TRICK_EXPECT_EQ( test_so.obj.u_c , 121, test_suite , "u_char" )

    test_so.obj.u_s = 131 ;
    TRICK_EXPECT_EQ( test_so.obj.u_s , 131, test_suite , "u_short" )

    test_so.obj.u_i = 141 ;
    TRICK_EXPECT_EQ( test_so.obj.u_i , 141, test_suite , "u_int" )

    test_so.obj.u_l = 151 ;
    TRICK_EXPECT_EQ( test_so.obj.u_l , 151, test_suite , "u_long" )

    test_so.obj.q = 161 ;
    TRICK_EXPECT_EQ( test_so.obj.q , 161, test_suite , "quad_t" )

    test_so.obj.uq = 171 ;
    TRICK_EXPECT_EQ( test_so.obj.uq , 171, test_suite , "u_quad_t" )

######################################################################################################################

    # Exceptions

    test_suite = "exception"

    test_case = "Array index out of bounds"
    try:
        test_so.obj.da[5] = 2.0
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

    test_case = "Double dimension array first index out of bounds"
    try:
        test_so.obj.daa[20][0] = 2.0
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

    test_case = "Double dimension array second index out of bounds"
    try:
        test_so.obj.daa[0][20] = 2.0
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

    test_case = "String too long"
    try:
        test_so.obj.ca = "dfjdslfjdsajfldjalfjdslafjdlsajfdsd"
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

    test_case = "Units mismatch"
    try:
        test_so.obj.da[2] = trick.attach_units("s" , 2.0)
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

    test_case = "unit-ed value assigned to unitless variable"
    try:
        test_so.obj.dno_units = trick.attach_units("in" , 60.6)
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

    test_case = "unit-ed value assigned to unitless integer array variable"
    try:
        test_so.obj.ia = trick.attach_units("in" , [60, 70])
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

    test_case = "unit-ed value assigned to unitless integer pointer variable"
    try:
        test_so.obj.ip = trick.attach_units("in" , [60, 70])
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EXCEPTION not tripped")
    except:
        trick.add_test_result( test_suite , test_case , "")

######################################################################################################################

    # Polymorphic assignments and access
    test_suite = "polymorphism"

    test_so.a = trick.Cat()
    TRICK_EXPECT_EQ( test_so.a.id , 1, test_suite , "single abstract ptr" )
    trick.trick_test_add_parent( test_suite , "single abstract ptr" , "1770735610")
    #test_so.a.speak()
    #test_so.a[0].speak()
    test_so.a = trick.Dog()
    TRICK_EXPECT_EQ( test_so.a.id , 2, test_suite , "single abstract ptr" )

    test_so.aarray[0] = trick.Cat()
    test_so.aarray[1] = trick.Dog()
    ids = [ test_so.aarray[0].id , test_so.aarray[1].id ]
    TRICK_EXPECT_EQ( str(ids), "[1, 2]", test_suite , "fixed array of abstract ptrs" )
    #test_so.aarray[0].speak()
    #test_so.aarray[1].speak()

    test_so.alist = trick.TMM_declare_var_1d("Abstract *", 4)

    test_so.alist[0] = trick.TMM_declare_var_s("Cat")
    test_so.alist[1] = trick.TMM_declare_var_s("Dog")
    test_so.alist[2] = trick.Cat()
    test_so.alist[3] = trick.Dog()
    ids = [ test_so.alist[0].id , test_so.alist[1].id , test_so.alist[2].id , test_so.alist[3].id ]
    TRICK_EXPECT_EQ( str(ids), "[1, 2, 1, 2]", test_suite , "fixed array of abstract ptrs" )
    #test_so.alist[0].speak()
    #test_so.alist[1].speak()
    #test_so.alist[2].speak()
    #test_so.alist[3].speak()

    # test vector of abstract pointers
    new_cat = trick.TMM_declare_var(trick.TRICK_STRUCTURED,"Cat",0,"my_cat",0,None)
    test_so.vap.push_back(new_cat)
    TRICK_EXPECT_EQ( test_so.vap[0].id , 1, test_suite , "std::vector of abstract ptrs" )

    new_dog = trick.TMM_declare_var(trick.TRICK_STRUCTURED,"Dog",0,"my_dog",0,None)
    test_so.vap.push_back(new_dog)
    TRICK_EXPECT_EQ( test_so.vap[1].id , 2, test_suite , "std::vector of abstract ptrs" )

    #test_so.vap[0].speak()
    #test_so.vap[1].speak()

    #drg0 = trick.DRAscii("cat_stuff")
    #drg0.add_variable("my_cat.id")
    #drg0.add_variable("my_dog.id")
    #drg0.set_cycle(0.1)
    #drg0.freq = trick.DR_Always
    #drg0.thisown = 0
    #trick.add_data_record_group(drg0, trick.DR_Buffer)

    test_so.vap2.push_back(new_dog)
    test_so.vap2.push_back(new_cat)

    # vector of vectors (experimental.  It does work!)
    test_so.vvap.push_back(test_so.vap)
    test_so.vvap.push_back(test_so.vap2)
    #test_so.vvap[0][0].speak()
    #test_so.vvap[0][1].speak()
    #test_so.vvap[1][0].speak()
    #test_so.vvap[1][1].speak()

######################################################################################################################

    test_suite = "array_sclicing"

    # fixed array
    test_so.obj.ia = [ 10 , 20 , 30 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ia[:]), "[10, 20, 30]", test_suite , "full slice, fixed array" )
    TRICK_EXPECT_EQ( str(test_so.obj.ia[1:]), "[20, 30]", test_suite , "slice with start value, fixed array" )
    TRICK_EXPECT_EQ( str(test_so.obj.ia[:2]), "[10, 20]", test_suite , "slice with end value, fixed array" )
    TRICK_EXPECT_EQ( str(test_so.obj.ia[::2]), "[10, 30]", test_suite , "sclice with step value, fixed array" )
    TRICK_EXPECT_EQ( str(test_so.obj.ia[-3:-1]), "[10, 20]", test_suite , "slice with negative start and end value, fixed array" )
    TRICK_EXPECT_EQ( str(test_so.obj.ia[::-2]), "[30, 10]", test_suite , "slice with negative step, fixed array" )

    test_so.obj.ia = [ 10 , 20 , 30]
    test_so.obj.ia[1:1] = 400
    TRICK_EXPECT_EQ( str(test_so.obj.ia), "[10, 400, 20]", test_suite , "slice insertion with scalar value, fixed array" )

    test_so.obj.ia = [ 10 , 20 , 30]
    test_so.obj.ia[1:1] = [400 , 500]
    TRICK_EXPECT_EQ( str(test_so.obj.ia), "[10, 400, 500]", test_suite , "slice insertion of list, fixed array" )

    test_so.obj.ia = [ 10 , 20 , 30]
    test_so.obj.ia[1:2] = 400
    TRICK_EXPECT_EQ( str(test_so.obj.ia), "[10, 400, 30]", test_suite , "slice replacement with scalar value, fixed array" )

    test_so.obj.ia = [ 10 , 20 , 30]
    test_so.obj.ia[0:1] = [400 , 500]
    TRICK_EXPECT_EQ( str(test_so.obj.ia), "[400, 500, 20]", test_suite , "slice replacement list larger than sclice, fixed array" )

    test_so.obj.ia = [ 10 , 20 , 30]
    test_so.obj.ia[0:2] = [400 , 500]
    TRICK_EXPECT_EQ( str(test_so.obj.ia), "[400, 500, 30]", test_suite , "slice replacement list same size as sclice, fixed array" )

    test_so.obj.ia = [ 10 , 20 , 30]
    test_so.obj.ia[0:3] = [400 , 500]
    TRICK_EXPECT_EQ( str(test_so.obj.ia), "[400, 500, 0]", test_suite , "slice replacement list smaller than slice, fixed array" )

    test_so.obj.ia = [ 10 , 20 , 30]
    test_so.obj.ia[:] = [400 , 500]
    TRICK_EXPECT_EQ( str(test_so.obj.ia), "[400, 500, 0]", test_suite , "slice replacement full array, fixed array" )

    test_so.obj.ia = [ 10 , 20 , 30]
    test_so.obj.ia[0:2:2] = 400
    TRICK_EXPECT_EQ( str(test_so.obj.ia), "[400, 20, 30]", test_suite , "slice replacement with scalar and step, fixed array" )

    test_so.obj.ia = [ 10 , 20 , 30]
    test_so.obj.ia[0:3:2] = [400 , 500]
    TRICK_EXPECT_EQ( str(test_so.obj.ia), "[400, 20, 500]", test_suite , "slice replacement with list and step, fixed array" )

    # pointer
    test_so.obj.ip = [ 10 , 20 , 30 , 40 ]
    TRICK_EXPECT_EQ( str(test_so.obj.ip[:]), "[10, 20, 30, 40]", test_suite , "full slice, pointer" )
    TRICK_EXPECT_EQ( str(test_so.obj.ip[1:]), "[20, 30, 40]", test_suite , "slice with start value, pointer" )
    TRICK_EXPECT_EQ( str(test_so.obj.ip[:2]), "[10, 20]", test_suite , "slice with end value, pointer" )
    TRICK_EXPECT_EQ( str(test_so.obj.ip[::2]), "[10, 30]", test_suite , "sclice with step value, pointer" )
    TRICK_EXPECT_EQ( str(test_so.obj.ip[-3:-1]), "[20, 30]", test_suite , "slice with negative start and end value, pointer" )
    TRICK_EXPECT_EQ( str(test_so.obj.ip[::-2]), "[40, 20]", test_suite , "slice with negative step, pointer" )

    test_so.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    test_so.obj.ip[1:1] = 400
    TRICK_EXPECT_EQ( str(test_so.obj.ip), "[10, 400, 20, 30, 40]", test_suite , "slice insertion with scalar value, pointer" )

    test_so.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    test_so.obj.ip[1:1] = [400 , 500]
    TRICK_EXPECT_EQ( str(test_so.obj.ip), "[10, 400, 500, 20, 30]", test_suite , "slice insertion of list, pointer" )

    test_so.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    test_so.obj.ip[1:2] = 400
    TRICK_EXPECT_EQ( str(test_so.obj.ip), "[10, 400, 30, 40, 50]", test_suite , "slice replacement with scalar value, pointer" )

    test_so.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    test_so.obj.ip[1:2] = [400 , 500]
    TRICK_EXPECT_EQ( str(test_so.obj.ip), "[10, 400, 500, 30, 40]", test_suite , "slice replacement list larger than sclice, pointer" )

    test_so.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    test_so.obj.ip[1:3] = [400 , 500]
    TRICK_EXPECT_EQ( str(test_so.obj.ip), "[10, 400, 500, 40, 50]", test_suite , "slice replacement list same size as sclice, pointer" )

    test_so.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    test_so.obj.ip[1:4] = [400 , 500]
    TRICK_EXPECT_EQ( str(test_so.obj.ip), "[10, 400, 500, 50, 0]", test_suite , "slice replacement list smaller than slice, pointer" )

    test_so.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    test_so.obj.ip[:] = [400 , 500]
    TRICK_EXPECT_EQ( str(test_so.obj.ip), "[400, 500, 0, 0, 0]", test_suite , "slice replacement full array, pointer" )

    test_so.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    test_so.obj.ip[1:3:2] = 400
    TRICK_EXPECT_EQ( str(test_so.obj.ip), "[10, 400, 30, 40, 50]", test_suite , "slice replacement with scalar and step, pointer" )

    test_so.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    test_so.obj.ip[1:4:2] = [400 , 500]
    TRICK_EXPECT_EQ( str(test_so.obj.ip), "[10, 400, 30, 500, 50]", test_suite , "slice replacement with list and step, pointer" )

    test_so.obj.ip = [ 10 , 20 , 30 , 40 , 50]
    test_so.obj.ip[1:5:2] = [400 , 500]
    TRICK_EXPECT_EQ( str(test_so.obj.ip), "[10, 400, 30, 500, 50]", test_suite , "slice replacement with list and step, pointer" )

######################################################################################################################

    test_suite = "STL list support"

    #print dir(test_so.obj.ls)

    TRICK_EXPECT_EQ( test_so.obj.ls.empty(), 1, test_suite , "STL list empty true" )
    test_so.obj.ls.push_back('string 1')
    test_so.obj.ls.push_front('string 2')
    test_so.obj.ls.push_back('string 3')
    TRICK_EXPECT_EQ( test_so.obj.ls.empty(), 0, test_suite , "STL list empty false" )
    TRICK_EXPECT_EQ( test_so.obj.ls.front(), "string 2", test_suite , "STL list front access" )
    TRICK_EXPECT_EQ( test_so.obj.ls.back(), "string 3", test_suite , "STL list back access" )
    TRICK_EXPECT_EQ( test_so.obj.ls.size(), 3, test_suite , "STL list size command" )

    #test_so.obj.ls.insert(test_so.obj.ls.begin(), 'string 4')
    #test_so.obj.ls.pop_front()
    #test_so.obj.ls.erase(test_so.obj.ls.begin())
    #for l in test_so.obj.ls:
    #   print l

######################################################################################################################

    test_suite = "STL map support"

    TRICK_EXPECT_EQ( test_so.obj.msi.empty(), 1, test_suite , "STL map empty true" )

    test_so.obj.msi['key1'] = 50
    test_so.obj.msi['key2'] = 60
    test_so.obj.msi['key3'] = 70

    TRICK_EXPECT_EQ( test_so.obj.msi.empty(), 0, test_suite , "STL map empty false" )
    TRICK_EXPECT_EQ( test_so.obj.msi['key1'], 50, test_suite , "STL map key/data insertion/access" )
    TRICK_EXPECT_EQ( str(test_so.obj.msi.keys()), "['key1', 'key2', 'key3']", test_suite , "STL map keys command" )
    TRICK_EXPECT_EQ( str(test_so.obj.msi.values()), "[50, 60, 70]", test_suite , "STL map values command" )
    TRICK_EXPECT_EQ( test_so.obj.msi.has_key('key1'), 1, test_suite , "STL map has_key true" )
    TRICK_EXPECT_EQ( test_so.obj.msi.has_key('key4'), 0, test_suite , "STL map has_key false" )
    TRICK_EXPECT_EQ( test_so.obj.msi.size(), 3, test_suite , "STL map size command" )
    #print dict(test_so.obj.msi)

######################################################################################################################

    test_suite = "Templated SimObject"

    TRICK_EXPECT_EQ( tso.t, 25, test_suite , "templated sim_object access member" )
    TRICK_EXPECT_EQ( iftso.t, 25, test_suite , "inherit from templated sim_object access member" )

######################################################################################################################

    test_suite = "Templated SimObject"
    TRICK_EXPECT_TRUE( test_so.test_true(), test_suite , "boolean function return" )
    TRICK_EXPECT_FALSE( test_so.test_false(), test_suite , "boolean function return" )

######################################################################################################################

if __name__ == "__main__":
    main()


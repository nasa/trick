import math
from trick.unit_test import *

def test():
    test_suite = "AllocatingUserDefinedTypes"

    test_case = "type_AllocTestWithArguments"

    alloc_test.atwargs_input_file = trick.AllocTestWithArguments(5, 7.0)

    TRICK_EXPECT_EQ(alloc_test.atwargs_1.some_int, 0, test_case, test_suite)
    TRICK_EXPECT_NEAR(alloc_test.atwargs_1.some_double, 0.0, 1e-6, test_case, test_suite)

    TRICK_EXPECT_EQ(alloc_test.atwargs_2.some_int, 0, test_case, test_suite)
    TRICK_EXPECT_NEAR(alloc_test.atwargs_2.some_double, 1.0, 1e-6, test_case, test_suite)

    TRICK_EXPECT_EQ(alloc_test.atwargs_3.some_int, 5, test_case, test_suite)
    TRICK_EXPECT_NEAR(alloc_test.atwargs_3.some_double, 10.0, 1e-6, test_case, test_suite)

    TRICK_EXPECT_EQ(alloc_test.atwargs_4.some_int, 4, test_case, test_suite)
    TRICK_EXPECT_NEAR(alloc_test.atwargs_4.some_double, 8.0, 1e-6, test_case, test_suite)

def main():
    trick_utest.unit_tests.enable()
    trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_tmm_alloc_args.xml" ) 
    trick_utest.unit_tests.set_test_name( "TMMAllocWithArgsTest" ) 

    trick.add_read(2.0, """test()""")
   

trick.stop(5.0)


if __name__ == "__main__":
    main()
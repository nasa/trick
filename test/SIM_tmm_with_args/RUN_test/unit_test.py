import math
from trick.unit_test import *

print(f"alloc_test.atwargs = {alloc_test}")


def test():
    test_suite = "AllocatingUserDefinedTypes"

    test_case = "type_AllocTestWithArguments"

    trick.add_read(4.0, """TRICK_EXPECT_EQ(alloc_test.atwargs.some_int, 0, test_case, test_suite)""")
    trick.add_read(4.0, """TRICK_EXPECT_NEAR(alloc_test.atwargs.some_double, 0, 1e-6, test_case, test_suite)""")

def main():
    trick_utest.unit_tests.enable()
    trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_tmm_alloc_args.xml" ) 
    trick_utest.unit_tests.set_test_name( "TMMAllocWithArgsTest" ) 

    
    
    trick.add_read(5.0, """test()""")
   



trick.stop(5.0)


if __name__ == "__main__":
    main()
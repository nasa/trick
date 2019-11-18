from trick.unit_test import *

def main():

    trick.stop(5.0)
    trick.checkpoint(2.0)

    trick_utest.unit_tests.enable()
    trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_io.xml" )
    trick_utest.unit_tests.set_test_name( "IO_Permissions" )

    test_suite = "io_permissions"

    trick.var_set("test_io.d3", 60)
    TRICK_EXPECT_NEAR( test_io.d3 , 60, 0.000001, test_suite, "var set integer")

    trick.var_set("test_io.d3", 60.1)
    TRICK_EXPECT_NEAR( test_io.d3 , 60.1, 0.000001, test_suite, "var set float")

    trick.var_set("test_io.d3", 60.1, "ft")
    TRICK_EXPECT_NEAR( test_io.d3 , 18.31848, 0.000001, test_suite, "var set float with units")

    trick.var_set("test_io.cp", "hello")
    TRICK_EXPECT_EQ( str(test_io.cp) , "hello", test_suite , "var set string" )

    trick.var_set("test_io.dp", trick.get_address("test_io.d13"))
    TRICK_EXPECT_EQ( str(test_io.dp) , "[113]", test_suite , "var set address" )

    print(test_io)

if __name__ == "__main__":
    main()



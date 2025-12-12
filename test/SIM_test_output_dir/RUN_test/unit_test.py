trick.stop(15)

from trick.unit_test import *

trick_utest.unit_tests.enable()
trick_utest.unit_tests.set_file_name(
    os.getenv("TRICK_HOME") + "/trick_test/SIM_test_output_dir.xml"
)
trick_utest.unit_tests.set_test_name("CommandLineArgumentsTests")

test_suite = "command_line_arguments"


expected_argv = [
    "./T_main_Darwin_24_test.exe",
    "RUN_test/unit_test.py",
    "-OO",
    "sim_output",
]
expected_argc = len(expected_argv)

argc = trick.command_line_args_get_argc()
argv = trick.command_line_args_get_argv()


TRICK_EXPECT_EQ(expected_argc, argc, test_suite, "Expected argument count to match")

TRICK_EXPECT_EQ(expected_argv, argv, test_suite, "Expected argument values to match")

TRICK_EXPECT_EQ(
    expected_argv[1], argv[1], test_suite, "Expected second argument to match"
)
TRICK_EXPECT_EQ(
    expected_argv[2], argv[2], test_suite, "Expected third argument to match"
)
TRICK_EXPECT_EQ(
    expected_argv[3], argv[3], test_suite, "Expected fourth argument to match"
)

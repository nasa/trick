#include "trick/CommandLineArguments.hh"

#include <cstdio>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include <gtest/gtest.h>

class CommandLineArgumentsTest : public ::testing::Test {
protected:
    Trick::CommandLineArguments cmd_args;

    void SetUp() override
    {
        // Setup runs before each test
        // Create a temporary test directory and input file for file-based tests
        mkdir("RUN_test", 0755);
        // Create a dummy input file
        FILE* fp = fopen("RUN_test/input.py", "w");
        if (fp) {
            fprintf(fp, "# Test input file\n");
            fclose(fp);
        }
    }

    void TearDown() override
    {
        // Cleanup runs after each test
        unlink("RUN_test/input.py");
        rmdir("RUN_test");
    }
};

// Test get_argc() with no arguments processed
TEST_F(CommandLineArgumentsTest, GetArgcInitiallyZero)
{
    EXPECT_EQ(cmd_args.get_argc(), 0);
}

// Test get_argc() after processing arguments
TEST_F(CommandLineArgumentsTest, GetArgcAfterProcessing)
{
    char* args[] = {
        (char*)"./S_main_Linux.exe",
        (char*)"RUN_test/input.py"
    };
    int nargs = 2;

    cmd_args.process_sim_args(nargs, args);

    EXPECT_EQ(cmd_args.get_argc(), 2);
}

// Test get_argc() with multiple arguments
TEST_F(CommandLineArgumentsTest, GetArgcMultipleArgs)
{
    char* args[] = {
        (char*)"./S_main_Linux.exe",
        (char*)"RUN_test/input.py",
        (char*)"-O",
        (char*)"output_dir",
        (char*)"--some-flag"
    };
    int nargs = 5;

    cmd_args.process_sim_args(nargs, args);

    EXPECT_EQ(cmd_args.get_argc(), 5);

    // Cleanup
    rmdir("output_dir");
}

// Test get_argv() returns empty vector initially
TEST_F(CommandLineArgumentsTest, GetArgvInitiallyEmpty)
{
    std::vector<std::string> argv = cmd_args.get_argv();
    EXPECT_TRUE(argv.empty());
    EXPECT_EQ(argv.size(), 0);
}

// Test get_argv() returns correct arguments
TEST_F(CommandLineArgumentsTest, GetArgvReturnsCorrectArgs)
{
    char* args[] = {
        (char*)"./S_main_Linux.exe",
        (char*)"RUN_test/input.py"
    };
    int nargs = 2;

    cmd_args.process_sim_args(nargs, args);
    std::vector<std::string> argv = cmd_args.get_argv();

    ASSERT_EQ(argv.size(), 2);
    EXPECT_EQ(argv[0], "./S_main_Linux.exe");
    EXPECT_EQ(argv[1], "RUN_test/input.py");
}

// Test get_argv() with multiple arguments
TEST_F(CommandLineArgumentsTest, GetArgvMultipleArgs)
{
    char* args[] = {
        (char*)"./S_main_Linux.exe",
        (char*)"RUN_test/input.py",
        (char*)"-O",
        (char*)"output_dir"
    };
    int nargs = 4;

    // Create the output directory that -O will use
    mkdir("output_dir", 0755);

    cmd_args.process_sim_args(nargs, args);
    std::vector<std::string> argv = cmd_args.get_argv();

    ASSERT_EQ(argv.size(), 4);
    EXPECT_EQ(argv[0], "./S_main_Linux.exe");
    EXPECT_EQ(argv[1], "RUN_test/input.py");
    EXPECT_EQ(argv[2], "-O");
    EXPECT_EQ(argv[3], "output_dir");

    // Cleanup
    rmdir("output_dir");
}

// Test get_argv() returns by reference (modifications affect the object)
TEST_F(CommandLineArgumentsTest, GetArgvReturnsByReference)
{
    char* args[] = {
        (char*)"./S_main_Linux.exe",
        (char*)"RUN_test/input.py"
    };
    int nargs = 2;

    cmd_args.process_sim_args(nargs, args);
    std::vector<std::string>& argv_ref = cmd_args.get_argv();

    // Modify through reference
    argv_ref.emplace_back("new_arg");

    // Verify modification persisted
    std::vector<std::string>& argv_check = cmd_args.get_argv();
    ASSERT_EQ(argv_check.size(), 3);
    EXPECT_EQ(argv_check[2], "new_arg");
}

// Test consistency between get_argc() and get_argv().size()
TEST_F(CommandLineArgumentsTest, ArgcMatchesArgvSize)
{
    char* args[] = {
        (char*)"./S_main_Linux.exe",
        (char*)"RUN_test/input.py",
        (char*)"-O",
        (char*)"output"
    };
    int nargs = 4;

    // Create the output directory that -O will use
    mkdir("output", 0755);

    cmd_args.process_sim_args(nargs, args);

    EXPECT_EQ(cmd_args.get_argc(), cmd_args.get_argv().size());

    // Cleanup
    rmdir("output");
}

// Test with single argument
TEST_F(CommandLineArgumentsTest, SingleArgument)
{
    char* args[] = {
        (char*)"./S_main_Linux.exe"
    };
    int nargs = 1;

    cmd_args.process_sim_args(nargs, args);

    EXPECT_EQ(cmd_args.get_argc(), 1);

    std::vector<std::string> argv = cmd_args.get_argv();
    ASSERT_EQ(argv.size(), 1);
    EXPECT_EQ(argv[0], "./S_main_Linux.exe");
}

// Death test: Multiple -O flags should cause exit
TEST_F(CommandLineArgumentsTest, MultipleOutputFlagsDeathTest)
{
    char* args[] = {
        (char*)"./S_main",
        (char*)"RUN_test/input.py",
        (char*)"-O",
        (char*)"output1",
        (char*)"-O",
        (char*)"output2"
    };
    int nargs = 6;

    // EXPECT_EXIT checks that the code calls exit() with status 1
    // and that stderr contains the expected error message
    EXPECT_EXIT(cmd_args.process_sim_args(nargs, args),
        ::testing::ExitedWithCode(1),
        "ERROR: Multiple -O or -OO flags found");
}

// Death test: Multiple -OO flags should cause exit
TEST_F(CommandLineArgumentsTest, MultipleOutputDirMoreFlagsDeathTest)
{
    char* args[] = {
        (char*)"./S_main",
        (char*)"RUN_test/input.py",
        (char*)"-OO",
        (char*)"output1",
        (char*)"-OO",
        (char*)"output2"
    };
    int nargs = 6;

    EXPECT_EXIT(cmd_args.process_sim_args(nargs, args),
        ::testing::ExitedWithCode(1),
        "ERROR: Multiple -O or -OO flags found");
}

// Death test: Mix of -O and -OO flags should cause exit
TEST_F(CommandLineArgumentsTest, MixedOutputFlagsDeathTest)
{
    char* args[] = {
        (char*)"./S_main",
        (char*)"RUN_test/input.py",
        (char*)"-O",
        (char*)"output1",
        (char*)"-OO",
        (char*)"output2"
    };
    int nargs = 6;

    EXPECT_EXIT(cmd_args.process_sim_args(nargs, args),
        ::testing::ExitedWithCode(1),
        "ERROR: Multiple -O or -OO flags found");
}

TEST_F(CommandLineArgumentsTest, SequentialOutputFlagsDeathTest)
{
    char* args[] = {
        (char*)"./S_main",
        (char*)"RUN_test/input.py",
        (char*)"-O",
        (char*)"-OO",
        (char*)"output1"
    };
    int nargs = 5;

    EXPECT_EXIT(cmd_args.process_sim_args(nargs, args),
        ::testing::ExitedWithCode(1),
        "ERROR: Multiple -O or -OO flags found");
}

// Death test: -O flag without directory argument should cause exit
TEST_F(CommandLineArgumentsTest, OutputFlagWithoutDirDeathTest)
{
    char* args[] = {
        (char*)"./S_main",
        (char*)"RUN_test/input.py",
        (char*)"-O"
    };
    int nargs = 3;

    EXPECT_EXIT(cmd_args.process_sim_args(nargs, args),
        ::testing::ExitedWithCode(1),
        "ERROR: No directory specified after -O or -OO argument");
}

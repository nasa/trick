#include "trick/CommandLineArguments.hh"
#include "trick/command_line_protos.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

// Helper for directory cleanup
class TempDirectory
{
    std::string path_;
    static constexpr mode_t DIR_PERMISSIONS = 0755;

public:
    explicit TempDirectory(std::string path)
        : path_(std::move(path))
    {
        mkdir(path_.c_str(), DIR_PERMISSIONS);
    }

    ~TempDirectory()
    {
        rmdir(path_.c_str());
    }

    const std::string & path() const
    {
        return path_;
    }

    // Delete copy/move to ensure cleanup happens only once
    TempDirectory(const TempDirectory &) = delete;
    TempDirectory & operator=(const TempDirectory &) = delete;
    TempDirectory(TempDirectory &&) = delete;
    TempDirectory & operator=(TempDirectory &&) = delete;
};

class CommandLineArgumentsTest : public ::testing::Test
{
protected:
    Trick::CommandLineArguments cmd_args;
    std::vector<TempDirectory *> temp_dirs_; // Track directories created in tests

    // Helper to get platform-agnostic executable name
    static const char * getTestExecutable()
    {
        const char * trick_host_cpu = std::getenv("TRICK_HOST_CPU");
        static std::string exe_name;
        if(trick_host_cpu != nullptr)
        {
            exe_name = std::string("./S_main_") + trick_host_cpu + ".exe";
        }
        else
        {
            exe_name = "./S_main_test.exe"; // Generic fallback
        }

        return exe_name.c_str();
    }

    // Helper to create temp directory with RAII cleanup
    TempDirectory * createTempDir(const std::string & path)
    {
        auto * dir = new TempDirectory(path);
        temp_dirs_.push_back(dir);
        return dir;
    }

    void SetUp() override
    {
        // Setup runs before each test
        // Create a temporary test directory and input file for file-based tests
        createTempDir("RUN_test");
        // Create a dummy input file
        FILE * fp = fopen("RUN_test/input.py", "w");
        if(fp != nullptr)
        {
            fprintf(fp, "# Test input file\n");
            fclose(fp);
        }
    }

    void TearDown() override
    {
        // Cleanup runs after each test
        unlink("RUN_test/input.py");

        // Clean up any temporary directories created during tests
        for(auto * dir : temp_dirs_)
        {
            delete dir;
        }
        temp_dirs_.clear();
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
    char * args[] = {(char *)getTestExecutable(), (char *)"RUN_test/input.py"};
    int nargs = 2;

    cmd_args.process_sim_args(nargs, args);

    EXPECT_EQ(cmd_args.get_argc(), 2);
}

// Test get_argc() with multiple arguments
TEST_F(CommandLineArgumentsTest, GetArgcMultipleArgs)
{
    auto * output_dir = createTempDir("output_dir");

    char * args[] = {(char *)getTestExecutable(),
                     (char *)"RUN_test/input.py",
                     (char *)"-O",
                     (char *)output_dir->path().c_str(),
                     (char *)"--some-flag"};
    int nargs = 5;

    cmd_args.process_sim_args(nargs, args);

    EXPECT_EQ(cmd_args.get_argc(), 5);
}

// Test get_argv() returns empty vector initially
TEST_F(CommandLineArgumentsTest, GetArgvInitiallyEmpty)
{
    std::vector<std::string> argv = cmd_args.get_argv();
    EXPECT_TRUE(argv.empty());
    EXPECT_EQ(argv.size(), 0u);
}

// Test get_argv() returns correct arguments
TEST_F(CommandLineArgumentsTest, GetArgvReturnsCorrectArgs)
{
    const char * exe = getTestExecutable();
    char * args[] = {(char *)exe, (char *)"RUN_test/input.py"};
    int nargs = 2;

    cmd_args.process_sim_args(nargs, args);
    std::vector<std::string> argv = cmd_args.get_argv();

    ASSERT_EQ(argv.size(), 2u);
    EXPECT_EQ(argv[0], exe);
    EXPECT_EQ(argv[1], "RUN_test/input.py");
}

// Test get_argv() with multiple arguments
TEST_F(CommandLineArgumentsTest, GetArgvMultipleArgs)
{
    auto * output_dir = createTempDir("output_dir");
    const char * exe = getTestExecutable();

    char * args[] = {(char *)exe, (char *)"RUN_test/input.py", (char *)"-O", (char *)output_dir->path().c_str()};
    int nargs = 4;

    cmd_args.process_sim_args(nargs, args);
    std::vector<std::string> argv = cmd_args.get_argv();

    ASSERT_EQ(argv.size(), 4u);
    EXPECT_EQ(argv[0], exe);
    EXPECT_EQ(argv[1], "RUN_test/input.py");
    EXPECT_EQ(argv[2], "-O");
    EXPECT_EQ(argv[3], output_dir->path());
}

// Test get_argv() returns by reference (modifications affect the object)
TEST_F(CommandLineArgumentsTest, GetArgvReturnsByReference)
{
    const char * exe = getTestExecutable();
    char * args[] = {(char *)exe, (char *)"RUN_test/input.py"};
    int nargs = 2;

    cmd_args.process_sim_args(nargs, args);
    std::vector<std::string> & argv_ref = cmd_args.get_argv();

    // Modify through reference
    argv_ref.emplace_back("new_arg");

    // Verify modification persisted
    std::vector<std::string> & argv_check = cmd_args.get_argv();
    ASSERT_EQ(argv_check.size(), 3u);
    EXPECT_EQ(argv_check[2], "new_arg");
}

// Test consistency between get_argc() and get_argv().size()
TEST_F(CommandLineArgumentsTest, ArgcMatchesArgvSize)
{
    auto * output = createTempDir("output");
    const char * exe = getTestExecutable();

    char * args[] = {(char *)exe, (char *)"RUN_test/input.py", (char *)"-O", (char *)output->path().c_str()};
    int nargs = 4;

    cmd_args.process_sim_args(nargs, args);

    EXPECT_EQ(static_cast<size_t>(cmd_args.get_argc()), cmd_args.get_argv().size());
}

// Test with single argument
TEST_F(CommandLineArgumentsTest, SingleArgument)
{
    const char * exe = getTestExecutable();
    char * args[] = {(char *)exe};
    int nargs = 1;

    cmd_args.process_sim_args(nargs, args);

    EXPECT_EQ(cmd_args.get_argc(), 1);

    std::vector<std::string> argv = cmd_args.get_argv();
    ASSERT_EQ(argv.size(), 1u);
    EXPECT_EQ(argv[0], exe);
}

// Death test: Multiple -O flags should cause exit
TEST_F(CommandLineArgumentsTest, MultipleOutputFlagsDeathTest)
{
    char * args[] = {(char *)"./S_main",
                     (char *)"RUN_test/input.py",
                     (char *)"-O",
                     (char *)"output1",
                     (char *)"-O",
                     (char *)"output2"};
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
    char * args[] = {(char *)"./S_main",
                     (char *)"RUN_test/input.py",
                     (char *)"-OO",
                     (char *)"output1",
                     (char *)"-OO",
                     (char *)"output2"};
    int nargs = 6;

    EXPECT_EXIT(cmd_args.process_sim_args(nargs, args),
                ::testing::ExitedWithCode(1),
                "ERROR: Multiple -O or -OO flags found");
}

// Death test: Mix of -O and -OO flags should cause exit
TEST_F(CommandLineArgumentsTest, MixedOutputFlagsDeathTest)
{
    char * args[] = {(char *)"./S_main",
                     (char *)"RUN_test/input.py",
                     (char *)"-O",
                     (char *)"output1",
                     (char *)"-OO",
                     (char *)"output2"};
    int nargs = 6;

    EXPECT_EXIT(cmd_args.process_sim_args(nargs, args),
                ::testing::ExitedWithCode(1),
                "ERROR: Multiple -O or -OO flags found");
}

TEST_F(CommandLineArgumentsTest, SequentialOutputFlagsDeathTest)
{
    char * args[] = {(char *)"./S_main", (char *)"RUN_test/input.py", (char *)"-O", (char *)"-OO", (char *)"output1"};
    int nargs = 5;

    EXPECT_EXIT(cmd_args.process_sim_args(nargs, args),
                ::testing::ExitedWithCode(1),
                "ERROR: Multiple -O or -OO flags found");
}

// Death test: -O flag without directory argument should cause exit
TEST_F(CommandLineArgumentsTest, OutputFlagWithoutDirDeathTest)
{
    char * args[] = {(char *)"./S_main", (char *)"RUN_test/input.py", (char *)"-O"};
    int nargs = 3;

    EXPECT_EXIT(cmd_args.process_sim_args(nargs, args),
                ::testing::ExitedWithCode(1),
                "ERROR: No directory specified after -O or -OO argument");
}

// Test C interface: command_line_args_get_argv()
TEST_F(CommandLineArgumentsTest, CInterfaceGetArgv)
{
    const char * exe = getTestExecutable();
    char * args[] = {(char *)exe, (char *)"RUN_test/input.py"};
    int nargs = 2;

    cmd_args.process_sim_args(nargs, args);

    // Test C interface
    int argc = command_line_args_get_argc();
    char ** argv = command_line_args_get_argv();

    ASSERT_EQ(argc, 2);
    ASSERT_NE(argv, nullptr);
    EXPECT_STREQ(argv[0], exe);
    EXPECT_STREQ(argv[1], "RUN_test/input.py");
}

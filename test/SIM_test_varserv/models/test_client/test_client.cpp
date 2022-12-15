#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <sstream>
#include <iostream>
#include <functional>
#include <cmath>

#include <gtest/gtest.h>

#define SOCKET_BUF_SIZE 20480

#define DOUBLE_TOL 1e-5

// Pretend that gtest was kind enough to provide an EXPECT_FEQ operator with a tolerance
#define EXPECT_FEQ(a,b) EXPECT_LE(fabs(a - b), DOUBLE_TOL)

class Socket {

  public:

    int max_retries = 10;

    Socket() : _initialized(false) {}
    int init(std::string hostname, int port) {
        _hostname = hostname;
        _port = port;
        int tries = 0;

        while ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 && tries < max_retries) tries++;

        if (_socket_fd < 0) {
            std::cout << "Socket connection failed" << std::endl;
            return -1;
        }

        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port); // convert to weird network byte format

        if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) { 
            std::cout << "Invalid address/ Address not supported" << std::endl; 
            return -1;
        } 

        tries = 0;
        int connection_status;

        while ((connection_status = connect(_socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0 && tries < max_retries) tries++;

        if (connection_status < 0) {
            std::cout << "Connection failed" << std::endl;
            return -1;
        }

        _initialized = true;

        return 0;
    }

    int send (std::string message) {
        // weird syntax I've never used before - since the send syscall that i'm trying to use is overloaded in this class,
        // I have to append :: to the front of it so that the compiler knows to look in the global namespace
        int success = ::send(_socket_fd, message.c_str(), message.size(), 0);
        if (success < message.size()) {
            std::cout << "Failed to send message" << std::endl;
        }
        return success;
    }

    int operator<< (std::string message) {
        return send(message);
    }

    std::string receive () {
        char buffer[SOCKET_BUF_SIZE];
        int numBytes = recv(_socket_fd, buffer, SOCKET_BUF_SIZE, 0);
        if (numBytes < 0) {
        } else if (numBytes < SOCKET_BUF_SIZE) {
            buffer[numBytes] = '\0';
        }

        return std::string(buffer);
    }

    void operator>> (std::string& ret) {
        ret = receive();
    }

    bool check_for_message_availible(long timeout_sec = 2) {
        fd_set read_fd_set;
        struct timeval timeout = { .tv_sec = timeout_sec, .tv_usec = 0 };
        FD_ZERO(&read_fd_set);
        FD_SET(_socket_fd, &read_fd_set);

        // I have one question for the designers of the interface for this syscall: why
        select(_socket_fd+1, &read_fd_set, NULL, NULL, &timeout);

        return FD_ISSET(_socket_fd, &read_fd_set);
    }

    void clear_buffered_data() {
        // Poll the socket
        if (check_for_message_availible(0)) {
            // Receive into the void if there is a message
            receive();
        }
        // otherwise no worries
    }

    void close() {
        ::close(_socket_fd);
    }

  private:
    int _port;
    std::string _hostname;
    int _socket_fd;
    bool _initialized;

};

int strcmp_IgnoringWhiteSpace(std::string s1_str, std::string s2_str) {
    const char * s1 = s1_str.c_str();
    const char * s2 = s2_str.c_str();

    int i1 = 0;
    int i2 = 0;

    while (1) {
        while ( !isgraph( s1[i1] ) && s1[i1] != '\0') { i1++; }
        while ( !isgraph( s2[i2] ) && s2[i2] != '\0') { i2++; }
        if ( s1[i1] == '\0' && s2[i2] == '\0') { return 0; }
        if ( s1[i1] != s2[i2]) {
            if (s1[i1] < s2[i2]) {
                return -1;
            } else {
                return 1;
            }
        }
        i1++; i2++;
    }
}

class VariableServerTest : public ::testing::Test {
    protected:
        VariableServerTest() {
            socket_status = socket.init("localhost", 40000);

            if (socket_status == 0) {
                std::stringstream request;
                request << "trick.var_set_client_tag(\"VSTest";
                request << numSession++;
                request << "\") \n";

                socket << request.str();
            }
        }
        ~VariableServerTest() {
            socket << "trick.var_exit()\n";
            socket.close();
        }

        Socket socket;
        int socket_status;
        
        static int numSession;
};

int VariableServerTest::numSession = 0;

TEST_F (VariableServerTest, Strings) {
    if (socket_status != 0) {
        FAIL();
    }

    std::string reply;
    socket << "trick.var_send_once(\"vsx.vst.o\")\n";
    socket >> reply;
    std::string expected("5\tYou will rejoice to hear that no disaster has accompanied the commencement of an enterprise which you have regarded with such evil forebodings. I arrived here yesterday, and my first task is to assure my dear sister of my welfare and increasing confidence in the success of my undertaking.");

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);


    expected = std::string("5\tI am already far north of London, and as I walk in the streets of Petersburgh, I feel a cold northern breeze play upon my cheeks, which braces my nerves and fills me with delight. Do you understand this feeling?");
    socket << "trick.var_send_once(\"vsx.vst.p\")\n";

    socket >> reply;

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

    // TODO: Does wchar actually work?
    // expected = std::string("5\tThis breeze, which has travelled from the regions towards which I am advancing, gives me a foretaste of those icy climes. Inspirited by this wind of promise, my daydreams become more fervent and vivid.");
    // socket << "trick.var_send_once(\"vsx.vst.q\")\n";

    // socket >> reply;

    // std::cout << "\tExpected: " << expected << "\n\tActual: " << reply << std::endl;

    // EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);
}

TEST_F (VariableServerTest, AddRemove) {
    if (socket_status != 0) {
        FAIL();
    }

    std::string reply;
    std::string expected;

    socket << "trick.var_add(\"vsx.vst.c\")\n";
    socket >> reply;
    expected = std::string("0  -1234");

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

    socket >> reply;

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

    socket << "trick.var_add(\"vsx.vst.m\")\n";
    socket >> reply;
    expected = std::string("0  -1234 1");
    
    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

    socket << "trick.var_remove(\"vsx.vst.m\")\n";
    socket >> reply;
    expected = std::string("0  -1234");
    
    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);
}

TEST_F (VariableServerTest, BadRefResponse) {
    if (socket_status != 0) {
        FAIL();
    }

    std::string reply;
    std::string expected;

    std::cerr << "The purpose of this test is to cause an error. Error messages are expected." << std::endl;

    socket << "trick.var_send_once(\"vsx.vst.no_such_variable\")\n";
    socket >> reply;
    expected = std::string("5   BAD_REF");

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);
}


TEST_F (VariableServerTest, Units) {
    if (socket_status != 0) {
        FAIL();
    }

    std::string reply;
    std::string expected;

    std::cerr << "The purpose of this test is to cause an error. Error messages are expected." << std::endl;
    socket << "trick.var_add(\"vsx.vst.c\")\ntrick.var_units(\"vsx.vst.c\",\"g\")\n";
    socket >> reply;
    expected = std::string("0  -1234");

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

    socket << "trick.var_add(\"vsx.vst.e\",\"m\")\n";
    socket >> reply;
    expected = std::string("0  -1234  -123456 {m}");

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

    socket << "trick.var_units(\"vsx.vst.e\",\"ft\")\n";
    socket >> reply;
    expected = std::string("0  -1234  -405039 {ft}");

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);
}

TEST_F (VariableServerTest, SendOnce) {
    if (socket_status != 0) {
        FAIL();
    }

    std::string reply;
    std::string expected;

    socket << "trick.var_send_once(\"vsx.vst.e\")\n";
    socket >> reply;
    expected = std::string("5  -123456");

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

    // Check that nothing is sent cyclically
    EXPECT_EQ(socket.check_for_message_availible(), false);

    socket << "trick.var_send_once(\"vsx.vst.n[0], vsx.vst.n[1], vsx.vst.n[2],\", 3)\n";
    socket >> reply;
    expected = std::string("5   0   1   2");

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

    std::cerr << "The purpose of this test is to cause an error. Error messages are expected." << std::endl;
    socket << "trick.var_send_once(\"vsx.vst.n[0], vsx.vst.n[1], vsx.vst.n[2],\", 4)\n";

    // Wrong number of variables, should not send a response but should see an error message from the Trick side
    EXPECT_EQ(socket.check_for_message_availible(), false);
}

TEST_F (VariableServerTest, Exists) {
    if (socket_status != 0) {
        FAIL();
    }

    std::string reply;
    std::string expected;

    socket << "trick.var_exists(\"vsx.vst.e\")\n";
    socket >> reply;
    expected = std::string("1  1");

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);


    socket << "trick.var_exists(\"vsx.vst.z\")\n";
    socket >> reply;
    expected = std::string("1  0");

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

}


TEST_F (VariableServerTest, Cycle) {
    if (socket_status != 0) {
        FAIL();
    }

    double cycle = 1.0;
    double tolerance = 1e-5;
    int num_cycles = 5;

    // Challenge: no loops allowed
    // I've been reading about lamdbas and when you have a hammer........

    // Test: compare the differences in the returned sim time, make sure the difference 
    // between them are equal to var_cycle
    // In copy mode VS_COPY_SCHEDULED and write mode VS_WRITE_WHEN_COPY, we should see exactly the correct copy rate
    // Use a very small tolerance to compensate for floating point error

    auto parse_message_for_sim_time = [](const std::string& message) {
        // For this case the message will be
        // 0\t<time> {s}\t
        // We only care about <time>
        std::stringstream message_stream(message);
        std::string token;
        std::getline(message_stream, token, '\t');
        std::getline(message_stream, token, ' ');
        return std::stod(token);
    };

    // Tail recursion, just for fun
    std::function<void(int, double)> compare_cycle = [&] (int n_cycles, double last_sim_time) {
        if (n_cycles <= 0)
            return;

        double sim_time = parse_message_for_sim_time(socket.receive());
        EXPECT_FEQ((sim_time - last_sim_time), cycle) << "Expected cycle: " << cycle << " Measured cycle: " << (sim_time - last_sim_time);
        compare_cycle(n_cycles-1, sim_time);
    };

    auto run_cycle_test = [&] () {
        std::string command = "trick.var_cycle(" + std::to_string(cycle) + ")\n";
        socket << command;
        // Give it a cycle to update
        socket.receive();
        double sim_time = parse_message_for_sim_time(socket.receive());
        compare_cycle(num_cycles, sim_time);
    };

    std::string command = "trick.var_set_copy_mode(1)\ntrick.var_set_write_mode(1)\ntrick.var_add(\"time\")\n";
    socket << command;
    
    cycle = 0.1;
    num_cycles = 5;
    run_cycle_test();

    cycle = 0.5;
    run_cycle_test();

    cycle = 3.0;
    num_cycles = 3;
    run_cycle_test();
}


TEST_F (VariableServerTest, Pause) {
    if (socket_status != 0) {
        FAIL();
    }
    
    std::string reply;
    std::string expected;

    socket << "trick.var_add(\"vsx.vst.f\")\ntrick.var_add(\"vsx.vst.i\")\ntrick.var_pause()\n";

    // Make sure it's paused
    EXPECT_EQ(socket.check_for_message_availible(), false);

    socket << "trick.var_send()\n";
    socket >> reply;
    expected = std::string("0  123456  1234.5677");

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

    // Make sure it doesn't send cyclically
    EXPECT_EQ(socket.check_for_message_availible(), false);

    socket << "trick.var_unpause()\n";
    socket >> reply;
    expected = std::string("0  123456  1234.5677");

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);
}

TEST_F (VariableServerTest, Freeze) {
    if (socket_status != 0) {
        FAIL();
    }
    
    std::string reply;
    std::string expected;
    int mode;
    long long frame_count;
    long long freeze_frame_count;
    
    // Constants for clarity
    const int MODE_RUN = 5;
    const int MODE_FREEZE = 1;

    // lambda capture by refence is neat
    auto parse_message_for_sim_mode_and_frames = [&](const std::string& message) {
        // For this case the message will be
        // 0\t<mode>\t<frame_count>\t<freeze_frame_count>\t
        // Sets the local variables to new values
        std::stringstream message_stream(message);
        std::string token;
        std::getline(message_stream, token, '\t');
        std::getline(message_stream, token, '\t');
        mode = std::stoi(token);
        std::getline(message_stream, token, '\t');
        frame_count = std::stoll(token);
        std::getline(message_stream, token, '\t');
        freeze_frame_count = std::stoll(token);
    };

    auto wait_for_mode_change = [&] (int expected_mode, int max_wait_iterations = 10) {
        int iteration = 0;
        while (iteration++ < max_wait_iterations && mode != expected_mode) {
            parse_message_for_sim_mode_and_frames(socket.receive());
        }
    };

    // Send the sim mode, frame_count, freeze_frame_count, and sim_time
    socket << "trick.var_add(\"trick_sys.sched.mode\")\ntrick.var_add(\"trick_sys.sched.frame_count\")\ntrick.var_add(\"trick_sys.sched.freeze_frame_count\")\n";
    parse_message_for_sim_mode_and_frames(socket.receive());

    // Sim mode should be MODE_RUN == 5
    EXPECT_EQ(mode, MODE_RUN);

    // Set to Freeze mode
    socket << "trick.exec_freeze()\n";

    // The mode takes a little bit of time to change
    wait_for_mode_change(MODE_FREEZE);

    // Sim mode should be MODE_FREEZE == 1
    ASSERT_EQ(mode, MODE_FREEZE);

    // Not sure what else to test in copy mode VS_COPY_ASYNC

    // Test VS_COPY_SCHEDULED - should see 1 message per frame
    socket << "trick.var_set_copy_mode(1)\n";
    parse_message_for_sim_mode_and_frames(socket.receive());
    int num_tests = 5;
    int prev_frame = 0;
    for (int i = 0; i < num_tests; i++) {
        prev_frame = freeze_frame_count;
        parse_message_for_sim_mode_and_frames(socket.receive());
        EXPECT_EQ(freeze_frame_count - prev_frame, 1);
    }


    // Test VS_COPY_TOP_OF_FRAME, along with freeze frame multiplier and offset
    socket << "trick.var_set_copy_mode(2)\ntrick.var_set_freeze_frame_multiple(3)\ntrick.var_set_freeze_frame_offset(1)\n";
    parse_message_for_sim_mode_and_frames(socket.receive());
    for (int i = 0; i < num_tests; i++) {
        prev_frame = freeze_frame_count;
        parse_message_for_sim_mode_and_frames(socket.receive());
        EXPECT_EQ(freeze_frame_count % 3, 1);
        EXPECT_EQ(freeze_frame_count - prev_frame, 3);
    }
    
    // Set the mode back to run or the next tests will get confused
    socket << "trick.exec_run()\n";
    socket << "trick.var_set_copy_mode(0)\n";

    // Wait for the mode to actually change
    wait_for_mode_change(MODE_RUN);

    ASSERT_EQ(mode, MODE_RUN);
}


TEST_F (VariableServerTest, CopyAndWriteModes) {
    if (socket_status != 0) {
        FAIL();
    }

    std::string reply;
    std::string expected;

    int num_tests = 5;
    double sim_time;
    int frame_count;
    std::string vars;
    std::string command;
    std::string test_vars_command = "trick.var_add(\"time\")\ntrick.var_add(\"trick_sys.sched.frame_count\")\n";

    double expected_cycle = 0.5;
    command = "trick.var_cycle(" + std::to_string(expected_cycle) + ")\n";
    socket << command;

    auto parse_message = [&](const std::string& message) {
        // For this case the message will be
        // 0\t<sim_time> {s}\t<frame_count>\t<other variables>\n
        // Sets the local variables to new values
        std::stringstream message_stream(message);
        std::string token;
        std::getline(message_stream, token, '\t');
        std::getline(message_stream, token, ' ');
        sim_time = std::stod(token);
        std::getline(message_stream, token, '\t');
        std::getline(message_stream, token, '\t');
        frame_count = std::stoll(token);
        std::getline(message_stream, token, '\n');
        vars = token;
    };

    auto spin = [&](int wait_cycles = 5) {
        socket.receive();
    };

    // Check that every combination of modes is functional
    // Check that reasonable times and frames are returned as well
    // Default is VS_COPY_ASYNC=0 and VS_WRITE_ASYNC=0
    // This mode doesn't give us any guarantees about frame or cycle consistency, so we can't test it
    // Just make sure it works

    command = test_vars_command + "trick.var_add(\"vsx.vst.a\")\ntrick.var_add(\"vsx.vst.b\")\n";
    socket << command;
    parse_message(socket.receive());

    expected = "97 98";
    EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars, expected), 0) << "Received: " << vars << " Expected: " << expected;

    // Clear out anything else that's been sent
    socket << "trick.var_pause()\n";
    socket.clear_buffered_data();

    // Copy mode 1 (VS_COPY_SCHEDULED) write mode 0 (VS_WRITE_ASYNC)
    command = "trick.var_clear()\n" + test_vars_command + "trick.var_set_copy_mode(1)\ntrick.var_add(\"vsx.vst.c\")\ntrick.var_add(\"vsx.vst.d\")\ntrick.var_unpause()\n";
    socket << command;

    // With copy mode VS_COPY_SCHEDULED and write mode VS_WRITE_ASYNC, the first reply will be all 0 since the main time to copy has not occurred yet.
    // Is this what we want? Maybe we should have more strict communication on whether the data has been staged so the first message isn't incorrect
    spin();

    // expected = "0 -1234 1234";
    // EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);   
    // std::cout << "\tExpected: " << expected << "\n\tActual: " << reply << std::endl;
    expected = "-1234 1234";
    parse_message(socket.receive());
    EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars, expected), 0) << "Received: " << vars << " Expected: " << expected;

    // Test that we see a difference of exactly expected_cycle (with a tolerance for floating point issues)
    int prev_frame = 0;
    double prev_time = 0;
    for (int i = 0; i < num_tests; i++) {  
        prev_time = sim_time;
        parse_message(socket.receive());
        EXPECT_FEQ(sim_time - prev_time, expected_cycle);
        EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars, expected), 0) << "Received: " << vars << " Expected: " << expected;
    }

    // Clear out anything else that's been sent
    socket << "trick.var_pause()\n";
    socket.clear_buffered_data();


    // Copy mode 1 (VS_COPY_SCHEDULED) write mode 1 (VS_WRITE_WHEN_COPIED)
    command = "trick.var_clear()\n" + test_vars_command + "trick.var_set_write_mode(1)\ntrick.var_add(\"vsx.vst.e\")\ntrick.var_add(\"vsx.vst.f\")\ntrick.var_unpause()\n";
    socket << command;

    parse_message(socket.receive());
    expected = "-123456 123456";
    EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars, expected), 0) << "Received: " << vars << " Expected: " << expected;

    // Test that we still see a difference of exactly expected_cycle (with a tolerance for floating point issues)
    prev_frame = 0;
    prev_time = 0;
    for (int i = 0; i < num_tests; i++) {  
        prev_time = sim_time;
        parse_message(socket.receive());
        EXPECT_FEQ(sim_time - prev_time, expected_cycle);
        EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars, expected), 0) << "Received: " << vars << " Expected: " << expected;
    }

    // Clear out anything else that's been sent
    socket << "trick.var_pause()\n";
    socket.clear_buffered_data();


    // Copy mode 2 (VS_COPY_TOP_OF_FRAME) write mode 0 (VS_WRITE_ASYNC)
    // Test frame_multiple and frame_offset as well, with stupid values
    int frame_multiple = 13*5;
    int frame_offset = 7;
    command = "trick.var_clear()\n" + test_vars_command + "trick.var_set_copy_mode(2)\ntrick.var_set_write_mode(0)\ntrick.var_set_frame_multiple(" + std::to_string(frame_multiple) + ")\ntrick.var_set_frame_offset(" + std::to_string(frame_offset) + ")\ntrick.var_add(\"vsx.vst.g\")\ntrick.var_add(\"vsx.vst.h\")\ntrick.var_unpause()\n";
    socket << command;

    // Same issue as copy mode 1 write mode 0
    spin();
    // expected = "0 -1234567 123456789";
    // EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);   
    // std::cout << "\tExpected: " << expected << "\n\tActual: " << reply << std::endl;

    parse_message(socket.receive());
    expected = "-1234567 123456789";
    EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars, expected), 0) << "Received: " << vars << " Expected: " << expected;

    // Test that we see a difference in frame_count of exactly frame_multiple with an offset of frame_offset
    prev_frame = 0;
    for (int i = 0; i < num_tests; i++) {  
        prev_frame = frame_count;
        parse_message(socket.receive());
        EXPECT_EQ(frame_count - prev_frame, frame_multiple);
        EXPECT_EQ(frame_count % frame_multiple, frame_offset);
        EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars, expected), 0) << "Received: " << vars << " Expected: " << expected;
    }

    // Clear out anything else that's been sent
    socket << "trick.var_pause()\n";
    socket.clear_buffered_data();


    // Copy mode 2 (VS_COPY_TOP_OF_FRAME) write mode 1 (VS_WRITE_WHEN_COPIED)
    frame_multiple = 17*5;
    frame_offset = 11;
    command = "trick.var_clear()\n" + test_vars_command + "trick.var_set_copy_mode(2)\ntrick.var_set_write_mode(1)\ntrick.var_set_frame_multiple(" + std::to_string(frame_multiple) + ")\ntrick.var_set_frame_offset(" + std::to_string(frame_offset) + ")\ntrick.var_add(\"vsx.vst.i\")\ntrick.var_add(\"vsx.vst.j\")\ntrick.var_unpause()\n";
    socket << command;

    expected = "1234.5677 -1234.56789";
    parse_message(socket.receive());
    EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars, expected), 0) << "Received: " << vars << " Expected: " << expected;

    // Test that we see a difference in frame_count of exactly frame_multiple with an offset of frame_offset
    prev_frame = 0;
    for (int i = 0; i < num_tests; i++) {  
        prev_frame = frame_count;
        parse_message(socket.receive());
        EXPECT_EQ(frame_count - prev_frame, frame_multiple);
        EXPECT_EQ(frame_count % frame_multiple, frame_offset);
        EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars, expected), 0) << "Received: " << vars << " Expected: " << expected;
    }

    // Clear out anything else that's been sent
    socket << "trick.var_pause()\n";
    socket.clear_buffered_data();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();

    Socket socket;
    socket.init("localhost", 40000);
    
    if (result == 0) {
        // Success
        std::cout << "Exiting peacefully!" << std::endl;
        socket << "vsx.vst.success() \n";
    } else {
        // Failure
        std::cout << "Exiting with failure!" << std::endl;
        socket << "vsx.vst.fail() \n";
    }
    socket << "trick.stop() \n";

    return result;
}
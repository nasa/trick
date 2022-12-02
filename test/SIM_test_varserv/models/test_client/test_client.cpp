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

#include <gtest/gtest.h>

#define SOCKET_BUF_SIZE 20480

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
    double tolerance = 0.01;
    int num_cycles = 5;

    // Challenge: no loops allowed
    // I've been reading about lamdbas and when you have a hammer........

    // Test: compare the differences in the returned sim time, make sure the difference 
    // between them are equal to what var_cycle has been set to within some tolerance

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
    std::function<void(int, double)> compare_cycle_forward = [&] (int n_cycles, double last_sim_time) {
        if (n_cycles <= 0)
            return;

        double sim_time = parse_message_for_sim_time(socket.receive());
        EXPECT_LE(abs((sim_time - last_sim_time) - cycle), tolerance);
        compare_cycle_forward(n_cycles-1, sim_time);
    };

    auto run_cycle_test = [&] () {
        std::string command = "trick.var_cycle(" + std::to_string(cycle) + ")\n";
        socket << command;
        double sim_time = parse_message_for_sim_time(socket.receive());
        compare_cycle_forward(num_cycles, sim_time);
    };

    std::string command = "trick.var_add(\"time\")\n";
    socket << command;

    tolerance = 0.011;
    cycle = 1.0;
    run_cycle_test();
    
    cycle = 0.5;
    run_cycle_test();

    tolerance = 0.015;
    cycle = 0.1;    
    run_cycle_test();

    num_cycles = 2;
    cycle = 3.0;
    tolerance = 0.1;
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

TEST_F (VariableServerTest, CopyAndWriteModes) {
    if (socket_status != 0) {
        FAIL();
    }

    std::string reply;
    std::string expected;

    // We're just checking that every combination of modes is functional
    // We can't test that they perform their copying and writing in the correct place from here
    // Default is 0 0
    socket << "trick.var_add(\"vsx.vst.a\")\ntrick.var_add(\"vsx.vst.b\")\n";
    socket >> reply;

    expected = "0 97 98";
    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

    // Clear out anything else that's been sent
    // I may need to write something else for this
    socket << "trick.var_pause()\n";
    socket.clear_buffered_data();

    // Copy mode 1 (VS_COPY_SCHEDULED) write mode 0 (VS_WRITE_ASYNC)
    socket << "trick.var_clear()\ntrick.var_set_copy_mode(1)\ntrick.var_add(\"vsx.vst.c\")\ntrick.var_add(\"vsx.vst.d\")\ntrick.var_unpause()\n";
    socket >> reply;

    // With copy mode VS_COPY_SCHEDULED and write mode VS_WRITE_ASYNC, the first reply will be all 0 since the main time to copy has not occurred yet.
    // Is this what we want? Maybe we should have more strict communication on whether the data has been staged so the first message isn't incorrect

    // expected = "0 -1234 1234";
    // EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);   
    // std::cout << "\tExpected: " << expected << "\n\tActual: " << reply << std::endl;

    socket >> reply;

    expected = "0 -1234 1234";
    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);   

    // Clear out anything else that's been sent
    socket << "trick.var_pause()\n";
    socket.clear_buffered_data();


    // Copy mode 1 (VS_COPY_SCHEDULED) write mode 1 (VS_WRITE_WHEN_COPIED)
    socket << "trick.var_clear()\ntrick.var_set_write_mode(1)\ntrick.var_add(\"vsx.vst.e\")\ntrick.var_add(\"vsx.vst.f\")\ntrick.var_unpause()\n";

    socket >> reply;
    expected = "0 -123456 123456";
    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);   

    socket >> reply;
    expected = "0 -123456 123456";
    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);   

    // Clear out anything else that's been sent
    socket << "trick.var_pause()\n";
    socket.clear_buffered_data();


    // Copy mode 2 (VS_COPY_TOP_OF_FRAME) write mode 0 (VS_WRITE_ASYNC)
    socket << "trick.var_clear()\ntrick.var_set_copy_mode(2)\ntrick.var_set_write_mode(0)\ntrick.var_add(\"vsx.vst.g\")\ntrick.var_add(\"vsx.vst.h\")\ntrick.var_unpause()\n";

    // Same issue as copy mode 1 write mode 0
    socket >> reply;
    // expected = "0 -1234567 123456789";
    // EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);   
    // std::cout << "\tExpected: " << expected << "\n\tActual: " << reply << std::endl;

    socket >> reply;
    expected = "0 -1234567 123456789";
    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);   

    // Clear out anything else that's been sent
    socket << "trick.var_pause()\n";
    socket.clear_buffered_data();


    // Copy mode 2 (VS_COPY_TOP_OF_FRAME) write mode 1 (VS_WRITE_WHEN_COPIED)
    socket << "trick.var_clear()\ntrick.var_set_copy_mode(2)\ntrick.var_set_write_mode(1)\ntrick.var_add(\"vsx.vst.i\")\ntrick.var_add(\"vsx.vst.j\")\ntrick.var_unpause()\n";
    socket >> reply;
    expected = "0 1234.5677 -1234.56789";
    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);   

    socket >> reply;
    expected = "0 1234.5677 -1234.56789";
    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);   

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
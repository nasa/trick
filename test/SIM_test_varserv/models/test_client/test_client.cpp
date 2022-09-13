#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <sstream>
#include <iostream>

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
        int numBytes = read(_socket_fd, buffer, SOCKET_BUF_SIZE);
        if (numBytes < 0) {
            std::cout << "Failed to read from socket" << std::endl;
        } else if (numBytes < SOCKET_BUF_SIZE) {
            buffer[numBytes] = '\0';
        }

        return std::string(buffer);
    }

    void operator>> (std::string& ret) {
        ret = receive();
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

    socket << "trick.var_add(\"vsx.vst.e\")\n";
    socket >> reply;
    expected = std::string("0  -1234 -123456");
    
    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

    socket << "trick.var_remove(\"vsx.vst.e\")\n";
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

    socket << "trick.var_send_once(\"vsx.vst.n[0], vsx.vst.n[1], vsx.vst.n[2],\", 3)\n";
    socket >> reply;
    expected = std::string("5   0   1   2");

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

    std::cerr << "The purpose of this test is to cause an error. Error messages are expected." << std::endl;
    socket << "trick.var_send_once(\"vsx.vst.n[0], vsx.vst.n[1], vsx.vst.n[2],\", 4)\n";
    //socket >> reply;
    //expected = std::string("5");

    //EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);
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

TEST_F (VariableServerTest, Pause) {
    if (socket_status != 0) {
        FAIL();
    }
    
    std::string reply;
    std::string expected;

    socket << "trick.var_add(\"vsx.vst.f\")\ntrick.var_add(\"vsx.vst.i\")\ntrick.var_pause()\n";

    // TODO: Make sure it's paused
    // socket >> reply;
    // expected = std::string("");

    // EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);


    socket << "trick.var_send()\n";
    socket >> reply;
    expected = std::string("0  123456  1234.5677");

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

    // TODO: Check that it doesn't send cyclically
    // socket >> reply;
    // expected = std::string("0  123456  1234.5677");

    // EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

    socket << "trick.var_unpause()\n";
    socket >> reply;
    expected = std::string("0  123456  1234.5677");

    EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);
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
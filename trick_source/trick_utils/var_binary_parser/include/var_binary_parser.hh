#include <unistd.h>
#include <vector>
#include <string>


class Var {
    public:
    unsigned int name_length;
    std::string name;

    uint32_t trick_type;
    unsigned int var_size;
    std::vector<unsigned char> value_bytes;

    // The closest to runtime return type polymorphism that I can think of
    template <typename T>
    T getValue();

    bool validate();
    //  {
    //     return name_length == name.size() && var_size == data.size() && trick_type <= 25;
    // }
};

class ParsedBinaryMessage {
    public:
    unsigned int message_type;
    unsigned int message_size;
    unsigned int num_vars;

    std::vector<Var> variables;
};

// Size of bytes must be 4
// int bytesToInt(unsigned char* bytes) {
//     int result = 0;
//     for (int i = 0; i < 4; i++) {
//         result |= bytes[i] << i;
//     }

//     return result;
// }


// struct ParsedMessage {
//     int message_type;
//     size_t message_size;
//     uint32_t num_variables;

//     std::vector<Var> variables;

//     int parse (const std::vector<unsigned char>& message) {
//         // putting it in a raw buffer makes it easier to manipulate
//         unsigned char buf[message.size()];
//         for (int i = 0; i < message.size(); i++) {
//             buf[i] = message.at(i);
//         }

//         message_type = bytesToInt(&buf[0]);
//         message_size = bytesToInt(&buf[4]);
//         num_variables = bytesToInt(&buf[8]);

//         std::cout << "Parsed type: " << message_type << std::endl;
//         std::cout << "Parsed size: " << message_size << std::endl;
//         std::cout << "Parsed num_variables: " << num_variables << std::endl;

        
//         return 1;
//     }

// };



// TEST_F (VariableServerTest, Binary) {
//     if (socket_status != 0) {
//         FAIL();
//     }

//     std::vector<unsigned char> reply;
//     std::vector<unsigned char> expected;

//     std::cout << "sizeof char: " << sizeof(unsigned char) << std::endl;

//     socket << "trick.var_binary()\ntrick.var_add(\"vsx.vst.c\")\ntrick.var_add(\"vsx.vst.d\")\n";

//     reply = socket.receive_bytes();
//     ParsedMessage message;
//     message.parse(reply);

//     std::cout << "Size of actual: " << reply.size() << std::endl;
//     // std::cout << "\tExpected: " << expected << "\n\tActual: ";

//     for (int i = 0; i < reply.size(); i++) {
//         std::cout << std::bitset<sizeof (unsigned char) * 8>(reply.at(i)) << " " ;
//     }

//     unsigned char message_type[4] = {reply[0], reply[1], reply[2], reply[3]};
//     std::cout << "\nMessage type: " << bytesToInt(message_type) << std::endl;

//     unsigned char message_size[4] = {reply[4], reply[5], reply[6], reply[7]};
//     std::cout << "\nMessage size: " << bytesToInt(message_size) << std::endl;

//     unsigned char message_num_vars[4] = {reply[8], reply[9], reply[10], reply[11]};
//     std::cout << "\nMessage num vars: " << bytesToInt(message_num_vars) << std::endl;

//     // std::cout << std::endl;

//     // EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

//     // socket >> reply;
//     // std::cout << "Size of actual: " << reply.size() << std::endl;

//     // EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

//     // socket << "trick.var_add(\"vsx.vst.m\")\n";
//     // socket >> reply;
//     //     std::cout << "Size of actual: " << reply.size() << std::endl;

//     // expected = std::string("0  -1234 1");
//     // std::cout << "\tExpected: " << expected << "\n\tActual: " << reply << std::endl;

//     // EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);

//     // socket << "trick.var_remove(\"vsx.vst.m\")\n";
//     // socket >> reply;
//     // std::cout << "Size of actual: " << reply.size() << std::endl;

//     // expected = std::string("0  -1234");
//     // std::cout << "\tExpected: " << expected << "\n\tActual: " << reply << std::endl;
//     // EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply, expected), 0);
// }

//     std::vector<unsigned char> receive_bytes() {
//         unsigned char buffer[SOCKET_BUF_SIZE];
//         int numBytes = recv(_socket_fd, buffer, SOCKET_BUF_SIZE, 0);
//         if (numBytes < 0) {
//             std::cout << "Failed to read from socket" << std::endl;
//         } else if (numBytes < SOCKET_BUF_SIZE) {
//             std::cout << "Recieved num bytes: " << numBytes << std::endl;
//         }

//         std::vector<unsigned char> bytes;
//         for (int i = 0; i < numBytes; i++) {
//             bytes.push_back(buffer[i]);
//         }

//         return bytes;
//     }

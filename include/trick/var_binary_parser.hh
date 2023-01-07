#include <unistd.h>
#include <vector>
#include <string>
#include <exception>

#include "trick/parameter_types.h"

class IncorrectUsageException : public std::exception 
{
    private:
        std::string _message;
    public:
        IncorrectUsageException(std::string msg) : _message(msg) {}
        const char * what() const noexcept override { return _message.c_str(); }
};

class ParseTypeException : public IncorrectUsageException
{
    public:
        ParseTypeException() : IncorrectUsageException("Mismatched trick type and template call") {}
};

class MalformedMessageException : public std::exception
{
    private:
        std::string _message;
    public:
        MalformedMessageException() : _message("Attempted to parse an incorrectly formed message") {}
        MalformedMessageException(std::string msg) : _message(msg) {}
        const char * what() const noexcept override { return _message.c_str(); }
};

typedef union Number {
    unsigned char raw_bytes[sizeof(long long)];     // Raw bytes 

    uint8_t unsigned_char_val;
    int8_t char_val;

    uint16_t unsigned_short_val;
    int16_t short_val;

    uint32_t unsigned_int_val;
    int32_t int_val;

    uint64_t unsigned_long_val;
    int64_t long_val;

    unsigned long long unsigned_long_long_val;
    long long long_long_val;

    float float_val;
    double double_val;
} Number;

    
class Var {
    public:
        Var () : _has_name(false) {};
        void setValue(const std::vector<unsigned char>& bytes, size_t size, TRICK_TYPE type, bool byteswap = false);
        void setName(size_t name_size, const std::vector<unsigned char>& name_data);

        // The closest to runtime return type polymorphism that I can think of
        // There won't be a general case
        template <typename T>
        T getValue() const;

        std::string getName() const;
        TRICK_TYPE getType() const;

    private:
        std::vector<unsigned char> value_bytes;
        Number getInterpreter () const;

        bool _has_name;
        unsigned int _name_length;
        std::string _name;

        bool _byteswap;

        TRICK_TYPE _trick_type;
        unsigned int _var_size;
};

class ParsedBinaryMessage {
    public:
        ParsedBinaryMessage() : _byteswap(false), _nonames(false) {}
        ParsedBinaryMessage (bool byteswap, bool nonames) : _byteswap(byteswap), _nonames(nonames) {}

        void combine (const ParsedBinaryMessage& message);
        
        int parse (const std::vector<unsigned char>& bytes);

        int getMessageType() const;
        unsigned int getMessageSize() const;
        unsigned int getNumVars() const;
        Var getVariable(const std::string& name);
        Var getVariable(int index);

        std::vector<Var> variables;

    private:
        static bool validateMessageType(int message_type);

        int _message_type;
        unsigned int _message_size;
        unsigned int _num_vars;

        bool _byteswap;
        bool _nonames;

        const static size_t header_size;
        const static size_t message_indicator_size;
        const static size_t variable_num_size;
        const static size_t message_size_size;
        const static size_t variable_name_length_size;
        const static size_t variable_type_size;
        const static size_t variable_size_size;
};


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

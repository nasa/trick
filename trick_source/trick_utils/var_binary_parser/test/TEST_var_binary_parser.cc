#include "trick/var_binary_parser.hh"
#include <climits>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>

// int hi = 161
std::vector<unsigned char> test_var_1 =
    {0x02, 0x00, 0x00, 0x00, 0x68, 0x69, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xa1, 0x00, 0x00, 0x00};

// string my_string = "99 red balloons"
std::vector<unsigned char> test_var_2 = {0x09, 0x00, 0x00, 0x00, 0x6D, 0x79, 0x5F, 0x73, 0x74, 0x72, 0x69, 0x6E,
                                         0x67, 0x03, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x39, 0x39, 0x20,
                                         0x72, 0x65, 0x64, 0x20, 0x62, 0x61, 0x6C, 0x6C, 0x6F, 0x6F, 0x6E, 0x73};

// Byteswapped
std::vector<unsigned char> test_var_1_byteswap =
    {0x00, 0x00, 0x00, 0x02, 0x68, 0x69, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xa1};
std::vector<unsigned char> test_var_2_byteswap = {0x00, 0x00, 0x00, 0x09, 0x6D, 0x79, 0x5F, 0x73, 0x74,
                                                  0x72, 0x69, 0x6E, 0x67, 0x00, 0x00, 0x00, 0x03, 0x00,
                                                  0x00, 0x00, 0x0F, 0x39, 0x39, 0x20, 0x72, 0x65, 0x64,
                                                  0x20, 0x62, 0x61, 0x6C, 0x6C, 0x6F, 0x6F, 0x6E, 0x73};

// noname variables
std::vector<unsigned char> test_var_1_noname = {0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xa1, 0x00, 0x00, 0x00};
std::vector<unsigned char> test_var_2_noname = {0x03, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x39, 0x39, 0x20, 0x72,
                                                0x65, 0x64, 0x20, 0x62, 0x61, 0x6C, 0x6C, 0x6F, 0x6F, 0x6E, 0x73};

TEST(BinaryParserTest, ParseMessageType)
{
    ParsedBinaryMessage message;
    std::vector<unsigned char> bytes = {0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    try
    {
        message.parse(bytes);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    EXPECT_EQ(message.getMessageType(), 5);
}

TEST(BinaryParserTest, ParseMessageSize)
{
    ParsedBinaryMessage message;
    std::vector<unsigned char> bytes = {0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    try
    {
        message.parse(bytes);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    EXPECT_EQ(message.getMessageSize(), 8);
}

TEST(BinaryParserTest, ParseNumVars)
{
    ParsedBinaryMessage message;
    std::vector<unsigned char> bytes = {0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};

    try
    {
        message.parse(bytes);
        FAIL() << "Expected a thrown exception";
    }
    catch(const MalformedMessageException & ex)
    {
        EXPECT_STREQ(ex.what(), "Message ends unexpectedly");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }

    EXPECT_EQ(message.getNumVars(), 1);
}

TEST(BinaryParserTest, ValidateHeaderSize)
{
    ParsedBinaryMessage message;
    std::vector<unsigned char> bytes = {0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00};

    try
    {
        message.parse(bytes);
        FAIL() << "Expected excpetion to be thrown";
    }
    catch(const MalformedMessageException & ex)
    {
        EXPECT_STREQ(ex.what(), "Not enough bytes in message to contain header: expected at least 12, got 11");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ValidateMessageType)
{
    ParsedBinaryMessage message;
    std::vector<unsigned char> bytes = {0xFF, 0xFF, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00};

    try
    {
        message.parse(bytes);
        FAIL() << "Expected excpetion to be thrown";
    }
    catch(const MalformedMessageException & ex)
    {
        EXPECT_STREQ(ex.what(), "Received unknown message type: 65535");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ValidateMessageSize)
{
    ParsedBinaryMessage message;
    std::vector<unsigned char> bytes =
        {0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};

    try
    {
        message.parse(bytes);
        FAIL() << "Expected excpetion to be thrown";
    }
    catch(const MalformedMessageException & ex)
    {
        EXPECT_STREQ(ex.what(), "Message size in header (8) does not match size of message received (10)");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseFirstVariableName)
{
    ParsedBinaryMessage message;
    std::vector<unsigned char> bytes = {0x01, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x01, 0x00,
                                        0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x68, 0x69, 0x06, 0x00,
                                        0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xa1, 0x00, 0x00, 0x00};

    try
    {
        message.parse(bytes);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    ASSERT_EQ(message.variables.size(), 1);
    EXPECT_EQ(message.variables[0].getName(), "hi");
}

TEST(BinaryParserTest, ParseFirstVariableType)
{
    ParsedBinaryMessage message;
    std::vector<unsigned char> bytes = {0x01, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x01, 0x00,
                                        0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x68, 0x69, 0x06, 0x00,
                                        0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xa1, 0x00, 0x00, 0x00};

    try
    {
        message.parse(bytes);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    ASSERT_EQ(message.variables.size(), 1);
    EXPECT_EQ(message.variables[0].getType(), TRICK_INTEGER);
}

TEST(BinaryParserTest, ParseFirstVariableValue)
{
    ParsedBinaryMessage message;
    std::vector<unsigned char> bytes = {0x01, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x01, 0x00,
                                        0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x68, 0x69, 0x06, 0x00,
                                        0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xa1, 0x00, 0x00, 0x00};

    try
    {
        message.parse(bytes);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    ASSERT_EQ(message.variables.size(), 1);
    EXPECT_EQ(message.variables[0].getValue<int>(), 161);
}

TEST(BinaryParserTest, HandleUnexpectedEnd)
{
    ParsedBinaryMessage message;
    std::vector<unsigned char> bytes = {0x01, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 0x01,
                                        0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x68, 0x69,
                                        0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00};

    try
    {
        message.parse(bytes);
        FAIL() << "Expected a thrown exception";
    }
    catch(const MalformedMessageException & ex)
    {
        EXPECT_STREQ(ex.what(), "Message ends unexpectedly");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseTwoVariables)
{
    ParsedBinaryMessage message;

    std::vector<unsigned char> bytes = {0x01, 0x00, 0x00, 0x00};
    char message_size = 8;

    // Messages are getting more complicated, build them programmatically

    // Calculate and push message size
    message_size += test_var_1.size();
    message_size += test_var_2.size();
    bytes.push_back(message_size);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);

    // Push number of variables
    bytes.push_back(2);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);

    // Push variables
    bytes.insert(bytes.end(), test_var_1.begin(), test_var_1.end());
    bytes.insert(bytes.end(), test_var_2.begin(), test_var_2.end());

    try
    {
        message.parse(bytes);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    ASSERT_EQ(message.variables.size(), 2);
    EXPECT_EQ(message.variables[0].getName(), "hi");
    EXPECT_EQ(message.variables[0].getType(), TRICK_INTEGER);
    EXPECT_EQ(message.variables[0].getValue<int>(), 161);
    EXPECT_EQ(message.variables[1].getName(), "my_string");
    EXPECT_EQ(message.variables[1].getType(), TRICK_STRING);
    EXPECT_EQ(message.variables[1].getValue<std::string>(), "99 red balloons");
}

TEST(BinaryParserTest, ParseVariableNoname)
{
    std::vector<unsigned char> bytes = {0x01, 0x00, 0x00, 0x00};
    char message_size = 8;

    // Messages are getting more complicated, build them programmatically
    // Calculate and push message size
    message_size += test_var_1_noname.size();
    message_size += test_var_2_noname.size();
    bytes.push_back(message_size);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);

    // Push number of variables
    bytes.push_back(2);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);

    // Push variables
    bytes.insert(bytes.end(), test_var_1_noname.begin(), test_var_1_noname.end());
    bytes.insert(bytes.end(), test_var_2_noname.begin(), test_var_2_noname.end());

    ParsedBinaryMessage message(false, true);
    try
    {
        message.parse(bytes);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    ASSERT_EQ(message.variables.size(), 2);
    EXPECT_EQ(message.variables[0].getName(), "<no name>");
    EXPECT_EQ(message.variables[0].getType(), TRICK_INTEGER);
    EXPECT_EQ(message.variables[0].getValue<int>(), 161);
    EXPECT_EQ(message.variables[1].getName(), "<no name>");
    EXPECT_EQ(message.variables[1].getType(), TRICK_STRING);
    EXPECT_EQ(message.variables[1].getValue<std::string>(), "99 red balloons");
}

TEST(BinaryParserTest, GetByNameNoname)
{
    std::vector<unsigned char> bytes = {0x01, 0x00, 0x00, 0x00};
    char message_size = 8;

    // Messages are getting more complicated, build them programmatically
    // Calculate and push message size
    message_size += test_var_1_noname.size();
    message_size += test_var_2_noname.size();
    bytes.push_back(message_size);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);

    // Push number of variables
    bytes.push_back(2);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);

    // Push variables
    bytes.insert(bytes.end(), test_var_1_noname.begin(), test_var_1_noname.end());
    bytes.insert(bytes.end(), test_var_2_noname.begin(), test_var_2_noname.end());

    ParsedBinaryMessage message(false, true);
    try
    {
        message.parse(bytes);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    try
    {
        message.getVariable("hi");
        FAIL() << "Expected exception thrown";
    }
    catch(const IncorrectUsageException & ex)
    {
        EXPECT_STREQ(ex.what(), "Cannot fetch variables by name in noname message");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown.";
    }
}

TEST(BinaryParserTest, ParseVariableByteswap)
{
    std::vector<unsigned char> bytes = {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00,
                                        0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x68, 0x69, 0x00, 0x00,
                                        0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xa1};
    ParsedBinaryMessage message(true, false);
    try
    {
        message.parse(bytes);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    ASSERT_EQ(message.variables.size(), 1);

    EXPECT_EQ(message.variables[0].getName(), "hi");
    EXPECT_EQ(message.variables[0].getType(), TRICK_INTEGER);
    EXPECT_EQ(message.variables[0].getValue<int>(), 161);
}

TEST(BinaryParserTest, ParseVariableByteswap2)
{
    std::vector<unsigned char> bytes = {0x00, 0x00, 0x00, 0x01};
    char message_size = 8;

    // Messages are getting more complicated, build them programmatically

    // Calculate and push message size
    message_size += test_var_1_byteswap.size();
    message_size += test_var_2_byteswap.size();
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(message_size);

    // Push number of variables
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(2);

    // Push variables
    bytes.insert(bytes.end(), test_var_1_byteswap.begin(), test_var_1_byteswap.end());
    bytes.insert(bytes.end(), test_var_2_byteswap.begin(), test_var_2_byteswap.end());

    ParsedBinaryMessage message(true, false);
    try
    {
        message.parse(bytes);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    ASSERT_EQ(message.variables.size(), 2);
    EXPECT_EQ(message.variables[0].getName(), "hi");
    EXPECT_EQ(message.variables[0].getType(), TRICK_INTEGER);
    EXPECT_EQ(message.variables[0].getValue<int>(), 161);
    EXPECT_EQ(message.variables[1].getName(), "my_string");
    EXPECT_EQ(message.variables[1].getType(), TRICK_STRING);
    EXPECT_EQ(message.variables[1].getValue<std::string>(), "99 red balloons");
}

TEST(BinaryParserTest, CombineMessages)
{
    ParsedBinaryMessage message1;
    ParsedBinaryMessage message2;
    std::vector<unsigned char> bytes1 = {0x01, 0x00, 0x00, 0x00};
    std::vector<unsigned char> bytes2 = {0x01, 0x00, 0x00, 0x00};

    char message_size1 = 8;
    char message_size2 = 8;

    // Messages are getting more complicated, build them programmatically

    // Calculate and push message size
    message_size1 += test_var_1.size();
    message_size2 += test_var_2.size();
    bytes1.push_back(message_size1);
    bytes1.push_back(0);
    bytes1.push_back(0);
    bytes1.push_back(0);

    bytes2.push_back(message_size2);
    bytes2.push_back(0);
    bytes2.push_back(0);
    bytes2.push_back(0);

    // Push number of variables
    bytes1.push_back(1);
    bytes1.push_back(0);
    bytes1.push_back(0);
    bytes1.push_back(0);

    bytes2.push_back(1);
    bytes2.push_back(0);
    bytes2.push_back(0);
    bytes2.push_back(0);

    // Push variables
    bytes1.insert(bytes1.end(), test_var_1.begin(), test_var_1.end());
    bytes2.insert(bytes2.end(), test_var_2.begin(), test_var_2.end());

    try
    {
        message1.parse(bytes1);
        message2.parse(bytes2);
        message1.combine(message2);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    ASSERT_EQ(message1.variables.size(), 2);
    EXPECT_EQ(message1.variables[0].getName(), "hi");
    EXPECT_EQ(message1.variables[0].getType(), TRICK_INTEGER);
    EXPECT_EQ(message1.variables[0].getValue<int>(), 161);
    EXPECT_EQ(message1.variables[1].getName(), "my_string");
    EXPECT_EQ(message1.variables[1].getType(), TRICK_STRING);
    EXPECT_EQ(message1.variables[1].getValue<std::string>(), "99 red balloons");
}

TEST(BinaryParserTest, CombineMessagesIncorrectType)
{
    ParsedBinaryMessage message1;
    ParsedBinaryMessage message2;
    std::vector<unsigned char> bytes1 = {0x01, 0x00, 0x00, 0x00};
    std::vector<unsigned char> bytes2 = {0x05, 0x00, 0x00, 0x00};

    char message_size1 = 8;
    char message_size2 = 8;

    // Messages are getting more complicated, build them programmatically

    // Calculate and push message size
    message_size1 += test_var_1.size();
    message_size2 += test_var_2.size();
    bytes1.push_back(message_size1);
    bytes1.push_back(0);
    bytes1.push_back(0);
    bytes1.push_back(0);

    bytes2.push_back(message_size2);
    bytes2.push_back(0);
    bytes2.push_back(0);
    bytes2.push_back(0);

    // Push number of variables
    bytes1.push_back(1);
    bytes1.push_back(0);
    bytes1.push_back(0);
    bytes1.push_back(0);

    bytes2.push_back(1);
    bytes2.push_back(0);
    bytes2.push_back(0);
    bytes2.push_back(0);

    // Push variables
    bytes1.insert(bytes1.end(), test_var_1.begin(), test_var_1.end());
    bytes2.insert(bytes2.end(), test_var_2.begin(), test_var_2.end());

    try
    {
        message1.parse(bytes1);
        message2.parse(bytes2);
        message1.combine(message2);
        FAIL() << "Expected a thrown exception";
    }
    catch(const MalformedMessageException & ex)
    {
        EXPECT_STREQ(ex.what(), "Trying to combine two messages with different message indicators (1 and 5)");
    }
    catch(...)
    {
        FAIL() << "Incorrect Exception thrown";
    }
}

TEST(BinaryParserTest, GetVarByName)
{
    ParsedBinaryMessage message;

    std::vector<unsigned char> bytes = {0x01, 0x00, 0x00, 0x00};
    char message_size = 8;

    // Messages are getting more complicated, build them programmatically

    // Calculate and push message size
    message_size += test_var_1.size();
    message_size += test_var_2.size();
    bytes.push_back(message_size);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);

    // Push number of variables
    bytes.push_back(2);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);

    // Push variables
    bytes.insert(bytes.end(), test_var_1.begin(), test_var_1.end());
    bytes.insert(bytes.end(), test_var_2.begin(), test_var_2.end());

    try
    {
        message.parse(bytes);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    Var var1 = message.getVariable("hi");
    EXPECT_EQ(var1.getName(), "hi");
    EXPECT_EQ(var1.getType(), TRICK_INTEGER);
    EXPECT_EQ(var1.getValue<int>(), 161);

    Var var2 = message.getVariable("my_string");
    EXPECT_EQ(var2.getName(), "my_string");
    EXPECT_EQ(var2.getType(), TRICK_STRING);
    EXPECT_EQ(var2.getValue<std::string>(), "99 red balloons");
}

TEST(BinaryParserTest, GetVarByIndex)
{
    ParsedBinaryMessage message;

    std::vector<unsigned char> bytes = {0x01, 0x00, 0x00, 0x00};
    char message_size = 8;

    // Messages are getting more complicated, build them programmatically

    // Calculate and push message size
    message_size += test_var_1.size();
    message_size += test_var_2.size();
    bytes.push_back(message_size);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);

    // Push number of variables
    bytes.push_back(2);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);

    // Push variables
    bytes.insert(bytes.end(), test_var_1.begin(), test_var_1.end());
    bytes.insert(bytes.end(), test_var_2.begin(), test_var_2.end());

    try
    {
        message.parse(bytes);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    Var var1 = message.getVariable(0);
    EXPECT_EQ(var1.getName(), "hi");
    EXPECT_EQ(var1.getType(), TRICK_INTEGER);
    EXPECT_EQ(var1.getValue<int>(), 161);

    Var var2 = message.getVariable(1);
    EXPECT_EQ(var2.getName(), "my_string");
    EXPECT_EQ(var2.getType(), TRICK_STRING);
    EXPECT_EQ(var2.getValue<std::string>(), "99 red balloons");
}

TEST(BinaryParserTest, GetVarByNameIncorrect)
{
    ParsedBinaryMessage message;

    std::vector<unsigned char> bytes = {0x01, 0x00, 0x00, 0x00};
    char message_size = 8;

    // Messages are getting more complicated, build them programmatically

    // Calculate and push message size
    message_size += test_var_1.size();
    message_size += test_var_2.size();
    bytes.push_back(message_size);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);

    // Push number of variables
    bytes.push_back(2);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);

    // Push variables
    bytes.insert(bytes.end(), test_var_1.begin(), test_var_1.end());
    bytes.insert(bytes.end(), test_var_2.begin(), test_var_2.end());

    try
    {
        message.parse(bytes);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    try
    {
        Var var1 = message.getVariable("no_such_var");
        FAIL() << "Expected exception thrown";
    }
    catch(const IncorrectUsageException & ex)
    {
        EXPECT_STREQ(ex.what(), "Variable no_such_var does not exist in this message.");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown.";
    }
}

TEST(BinaryParserTest, GetVarByIndexWrong)
{
    ParsedBinaryMessage message;

    std::vector<unsigned char> bytes = {0x01, 0x00, 0x00, 0x00};
    char message_size = 8;

    // Messages are getting more complicated, build them programmatically

    // Calculate and push message size
    message_size += test_var_1.size();
    message_size += test_var_2.size();
    bytes.push_back(message_size);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);

    // Push number of variables
    bytes.push_back(2);
    bytes.push_back(0);
    bytes.push_back(0);
    bytes.push_back(0);

    // Push variables
    bytes.insert(bytes.end(), test_var_1.begin(), test_var_1.end());
    bytes.insert(bytes.end(), test_var_2.begin(), test_var_2.end());

    try
    {
        message.parse(bytes);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    try
    {
        Var variable = message.getVariable(100);
        FAIL() << "Expected exception thrown";
    }
    catch(const IncorrectUsageException & ex)
    {
        EXPECT_STREQ(ex.what(), "Variable index 100 does not exist in this message.");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown.";
    }
}

TEST(BinaryParserTest, ArrayTest)
{
    // Message: vst.vsx.n
    std::vector<unsigned char> bytes = {0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x01, 0x00,
                                        0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00,
                                        0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00};
    ParsedBinaryMessage message(false, true);

    try
    {
        message.parse(bytes);
    }
    catch(const std::exception & ex)
    {
        FAIL() << "Exception thrown: " << ex.what();
    }

    ASSERT_EQ(message.getNumVars(), 1);
    Var arr_var = message.getVariable(0);
    EXPECT_EQ(arr_var.getArraySize(), 5);
    std::vector<unsigned char> raw_arr = arr_var.getRawBytes();
    std::vector<int> values;
    for(unsigned int i = 0; i < raw_arr.size(); i += 4)
    {
        int val = 0;
        for(unsigned int j = i; j < i + 4; j++)
        {
            val |= raw_arr[j] << j * 8;
        }
        values.push_back(val);
    }

    std::vector<int> expected = {0, 1, 2, 3, 4};
    EXPECT_EQ(values, expected);
}

/**************************************************************************
 * Var parsing type tests
 **************************************************************************/

TEST(BinaryParserTest, ParseChar)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x41};
    variable.setValue(bytes, 1, TRICK_CHARACTER, false);
    EXPECT_EQ(variable.getValue<char>(), 'A');
}

TEST(BinaryParserTest, ParseCharWrongType)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x0a, 0x00, 0x00, 0x00};
    variable.setValue(bytes, 4, TRICK_STRING, false);

    try
    {
        variable.getValue<char>();
        FAIL() << "Expected exception thrown";
    }
    catch(ParseTypeException & ex)
    {
        EXPECT_STREQ(ex.what(), "Mismatched trick type and template call");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseUnsignedChar)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x41};
    variable.setValue(bytes, 1, TRICK_UNSIGNED_CHARACTER, false);
    EXPECT_EQ(variable.getValue<unsigned char>(), 'A');
}

TEST(BinaryParserTest, ParseUnsignedCharWrongType)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x0a, 0x00, 0x00, 0x00};
    variable.setValue(bytes, 4, TRICK_STRING, false);

    try
    {
        variable.getValue<unsigned char>();
        FAIL() << "Expected exception thrown";
    }
    catch(ParseTypeException & ex)
    {
        EXPECT_STREQ(ex.what(), "Mismatched trick type and template call");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseBool)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x01};
    variable.setValue(bytes, 1, TRICK_BOOLEAN, false);
    EXPECT_EQ(variable.getValue<bool>(), true);
}

TEST(BinaryParserTest, ParseBoolWrongType)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x0a};
    variable.setValue(bytes, 1, TRICK_STRING, false);

    try
    {
        variable.getValue<bool>();
        FAIL() << "Expected exception thrown";
    }
    catch(ParseTypeException & ex)
    {
        EXPECT_STREQ(ex.what(), "Mismatched trick type and template call");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseShort)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x00, 0x80};
    variable.setValue(bytes, 2, TRICK_SHORT, false);

    EXPECT_EQ(variable.getValue<short>(), INT16_MIN);
}

TEST(BinaryParserTest, ParseShortWrongType)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x0a, 0x00, 0x00, 0x00};
    variable.setValue(bytes, 4, TRICK_STRING, false);

    try
    {
        variable.getValue<short>();
        FAIL() << "Expected exception thrown";
    }
    catch(ParseTypeException & ex)
    {
        EXPECT_STREQ(ex.what(), "Mismatched trick type and template call");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseUnsignedShort)
{
    Var variable;

    std::vector<unsigned char> bytes = {0xFF, 0xFF};
    variable.setValue(bytes, 2, TRICK_UNSIGNED_SHORT, false);

    EXPECT_EQ(variable.getValue<unsigned short>(), UINT16_MAX);
}

TEST(BinaryParserTest, ParseUnsignedShortWrongType)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x0a, 0x00, 0x00, 0x00};
    variable.setValue(bytes, 4, TRICK_STRING, false);

    try
    {
        variable.getValue<unsigned short>();
        FAIL() << "Expected exception thrown";
    }
    catch(ParseTypeException & ex)
    {
        EXPECT_STREQ(ex.what(), "Mismatched trick type and template call");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseInt)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x01, 0xFF, 0xFF, 0xFF};
    variable.setValue(bytes, 4, TRICK_INTEGER, false);

    EXPECT_EQ(variable.getValue<int>(), -255);
}

TEST(BinaryParserTest, ParseIntWrongType)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x0a, 0x00, 0x00, 0x00};
    variable.setValue(bytes, 4, TRICK_STRING, false);

    try
    {
        variable.getValue<int>();
        FAIL() << "Expected exception thrown";
    }
    catch(ParseTypeException & ex)
    {
        EXPECT_STREQ(ex.what(), "Mismatched trick type and template call");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseUnsignedInt)
{
    Var variable;

    std::vector<unsigned char> bytes = {0x01, 0xFF, 0xFF, 0xFF};
    variable.setValue(bytes, 4, TRICK_UNSIGNED_INTEGER, false);

    EXPECT_EQ(variable.getValue<unsigned int>(), 4294967041);
}

TEST(BinaryParserTest, ParseUnsignedIntWrongType)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x0a, 0x00, 0x00, 0x00};
    variable.setValue(bytes, 4, TRICK_STRING, false);

    try
    {
        variable.getValue<unsigned int>();
        FAIL() << "Expected exception thrown";
    }
    catch(ParseTypeException & ex)
    {
        EXPECT_STREQ(ex.what(), "Mismatched trick type and template call");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseLong)
{
    // Var variable;

    // // This is hard bc
    // std::vector<unsigned char> bytes = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80};
    // variable.setValue(bytes, 8, TRICK_LONG, false);

    // EXPECT_EQ(variable.getValue<long>(), -9151314442816847873);

    Var variable;

    // apparently this can be different by platform so we need to be careful here
    size_t long_size = sizeof(long);
    std::vector<unsigned char> bytes;
    for(unsigned int i = 0; i < long_size - 1; i++)
    {
        bytes.push_back(0x00);
    }
    bytes.push_back(0x80);

    variable.setValue(bytes, long_size, TRICK_LONG, false);

    EXPECT_EQ(variable.getValue<long>(), LONG_MIN);
}

TEST(BinaryParserTest, ParseLongWrongType)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x0a, 0x00, 0x00, 0x00};
    variable.setValue(bytes, 4, TRICK_STRING, false);

    try
    {
        variable.getValue<long>();
        FAIL() << "Expected exception thrown";
    }
    catch(ParseTypeException & ex)
    {
        EXPECT_STREQ(ex.what(), "Mismatched trick type and template call");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseUnsignedLong)
{
    Var variable;

    // apparently this can be different by platform so we need to be careful here
    size_t long_size = sizeof(unsigned long);
    std::vector<unsigned char> bytes;
    for(unsigned int i = 0; i < long_size - 1; i++)
    {
        bytes.push_back(0xFF);
    }
    bytes.push_back(0x7F);

    variable.setValue(bytes, long_size, TRICK_UNSIGNED_LONG, false);

    EXPECT_EQ(variable.getValue<unsigned long>(), LONG_MAX);
}

TEST(BinaryParserTest, ParseUnsignedLongWrongType)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x0a, 0x00, 0x00, 0x00};
    variable.setValue(bytes, 4, TRICK_STRING, false);

    try
    {
        variable.getValue<unsigned long>();
        FAIL() << "Expected exception thrown";
    }
    catch(ParseTypeException & ex)
    {
        EXPECT_STREQ(ex.what(), "Mismatched trick type and template call");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseLongLong)
{
    Var variable;

    // apparently this can be different by platform so we need to be careful here
    size_t long_long_size = sizeof(long long);
    std::vector<unsigned char> bytes;
    for(unsigned int i = 0; i < long_long_size - 1; i++)
    {
        bytes.push_back(0x00);
    }
    bytes.push_back(0x80);

    variable.setValue(bytes, long_long_size, TRICK_LONG_LONG, false);

    EXPECT_EQ(variable.getValue<long long>(), LLONG_MIN);
}

TEST(BinaryParserTest, ParseLongLongWrongType)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x0a, 0x00, 0x00, 0x00};
    variable.setValue(bytes, 4, TRICK_STRING, false);

    try
    {
        variable.getValue<long long>();
        FAIL() << "Expected exception thrown";
    }
    catch(ParseTypeException & ex)
    {
        EXPECT_STREQ(ex.what(), "Mismatched trick type and template call");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseUnsignedLongLong)
{
    Var variable;

    // apparently this can be different by platform so we need to be careful here
    size_t long_long_size = sizeof(unsigned long long);
    std::vector<unsigned char> bytes;
    for(unsigned int i = 0; i < long_long_size - 1; i++)
    {
        bytes.push_back(0xFF);
    }
    bytes.push_back(0x7F);

    variable.setValue(bytes, long_long_size, TRICK_UNSIGNED_LONG_LONG, false);

    EXPECT_EQ(variable.getValue<unsigned long long>(), LLONG_MAX);
}

TEST(BinaryParserTest, ParseUnsignedLongLongWrongType)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x0a, 0x00, 0x00, 0x00};
    variable.setValue(bytes, 4, TRICK_STRING, false);

    try
    {
        variable.getValue<unsigned long long>();
        FAIL() << "Expected exception thrown";
    }
    catch(ParseTypeException & ex)
    {
        EXPECT_STREQ(ex.what(), "Mismatched trick type and template call");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseFloat)
{
    Var variable;

    // 40 86 66 66
    std::vector<unsigned char> bytes = {0x66, 0x66, 0x86, 0x40};
    variable.setValue(bytes, 4, TRICK_FLOAT, false);

    // This is supposed to be 4.2, but floating point is gonna be the way it is
    float expected = 4.19999980926513671875;
    EXPECT_EQ(variable.getValue<float>(), expected);
}

TEST(BinaryParserTest, ParseFloatWrongType)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x0a, 0x00, 0x00, 0x00};
    variable.setValue(bytes, 4, TRICK_STRING, false);

    try
    {
        variable.getValue<float>();
        FAIL() << "Expected exception thrown";
    }
    catch(ParseTypeException & ex)
    {
        EXPECT_STREQ(ex.what(), "Mismatched trick type and template call");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseDouble)
{
    Var variable;

    std::vector<unsigned char> bytes = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0xc0};
    variable.setValue(bytes, 8, TRICK_DOUBLE, false);

    // This is supposed to be 4.2, but floating point is gonna be the way it is
    double expected = -5.0;
    EXPECT_EQ(variable.getValue<double>(), expected);
}

TEST(BinaryParserTest, ParseDoubleWrongType)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0xc0};
    variable.setValue(bytes, 8, TRICK_STRING, false);

    try
    {
        variable.getValue<double>();
        FAIL() << "Expected exception thrown";
    }
    catch(ParseTypeException & ex)
    {
        EXPECT_STREQ(ex.what(), "Mismatched trick type and template call");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseString)
{
    Var variable;

    std::vector<unsigned char> bytes = {0x48,
                                        0x69,
                                        0x20,
                                        0x74,
                                        0x68,
                                        0x69,
                                        0x73,
                                        0x20,
                                        0x69,
                                        0x73,
                                        0x20,
                                        0x61,
                                        0x20,
                                        0x73,
                                        0x74,
                                        0x72,
                                        0x69,
                                        0x6e,
                                        0x67};
    variable.setValue(bytes, 19, TRICK_STRING, false);

    // This is supposed to be 4.2, but floating point is gonna be the way it is
    std::string expected = "Hi this is a string";
    EXPECT_EQ(variable.getValue<std::string>(), expected);
}

TEST(BinaryParserTest, ParseStringWrongType)
{
    Var variable;
    std::vector<unsigned char> bytes = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0xc0};
    variable.setValue(bytes, 8, TRICK_INTEGER, false);

    try
    {
        variable.getValue<std::string>();
        FAIL() << "Expected exception thrown";
    }
    catch(ParseTypeException & ex)
    {
        EXPECT_STREQ(ex.what(), "Mismatched trick type and template call");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

TEST(BinaryParserTest, ParseWChar)
{
    Var variable;

    wchar_t test_wchar = L'J';
    std::vector<unsigned char> bytes;

    for(unsigned int i = 0; i < sizeof(wchar_t); i++)
    {
        bytes.push_back((unsigned char)((test_wchar >> (i * 8)) & 0xFF));
    }

    variable.setValue(bytes, sizeof(wchar_t), TRICK_WCHAR, false);

    EXPECT_EQ(variable.getValue<wchar_t>(), test_wchar);
}

TEST(BinaryParserTest, ParseWCharWrongType)
{
    Var variable;
    wchar_t test_wchar = L'J';
    std::vector<unsigned char> bytes;

    for(unsigned int i = 0; i < sizeof(wchar_t); i++)
    {
        bytes.push_back((unsigned char)((test_wchar >> (i * 8)) & 0xFF));
    }

    variable.setValue(bytes, 8, TRICK_INTEGER, false);

    try
    {
        variable.getValue<wchar_t>();
        FAIL() << "Expected exception thrown";
    }
    catch(ParseTypeException & ex)
    {
        EXPECT_STREQ(ex.what(), "Mismatched trick type and template call");
    }
    catch(...)
    {
        FAIL() << "Incorrect exception thrown";
    }
}

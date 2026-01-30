#include <exception>
#include <map>
#include <string>
#include <unistd.h>
#include <vector>

#include "trick/parameter_types.h"

class IncorrectUsageException : public std::exception
{
private:
    std::string _message;

public:
    IncorrectUsageException(std::string msg)
        : _message(msg)
    {
    }

    const char * what() const noexcept override
    {
        return _message.c_str();
    }
};

class ParseTypeException : public IncorrectUsageException
{
public:
    ParseTypeException()
        : IncorrectUsageException("Mismatched trick type and template call")
    {
    }
};

class MalformedMessageException : public std::exception
{
private:
    std::string _message;

public:
    MalformedMessageException()
        : _message("Attempted to parse an incorrectly formed message")
    {
    }

    MalformedMessageException(std::string msg)
        : _message(msg)
    {
    }

    const char * what() const noexcept override
    {
        return _message.c_str();
    }
};

typedef union Number
{
    unsigned char raw_bytes[sizeof(long long)]; // Raw bytes

    uint8_t unsigned_char_val;
    int8_t char_val;
    bool bool_val;

    wchar_t wchar_val;
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

const static std::map<TRICK_TYPE, unsigned int> type_size_map = {
    {         TRICK_CHARACTER,           sizeof(char)},
    {TRICK_UNSIGNED_CHARACTER,  sizeof(unsigned char)},
    {             TRICK_SHORT,          sizeof(short)},
    {    TRICK_UNSIGNED_SHORT, sizeof(unsigned short)},
    {           TRICK_INTEGER,            sizeof(int)},
    {  TRICK_UNSIGNED_INTEGER,   sizeof(unsigned int)},
    {              TRICK_LONG,           sizeof(long)},
    {     TRICK_UNSIGNED_LONG,  sizeof(unsigned long)},
    {         TRICK_LONG_LONG,      sizeof(long long)},
    {TRICK_UNSIGNED_LONG_LONG,  sizeof(unsigned long)},
    {             TRICK_FLOAT,          sizeof(float)},
    {            TRICK_DOUBLE,         sizeof(double)},
    {           TRICK_BOOLEAN,           sizeof(bool)},
    {             TRICK_WCHAR,        sizeof(wchar_t)}
};

class Var
{
public:
    Var()
        : _has_name(false)
    {
    }

    void setValue(const std::vector<unsigned char> & bytes, size_t size, TRICK_TYPE type, bool byteswap = false);
    void setName(size_t name_size, const std::vector<unsigned char> & name_data);

    // The closest to runtime return type polymorphism that I can think of
    // There won't be a general case
    template<typename T> T getValue() const;
    std::vector<unsigned char> getRawBytes() const;

    int getArraySize() const;
    std::string getName() const;
    TRICK_TYPE getType() const;

private:
    std::vector<unsigned char> value_bytes;
    Number getInterpreter() const;

    bool _has_name;
    unsigned int _name_length;
    std::string _name;

    bool _byteswap;

    TRICK_TYPE _trick_type;
    unsigned int _var_size;
    unsigned int _arr_length;
};

class ParsedBinaryMessage
{
public:
    ParsedBinaryMessage()
        : ParsedBinaryMessage(false, false)
    {
    }

    ParsedBinaryMessage(bool byteswap, bool nonames)
        : _message_type(0),
          _message_size(0),
          _num_vars(0),
          _byteswap(byteswap),
          _nonames(nonames)
    {
    }

    void combine(const ParsedBinaryMessage & message);

    int parse(const std::vector<unsigned char> & bytes);

    int getMessageType() const;
    unsigned int getMessageSize() const;
    unsigned int getNumVars() const;
    Var getVariable(const std::string & name);
    Var getVariable(unsigned int index);

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

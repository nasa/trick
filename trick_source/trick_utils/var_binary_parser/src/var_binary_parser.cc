#include "trick/var_binary_parser.hh"
#include "trick/variable_server_message_types.h"
#include <climits>
#include <iostream>
#include <sstream>


/**************************************************************************
 * Var implementation
**************************************************************************/


void Var::setName(size_t name_size, const std::vector<unsigned char>& name_data) {
    _has_name = true;
    _name_length = name_size;
    std::stringstream ss;

    for (unsigned char character : name_data) {
        ss << static_cast<char>(character);
    }

    _name = ss.str();
}


void Var::setValue(const std::vector<unsigned char>& bytes, size_t size, TRICK_TYPE type, bool byteswap) {
    _trick_type = type;
    _var_size = size;
    _byteswap = byteswap;

    value_bytes = std::vector<unsigned char> (bytes);
}

Number Var::getInterpreter() const {
    Number interpreted_val;

    if (_byteswap) {
        for (unsigned int i = 0; i < _var_size; i++) {
            interpreted_val.raw_bytes[i] = value_bytes[_var_size - 1 - i];
        }
    } else {
        for (unsigned int i = 0; i < _var_size; i++) {
            interpreted_val.raw_bytes[i] = value_bytes[i];
        }
    }

    return interpreted_val;
}

std::string Var::getName() const {
    if (_has_name) {
        return _name;
    } else {
        return std::string ("<no name>");
    }
}

TRICK_TYPE Var::getType() const {
    return _trick_type;
}

std::vector<unsigned char> Var::getRawBytes() const {
    return value_bytes;
}

int Var::getArraySize() const {
    auto val_size_pair = type_size_map.find(_trick_type);
    if (val_size_pair == type_size_map.end()) {
        return -1;
    }

    int val_size = val_size_pair->second;
    if (_var_size % val_size != 0) {
        return -1;
    }

    return _var_size / val_size;
}


// Template specialization for each supported type
template<>
char Var::getValue<char> () const {
    if (_trick_type != TRICK_CHARACTER) {
        throw ParseTypeException();
    }

    return getInterpreter().char_val;
}

template<>
unsigned char Var::getValue<unsigned char> () const {
    if (_trick_type != TRICK_UNSIGNED_CHARACTER) {
        throw ParseTypeException();
    }

    return getInterpreter().unsigned_char_val;
}

template<>
bool Var::getValue<bool> () const {
    if (_trick_type != TRICK_BOOLEAN) {
        throw ParseTypeException();
    }

    return getInterpreter().bool_val;
}

template<>
short Var::getValue<short> () const {
    if (_trick_type != TRICK_SHORT) {
        throw ParseTypeException();
    }

    return getInterpreter().short_val;
}

template<>
unsigned short Var::getValue<unsigned short> () const {
    if (_trick_type != TRICK_UNSIGNED_SHORT) {
        throw ParseTypeException();
    }

    return getInterpreter().unsigned_short_val;
}

template<>
unsigned int Var::getValue<unsigned int> () const {
    if (_trick_type != TRICK_UNSIGNED_INTEGER) {
        throw ParseTypeException();
    }

    return getInterpreter().unsigned_int_val;
}

template<>
int Var::getValue<int> () const {
    if (_trick_type != TRICK_INTEGER) {
        throw ParseTypeException();
    }

    return getInterpreter().int_val;
}

template<>
long Var::getValue<long> () const {
    if (_trick_type != TRICK_LONG) {
        throw ParseTypeException();
    }

    return getInterpreter().long_val;
}

template<>
unsigned long Var::getValue<unsigned long> () const {
    if (_trick_type != TRICK_UNSIGNED_LONG) {
        throw ParseTypeException();
    }

    return getInterpreter().unsigned_long_val;
}

template<>
long long Var::getValue<long long> () const {
    if (_trick_type != TRICK_LONG_LONG) {
        throw ParseTypeException();
    }

    return getInterpreter().long_long_val;
}

template<>
unsigned long long Var::getValue<unsigned long long> () const {
    if (_trick_type != TRICK_UNSIGNED_LONG_LONG) {
        throw ParseTypeException();
    }

    return getInterpreter().unsigned_long_long_val;
}

template<>
float Var::getValue<float> () const {
    if (_trick_type != TRICK_FLOAT) {
        throw ParseTypeException();
    }

    return getInterpreter().float_val;
}

template<>
double Var::getValue<double> () const {
    if (_trick_type != TRICK_DOUBLE) {
        throw ParseTypeException();
    }

    return getInterpreter().double_val;
}

template <>
std::string Var::getValue<std::string> () const {
    if (_trick_type != TRICK_STRING) {
        throw ParseTypeException();
    }  

    std::stringstream stream;
    for (unsigned char character : value_bytes) {
        stream << static_cast<char>(character);
    }

    return stream.str();
}

template <>
wchar_t Var::getValue<wchar_t> () const {
    if (_trick_type != TRICK_WCHAR) {
        throw ParseTypeException();
    }  

    return getInterpreter().wchar_val;
}



// Utility method
uint64_t bytesToInt(const std::vector<unsigned char>& bytes, bool byteswap) {
    uint64_t result = 0;

    if (byteswap) {
        for (unsigned int i = 0; i < bytes.size(); i++) {
            result |= bytes[bytes.size() - 1 - i] << i*8;
        }
    } else {
        for (unsigned int i = 0; i < bytes.size(); i++) {
            result |= bytes[i] << i*8;
        }
    }

    return result;
}

class MessageIterator {
    public:
        MessageIterator(const std::vector<unsigned char>& container) : _container(container), _index(0) {}

        std::vector<unsigned char> slice(unsigned int length) {
            unsigned int slice_end = _index + length;
            if (_index > _container.size() || slice_end > _container.size()) {
                throw MalformedMessageException("Message ends unexpectedly");
            }
            
            return std::vector<unsigned char>(_container.begin() + _index, _container.begin() + slice_end);
        }

        void operator+= (int n) {
            _index += n;
        } 

    private:
        std::vector<unsigned char> _container;
        unsigned int _index; 
};

/**************************************************************************
 * Var implementation
**************************************************************************/

const size_t ParsedBinaryMessage::header_size = 12;
const size_t ParsedBinaryMessage::message_indicator_size = 4;
const size_t ParsedBinaryMessage::variable_num_size = 4;
const size_t ParsedBinaryMessage::message_size_size = 4;
const size_t ParsedBinaryMessage::variable_name_length_size = 4;
const size_t ParsedBinaryMessage::variable_type_size = 4;
const size_t ParsedBinaryMessage::variable_size_size = 4;

int ParsedBinaryMessage::parse (const std::vector<unsigned char>& bytes) {
    if (bytes.size() < header_size) {
        throw MalformedMessageException(std::string("Not enough bytes in message to contain header: expected at least 12, got " + std::to_string(bytes.size())));
    }

    MessageIterator messageIterator (bytes);

    // First 4 bytes is message type
    _message_type = bytesToInt(messageIterator.slice(message_indicator_size), _byteswap);
    if (!validateMessageType(_message_type)) {
        throw MalformedMessageException(std::string("Received unknown message type: " + std::to_string(_message_type)));
    }

    messageIterator += message_indicator_size;

    // Next 4 bytes is message size
    _message_size = bytesToInt(messageIterator.slice(message_size_size), _byteswap);
    if (bytes.size() - message_indicator_size != _message_size) {
        std::string error_message = "Message size in header (" + std::to_string(_message_size) + ") does not match size of message received (" + std::to_string(bytes.size() - message_indicator_size) + ")";
        throw MalformedMessageException(error_message);
    }

    messageIterator += message_size_size;

    // Next 4 bytes is number of variables
    _num_vars = bytesToInt(messageIterator.slice(variable_num_size), _byteswap);
    messageIterator += variable_num_size;

    // Pull out all of the variables
    for (unsigned int i = 0; i < _num_vars; i++) {
        Var variable;

        if (!_nonames) {
            // Get the name
            size_t name_length = bytesToInt(messageIterator.slice(variable_name_length_size), _byteswap);
            messageIterator += variable_name_length_size;

            variable.setName(name_length, messageIterator.slice(name_length));
            messageIterator += name_length;
        } 

        // Parse the type first
        int var_type = bytesToInt(messageIterator.slice(variable_type_size), _byteswap);
        messageIterator += variable_type_size;

        size_t var_size = bytesToInt(messageIterator.slice(variable_size_size), _byteswap);
        messageIterator += variable_size_size;

        variable.setValue(messageIterator.slice(var_size), var_size, static_cast<TRICK_TYPE>(var_type), _byteswap);
        messageIterator += var_size;

        variables.emplace_back(variable);
    }

    return 0;
}

void ParsedBinaryMessage::combine (const ParsedBinaryMessage& other) {
    if (_message_type != other._message_type) {
        std::string error_message = "Trying to combine two messages with different message indicators (" + std::to_string(_message_type) + " and " + std::to_string(other._message_type) + ")";
        throw MalformedMessageException(error_message);
    }

    // Combined size - subtract the header size from other message size
    _message_size += other._message_size - message_size_size - variable_num_size;
    
    // Combine variables
    _num_vars += other._num_vars;
    variables.insert(variables.end(), other.variables.begin(), other.variables.end());

    // Other error checking - duplicate variables?
}

Var ParsedBinaryMessage::getVariable(const std::string& name) {
    if (_nonames) 
        throw IncorrectUsageException("Cannot fetch variables by name in noname message");

    for (auto variable : variables) {
        if (variable.getName() == name) 
            return variable;
    }

    throw IncorrectUsageException("Variable " + name + " does not exist in this message.");
}

Var ParsedBinaryMessage::getVariable(unsigned int index) {
    if (index >= variables.size()) {
        throw IncorrectUsageException("Variable index " + std::to_string(index) + " does not exist in this message.");
    }

    return variables[index];
}

int ParsedBinaryMessage::getMessageType() const {
    return _message_type;
}

unsigned int ParsedBinaryMessage::getMessageSize() const {
    return _message_size;
}

unsigned int ParsedBinaryMessage::getNumVars() const {
    return _num_vars; 
}

// Static methods

bool ParsedBinaryMessage::validateMessageType(int message_type) {
    return message_type >= VS_MIN_CODE && message_type <= VS_MAX_CODE;
}

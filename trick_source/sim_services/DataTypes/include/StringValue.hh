#ifndef STRING_VALUE_H
#define STRING_VALUE_H

#include "Value.hh"

/**
 */
class StringValue : public Value {

    public:

    /**
     Constructor.
     @param value The value of this StringValue.
     */
    StringValue(const char* value);

    /**
     Print this StringValue to a stream.
     */
    void print(std::ostream &s);

    /**
     Get a string representation of this StringValue.
     */
    std::string toString();

    private:
    std::string value;
};

#endif

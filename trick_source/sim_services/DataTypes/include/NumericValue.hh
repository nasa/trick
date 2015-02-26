#ifndef NUMERIC_VALUE_H
#define NUMERIC_VALUE_H

#include "Value.hh"

/**
 */
class NumericValue : public Value {

    public:

    /**
     Get the value of this NumericValue as a floating point value.
     @return the value as a double.
     */
    virtual double getFloatingPointValue() = 0;

    /**
     Get the value of this IntegerValue as an integer.
     @return the value as a long long int.
     */
    virtual long long getIntegerValue() = 0;
};

#endif

#ifndef FLOATING_POINT_VALUE_H
#define FLOATING_POINT_VALUE_H

#include "NumericValue.hh"

/**
 */
class FloatingPointValue : public NumericValue {

    public:

    /**
     Constructor.
     @param value The value of this FloatingPointValue.
     */
    FloatingPointValue(double value);

    /**
     Get the value of this FloatingPointValue as floating point.
     @return the value as a double.
     */
    double getFloatingPointValue();

    /**
     Get the value of this FloatingPointValue as an integer.
     @return the value as a long long int.
     */
    long long getIntegerValue();

    /**
     Print this FloatingPointValue to a stream.
     */
    void print(std::ostream &s);

    /**
     Get a string representation of this FloatingPointValue.
     */
    std::string toString();


    private:
    double value;
};
#endif

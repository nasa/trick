#ifndef PRIMITIVE_DATA_TYPE_HH
#define PRIMITIVE_DATA_TYPE_HH

#include "DataType.hh"
#include "IntegerValue.hh"
#include "FloatingPointValue.hh"

#include <stdlib.h>
#include <iostream>

/**
 */
template <class T>
class PrimitiveDataType : public DataType {

public:

    /**
     Constructor for PrimitiveDataType.
     */
    PrimitiveDataType() {}
    ~PrimitiveDataType() {}

    /* ==================================================================== */
    /*                          VIRTUAL INTERFACE                           */
    /* ==================================================================== */

    /**
     */
    bool validate() { return true; }

    /**
     @return The size (in bytes) of an instance of the DataType
     */
    size_t getSize() const {
        return sizeof(T);
    }

    TypeClass::e getTypeClass() const {
        return TypeClass::PRIMITIVE;
    }

    /**
     */
    DataType * clone () const {
        return new PrimitiveDataType<T>();
    }

    /**
     @return an instance of the type that this PrimiitveDataType Class describes.
     */
    void* createInstance(unsigned int num) const {
        T* temp = (T*)calloc(num, sizeof(T));
        return ((void *)temp);
    }

    /**
     */
    void deleteInstance(void* address) const {
        delete (T*)address;
    }

    /**
     */
    void clearValue(void * address) const {

        if ( isFloatingPoint() ) {
            *(T*)address =  0.0;
        } else {
            *(T*)address =  0;
        }
    }

    /**
     Assign a value to the variable at the given address.
     @param address Address of the variable.
     @param value Value to be assigned to the variable.
     */
    void assignValue(void * address, Value * value) const {

        NumericValue * numeric_value_p = dynamic_cast<NumericValue*>(value);
        if (numeric_value_p) {
            if ( isFloatingPoint() ) {
                *(T*)address =  numeric_value_p->getFloatingPointValue();
            } else {
                *(T*)address =  numeric_value_p->getIntegerValue();
            }
        } else {
            std::cerr << "ERROR: Attempt to assign non-numeric value to a numeric type.";
        }
    }

    /**
     Print the value of the variable at the given address, to the given stream.
     @param s The stream to print to.
     @param address Address of the variable.
     */
    void printValue(std::ostream &s, void *address ) const {
        s << *(T*)address ;
    }

    /**
     Get a string representation of this PrimitiveDataType's type-specifier.
     */
    std::string getTypeSpecName() const {
        std::string s("<invalid-primitive-type>");
        return s;
    }

    /**
     Return true if DataType is void. Otherwise return false.
     */
    bool isVoid() const { return false; }

    /* ==================================================================== */
    /*                       CLASS SPECIFIC INTERFACE                       */
    /* ==================================================================== */

    /**
     Return true if T is float or double. Otherwise return false.
     */
    bool isFloatingPoint() const { return false; }

    /**
     Return true if T is char, short, int, long, or long long. Otherwise return false.
     */
    bool isSigned() const { return false; }

};

/*
 Template specializations for PrimitiveDataType<T>::isFloatingPoint()
 */
template <> bool PrimitiveDataType<float>::isFloatingPoint() const;
template <> bool PrimitiveDataType<double>::isFloatingPoint() const;

/*
 Template specializations for PrimitiveDataType<T>::isSigned()
*/
template <> bool PrimitiveDataType<char>::isSigned() const;
template <> bool PrimitiveDataType<short>::isSigned() const;
template <> bool PrimitiveDataType<int>::isSigned() const;
template <> bool PrimitiveDataType<long>::isSigned() const;
template <> bool PrimitiveDataType<long long>::isSigned() const;

/*
 Template specializations for PrimitiveDataType<T>::isVoid()
*/
template <> bool PrimitiveDataType<void>::isVoid() const;

/*
 Template specializations for PrimitiveDataType<char>::printValue()
*/
template <> void PrimitiveDataType<char>::printValue(std::ostream &s, void *address ) const;
template <> void PrimitiveDataType<unsigned char>::printValue(std::ostream &s, void *address ) const;

/*
 Template specializations for PrimitiveDataType<void>
*/
template <> size_t PrimitiveDataType<void>::getSize() const;
template <> void* PrimitiveDataType<void>::createInstance(unsigned int num) const;
template <> void PrimitiveDataType<void>::deleteInstance(void* address) const;
template <> void PrimitiveDataType<void>::clearValue(void * address) const;
template <> void PrimitiveDataType<void>::assignValue(void * address, Value * value) const;
template <> void PrimitiveDataType<void>::printValue(std::ostream &s, void *address ) const;
#endif

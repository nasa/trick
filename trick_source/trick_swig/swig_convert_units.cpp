
#include <Python.h>
#include <iostream>

#include "trick/Unit.hh"

int convert_united_value( std::string & to_units , std::string & from_units , long long * val ) {

    if ( from_units.compare("--") ) {
        try {
            Unit converter(from_units.c_str()) ;
            *val = (long long)converter.Convert_to( (double)(* val) , to_units.c_str()) ;
        }
        catch (Unit::CONVERSION_ERROR & ce_err ) {
            PyErr_SetString(PyExc_AttributeError,"Units conversion Error");
            return -1 ;
        }
    }
    return 0 ;
}

int convert_united_value( std::string & to_units , std::string & from_units , double * val ) {

    if ( from_units.compare("--") ) {
        try {
            Unit converter(from_units.c_str()) ;
            *val = converter.Convert_to( *val , to_units.c_str()) ;
        }
        catch (Unit::CONVERSION_ERROR & ce_err ) {
            PyErr_SetString(PyExc_AttributeError,"Units conversion Error");
            return -1 ;
        }
    }
    return 0 ;
}


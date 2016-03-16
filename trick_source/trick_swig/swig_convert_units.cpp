
#include <Python.h>
#include <iostream>

#include "trick_utils/units/include/Unit.hh"

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

int scale_united_value( std::string & to_units , std::string & from_units , long long * val ) {

    if ( from_units.compare("--") ) {
        try {
            Units_t * from = new_units(from_units.c_str()) ;
            Units_t * to = new_units(to_units.c_str()) ;
            UnitsConvFn_t conv_fn;
            if (conv_fn_u(from, to, &conv_fn) != CONV_OK) {
                throw Unit::CONVERSION_ERROR();
            }
            *val = (long long)(conv_fn.C[1] * (*val)) ;
            free(from) ;
            free(to) ;
        }
        catch (Unit::CONVERSION_ERROR & ce_err ) {
            PyErr_SetString(PyExc_AttributeError,"Units conversion Error");
            return -1 ;
        }
    }
    return 0 ;
}

int scale_united_value( std::string & to_units , std::string & from_units , double * val ) {

    if ( from_units.compare("--") ) {
        try {
            Units_t * from = new_units(from_units.c_str()) ;
            Units_t * to = new_units(to_units.c_str()) ;
            UnitsConvFn_t conv_fn;
            if (conv_fn_u(from, to, &conv_fn) != CONV_OK) {
                throw Unit::CONVERSION_ERROR();
            }
            *val = conv_fn.C[1] * (*val) ;
            free(from) ;
            free(to) ;
        }
        catch (Unit::CONVERSION_ERROR & ce_err ) {
            PyErr_SetString(PyExc_AttributeError,"Units conversion Error");
            return -1 ;
        }
    }
    return 0 ;
}


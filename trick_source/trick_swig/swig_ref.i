
%module swig_ref

%{

#include "trick/swig/swig_ref.hh"
#include "trick_swig/swig_ref.cpp"

%}

%typemap(ret) int  __setitem__ {
    switch ( $1 ) {
        case TRICK_PARAMETER_ARRAY_SIZE:
            SWIG_exception_fail(SWIG_IndexError, "Error in method swig_ref::__setitem__") ;
        break ;
        case TRICK_UNITS_CONVERSION_ERROR:
            SWIG_exception_fail(SWIG_AttributeError, "Units Conversion error in method swig_ref::__setitem__");
        break ;
        case TRICK_SWIG_PARAMETER_INCORRECT_TYPE:
            SWIG_exception_fail(SWIG_TypeError, "Could not convert rhs to correct type in swig_ref::__setitem__");
        break ;
        case TRICK_SWIG_SLICE_ASSIGNMENT:
            SWIG_exception_fail(SWIG_TypeError, "Sequence does not fit with extended slice in swig_ref::__setitem__");
        break ;
        case TRICK_SWIG_UNKNOWN_INDEX_TYPE:
            SWIG_exception_fail(SWIG_TypeError, "Unknown object type as array index in swig_ref::__setitem__");
        break ;
        default:
        break ;
    }
}

%typemap(ret) PyObject * __getitem__ {
    if ( $1 == NULL ) {
        SWIG_exception_fail(SWIG_IndexError, "Index out of range in swig_ref::__getitem__") ;
     }
}

// expose the __str__ function to swig so that it can do pretty prints of parameters with units
class swig_ref {
    public:
        // only expose the getitem/setitem with PyObject argument.  We will deal with the argument ourselves.
        PyObject * __getitem__(PyObject * index) ;
        virtual int __setitem__( PyObject * index , PyObject * obj1 ) ;

        char * __str__() ;
        char * __repr__() ;

        PyObject * __len__() ;
} ;


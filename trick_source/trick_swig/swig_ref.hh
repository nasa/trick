
#ifndef SWIG_REF_HH
#define SWIG_REF_HH

#include <string>
#include "trick/reference.h"

#define TRICK_SWIG_PARAMETER_INCORRECT_TYPE 9
#define TRICK_SWIG_SLICE_ASSIGNMENT 10
#define TRICK_SWIG_UNKNOWN_INDEX_TYPE 11

class swig_ref {
    public:

        REF2 ref ;
        char * str_output ;

        swig_ref() ;
        ~swig_ref() ;

        PyObject * __getitem__(int ii) ;
        PyObject * __getitem__(PyObject * index) ;
        int __setitem__( int ii, PyObject * obj1 ) ;
        int __setitem__( PyObject * index , PyObject * obj1 ) ;

        char * __str__() ;
        char * __repr__() ;

        PyObject * __len__() ;

    private:
        bool need_to_deref ;
        void deref_address() ;
} ;

#endif

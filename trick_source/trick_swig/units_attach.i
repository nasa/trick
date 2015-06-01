
/* This provides %trick_units_functions that generically is used to wrap all units definintions in units.hh */

#ifndef UNITS_ATTACH_I
#define UNITS_ATTACH_I

%inline %{
#include "trick/Unit.hh"
#include "trick_swig/swig_double.hh"
%}

%inline %{

PyObject * attach_units(PyObject * in_units_obj , PyObject * in_object) {
    PyObject *result_obj = 0;
    unsigned int ii ;
    std::string in_units ;
    void * my_argp ;

    if ( PyString_Check(in_units_obj) ) {
        in_units = PyString_AsString(in_units_obj) ;
    }

    if ( PyList_Check(in_object)) {
        in_object = PyList_AsTuple(in_object) ;
    }

    if ( PyFloat_Check(in_object) || PyInt_Check(in_object)) {
        swig_double * container = new swig_double ;
        container->value = PyFloat_AsDouble(in_object) ;
        container->units = in_units ;
        result_obj = SWIG_NewPointerObj(SWIG_as_voidptr(container), SWIG_TypeQuery("swig_double *") , SWIG_POINTER_OWN);
    } else if ( SWIG_IsOK(SWIG_ConvertPtr(in_object, &my_argp,SWIG_TypeQuery("swig_double *"), 0 ))) {
        swig_double * temp_m = reinterpret_cast< swig_double * >(my_argp) ;
        if ( temp_m->units.compare("--") ) {
            try {
                Unit converter(temp_m->units.c_str()) ;
                temp_m->value = converter.Convert_to( temp_m->value , in_units.c_str()) ;
                temp_m->units = in_units ;
            }
            catch (Unit::CONVERSION_ERROR & ce_err ) {
                PyErr_SetString(PyExc_TypeError,"improper units conversion");
                return NULL;
            }
        }
        result_obj = SWIG_NewPointerObj(SWIG_as_voidptr(new swig_double(*temp_m)), SWIG_TypeQuery("swig_double *") , SWIG_POINTER_OWN);

    } else if ( PyTuple_Check(in_object)) {
        unsigned int size = PyTuple_Size(in_object) ;
        result_obj = PyTuple_New(size) ;

        for ( ii = 0 ; ii < size ; ii++ ) {

            PyObject *o = PyTuple_GetItem(in_object,ii) ;
            if ( PyList_Check(o)) {
                o = PyList_AsTuple(o) ;
            }

            if (PyTuple_Check(o)) {
                PyObject * new_obj ;
                new_obj = attach_units(in_units_obj , o) ;
                if ( new_obj != NULL ) {
                    PyTuple_SetItem( result_obj,  ii , new_obj ) ;
                } else {
                    return NULL;
                }
            } else if (PyFloat_Check(o) || PyInt_Check(o)) {
                swig_double * container = new swig_double ;
                PyObject * new_obj ;
                container->value = PyFloat_AsDouble(o) ;
                container->units = in_units ;
                new_obj = SWIG_NewPointerObj(SWIG_as_voidptr(container), SWIG_TypeQuery("swig_double *"), SWIG_POINTER_OWN);
                PyTuple_SetItem( result_obj,  ii , new_obj ) ;
            } else {
                PyErr_SetString(PyExc_TypeError,"List must contain double");
                return NULL;
            }
        }
    }
    return(result_obj) ;
}
%}

#endif

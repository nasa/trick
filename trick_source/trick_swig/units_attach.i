
/* This provides %trick_units_functions that generically is used to wrap all units definintions in units.hh */

#ifndef UNITS_ATTACH_I
#define UNITS_ATTACH_I

%inline %{
#include <frameobject.h>
#include <udunits2.h>
#include "trick/swig/swig_double.hh"
#include "trick/map_trick_units_to_udunits.hh"
#include "trick/IPPython.hh"

%}

%inline %{

PyObject * attach_units(PyObject * in_units_obj , PyObject * in_object) {
    PyObject *result_obj = 0;
    unsigned int ii ;
    std::string in_units ;
    void * my_argp ;

#if PY_VERSION_HEX >= 0x03000000
    if ( PyUnicode_Check(in_units_obj) ) {
        PyObject * temp = PyUnicode_AsEncodedString(in_units_obj, "utf-8", "Error ~");
        in_units = PyBytes_AS_STRING(temp) ;
#else
    if ( PyString_Check(in_units_obj) ) {
        in_units = PyString_AsString(in_units_obj) ;
#endif
        in_units.erase(remove_if(in_units.begin(), in_units.end(), isspace), in_units.end());
        if ( ! in_units.compare("--")) {
            in_units = "1" ;
        }
        std::string new_units = map_trick_units_to_udunits(in_units) ;
        if ( in_units.compare(new_units) ) {
            PyThreadState *tstate = PyThreadState_GET();
            std::string file_name ;
            int line_no = 0 ;
            if (NULL != tstate && NULL != tstate->frame) {
#if PY_VERSION_HEX >= 0x03000000
                PyObject * temp = PyUnicode_AsEncodedString(tstate->frame->f_code->co_filename, "utf-8", "Error ~");
                file_name = PyBytes_AS_STRING(temp) ;
#else
                file_name = PyString_AsString(tstate->frame->f_code->co_filename);
#endif
#if (PY_MAJOR_VERSION == 2 ) && (PY_MINOR_VERSION <= 6)
                line_no = PyCode_Addr2Line(tstate->frame->f_code, tstate->frame->f_lasti) ;
#else
                line_no = PyFrame_GetLineNumber(tstate->frame) ;
#endif
            }
            if ( check_units_conversion_messenger_for_signs_of_life() ) {
                std::cout << "\033[33mUnits converted from [" << in_units << "] to [" << new_units << "] "
                 << file_name << ":" << line_no << "\033[0m" << std::endl ;
            }
            in_units = new_units ;
        }
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
        if ( temp_m->units.compare("1") ) {
            ut_unit * from = ut_parse(Trick::UdUnits::get_u_system(), temp_m->units.c_str(), UT_ASCII) ;
            if ( !from ) {
                PyErr_SetString(PyExc_AttributeError,(std::string("could not covert from units "+temp_m->units).c_str()));
                return NULL ;
            }
            ut_unit * to = ut_parse(Trick::UdUnits::get_u_system(), in_units.c_str(), UT_ASCII) ;
            if ( !to ) {
                PyErr_SetString(PyExc_AttributeError,(std::string("could not covert to units "+in_units).c_str()));
                return NULL ;
            }

            cv_converter * converter = ut_get_converter(from,to) ;
            if ( converter ) {
                temp_m->value = cv_convert_double(converter, temp_m->value ) ;
                temp_m->units = in_units ;
                cv_free(converter) ;
            } else {
                PyErr_SetString(PyExc_AttributeError,"Units conversion Error");
                return NULL ;
            }
            ut_free(from) ;
            ut_free(to) ;
        } else {
            temp_m->units = in_units ;
        }
        result_obj = SWIG_NewPointerObj(SWIG_as_voidptr(new swig_double(*temp_m)), SWIG_TypeQuery("swig_double *") , SWIG_POINTER_OWN);
    } else if ( SWIG_IsOK(SWIG_ConvertPtr(in_object, &my_argp,SWIG_TypeQuery("swig_int *"), 0 ))) {
        swig_int * temp_m = reinterpret_cast< swig_int * >(my_argp) ;
        if ( temp_m->units.compare("1") ) {
            ut_unit * from = ut_parse(Trick::UdUnits::get_u_system(), temp_m->units.c_str(), UT_ASCII) ;
            if ( !from ) {
                PyErr_SetString(PyExc_AttributeError,(std::string("could not covert from units "+temp_m->units).c_str()));
                return NULL ;
            }
            ut_unit * to = ut_parse(Trick::UdUnits::get_u_system(), in_units.c_str(), UT_ASCII) ;
            if ( !to ) {
                PyErr_SetString(PyExc_AttributeError,(std::string("could not covert to units "+in_units).c_str()));
                return NULL ;
            }

            cv_converter * converter = ut_get_converter(from,to) ;
            if ( converter ) {
                temp_m->value = (long long)cv_convert_double(converter, (double)temp_m->value ) ;
                temp_m->units = in_units ;
                cv_free(converter) ;
            } else {
                PyErr_SetString(PyExc_AttributeError,"Units conversion Error");
                return NULL ;
            }
            ut_free(from) ;
            ut_free(to) ;
        } else {
            temp_m->units = in_units ;
        }
        result_obj = SWIG_NewPointerObj(SWIG_as_voidptr(new swig_int(*temp_m)), SWIG_TypeQuery("swig_int *") , SWIG_POINTER_OWN);
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

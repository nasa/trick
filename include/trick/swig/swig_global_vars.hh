
#include <Python.h>

extern "C" {
    PyObject * new_cvar_list() ;
    void combine_cvars( PyObject *arg1 , PyObject *arg2 ) ;
}


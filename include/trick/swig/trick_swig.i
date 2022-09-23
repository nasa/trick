
/* include support for STLs */
%include "std_list.i"
%include "std_map.i"
%include "std_string.i"
%include "std_vector.i"
%include "factory.i"

%include "trick/swig/swig_extend_str.i"
%include "trick/swig/swig_class_typedef.i"
%include "trick/swig/cast_as.i"
%include "trick/swig/swig_int_typemap.i"

/*
   compactdefaultargs fixes a bug with enumeration default arguments not being
   recogized in function calls starting in swig 3.0.x
 */
%feature("compactdefaultargs") ;

/* SWIG can't understand GNU C keyword "__attribute__" */
#ifdef SWIG
#define __attribute__(x)
#endif

%{
#include <sstream>

#include "trick/UnitsMap.hh"
#include "trick/MemoryManager.hh"
#include "trick/reference.h"
#include "trick/memorymanager_c_intf.h"
#include "trick/PythonPrint.hh"

#ifndef SWIG_as_voidptr 
#define SWIG_as_voidptr(a) const_cast< void * >(static_cast< const void * >(a)) 
#endif

#ifndef SWIG_as_voidptrptr 
#define SWIG_as_voidptrptr(a) ((void)SWIG_as_voidptr(*a),reinterpret_cast< void** >(a)) 
#endif

#ifndef SWIG_IsOK
#define SWIG_IsOK(r)               (r >= 0)
#endif

#ifndef SWIG_Error 
#define SWIG_Error(code, msg) std::cout<<"SWIG_Error(errorcode, "<<msg<<")"<<std::endl
#endif

#ifndef SWIG_exception_fail 
#define SWIG_exception_fail(code, msg) std::cout<<"SWIG_exception_fail(errorcode, "<<msg<<")"<<std::endl 
#endif

#ifndef Py_RETURN_TRUE 
#define Py_RETURN_TRUE return Py_INCREF(Py_True), Py_True
#endif

#ifndef Py_RETURN_FALSE
#define Py_RETURN_FALSE return Py_INCREF(Py_False), Py_False
#endif

#ifndef Py_RETURN_NONE 
#define Py_RETURN_NONE return Py_INCREF(Py_None), Py_None
#endif

%}

// This pythoncode section overrides the default swig defined functions in each py file.
// It sets the "static" argument to 1 which disallows new attributes
// to be created in our classes.
// It also provides more information to the user about how to correct the problem

#if SWIG_VERSION > 0x010321
%pythoncode %{
def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    all_keys = [attr for attr in dir(class_type) if not attr.startswith('__')and attr != '_s' ]
    data_keys = sorted(class_type.__swig_setmethods__.keys())
    method_keys = [ x for x in all_keys if x not in data_keys ]
    raise AttributeError("Type %s does not contain member %s.\n%s data = %s\n%s methods = %s" %
     (self.__class__.__name__,name,self.__class__.__name__,data_keys,self.__class__.__name__,method_keys))

def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        # this line is changed to handle older swigs that used PySwigObject instead of the current SwigPyObject
        if type(value).__name__ == 'SwigPyObject' or type(value).__name__ == 'PySwigObject' :
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        all_keys = [attr for attr in dir(class_type) if not attr.startswith('__') and attr != '_s' ]
        data_keys = sorted(class_type.__swig_setmethods__.keys())
        method_keys = [ x for x in all_keys if x not in data_keys ]
        raise AttributeError("Type %s does not contain member %s.\n%s data = %s\n%s methods = %s" %
         (self.__class__.__name__,name,self.__class__.__name__,data_keys,self.__class__.__name__,method_keys))

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,1)
%}
#endif


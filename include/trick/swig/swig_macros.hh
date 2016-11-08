
//These appear in some files but not in others.  define them if we have to.


#ifdef SWIG
#define __attribute__(x)
#endif

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

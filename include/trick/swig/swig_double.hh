
#ifndef SWIG_DOUBLE_HH
#define SWIG_DOUBLE_HH

#include <string>

class swig_double {
    public:
        double value ;
        std::string units ;
        static char str_output[32] ;

        swig_double() ;

        char * __str__() ;
        char * __repr__() ;

        PyObject * __add__(PyObject * obj1) ;
        PyObject * __sub__(PyObject * obj1) ;
        PyObject * __mul__(PyObject * obj1) ;
        PyObject * __div__(PyObject * obj1) ;
        PyObject * __truediv__(PyObject * obj1) ;
        PyObject * __floordiv__(PyObject * obj1) ;
        PyObject * __mod__(PyObject * obj1) ;
        //PyObject * __divmod__(PyObject * obj1) ;
        PyObject * __pow__(PyObject * obj1) ;

        PyObject * __radd__(PyObject * obj1) ;
        PyObject * __rsub__(PyObject * obj1) ;
        PyObject * __rmul__(PyObject * obj1) ;
        PyObject * __rdiv__(PyObject * obj1) ;
        PyObject * __rtruediv__(PyObject * obj1) ;
        PyObject * __rfloordiv__(PyObject * obj1) ;
        PyObject * __rmod__(PyObject * obj1) ;
        //PyObject * __rdivmod__(PyObject * obj1) ;
        PyObject * __rpow__(PyObject * obj1) ;

        PyObject * __iadd__(PyObject * obj1) ;
        PyObject * __isub__(PyObject * obj1) ;
        PyObject * __imul__(PyObject * obj1) ;
        PyObject * __idiv__(PyObject * obj1) ;
        PyObject * __itruediv__(PyObject * obj1) ;
        PyObject * __ifloordiv__(PyObject * obj1) ;
        PyObject * __imod__(PyObject * obj1) ;
        PyObject * __ipow__(PyObject * obj1) ;

        PyObject * __lt__(PyObject * obj1) ;
        PyObject * __le__(PyObject * obj1) ;
        PyObject * __eq__(PyObject * obj1) ;
        PyObject * __ne__(PyObject * obj1) ;
        PyObject * __gt__(PyObject * obj1) ;
        PyObject * __ge__(PyObject * obj1) ;

        PyObject * __neg__() ;
        PyObject * __pos__() ;
        PyObject * __abs__() ;

        //PyObject * __complex__(PyObject * obj1) ;
        PyObject * __int__() ;
        PyObject * __long__() ;
        PyObject * __float__() ;

        PyObject * __len__() ;
} ;

#endif


%module swig_int

%include "std_string.i"

%{

#include "trick/swig/swig_int.hh"
#include "trick_swig/swig_int.cpp"

%}

// expose the __str__ function to swig so that it can do pretty prints of parameters with units
class swig_int {
    public:
        std::string units;

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
        PyObject * __lshift__(PyObject * obj1) ;
        PyObject * __rshift__(PyObject * obj1) ;
        PyObject * __and__(PyObject * obj1) ;
        PyObject * __xor__(PyObject * obj1) ;
        PyObject * __or__(PyObject * obj1) ;

        PyObject * __radd__(PyObject * obj1) ;
        PyObject * __rsub__(PyObject * obj1) ;
        PyObject * __rmul__(PyObject * obj1) ;
        PyObject * __rdiv__(PyObject * obj1) ;
        PyObject * __rtruediv__(PyObject * obj1) ;
        PyObject * __rfloordiv__(PyObject * obj1) ;
        PyObject * __rmod__(PyObject * obj1) ;
        //PyObject * __rdivmod__(PyObject * obj1) ;
        PyObject * __rpow__(PyObject * obj1) ;
        PyObject * __rlshift__(PyObject * obj1) ;
        PyObject * __rrshift__(PyObject * obj1) ;
        PyObject * __rand__(PyObject * obj1) ;
        PyObject * __rxor__(PyObject * obj1) ;
        PyObject * __ror__(PyObject * obj1) ;

        PyObject * __iadd__(PyObject * obj1) ;
        PyObject * __isub__(PyObject * obj1) ;
        PyObject * __imul__(PyObject * obj1) ;
        PyObject * __idiv__(PyObject * obj1) ;
        PyObject * __itruediv__(PyObject * obj1) ;
        PyObject * __ifloordiv__(PyObject * obj1) ;
        PyObject * __imod__(PyObject * obj1) ;
        PyObject * __ipow__(PyObject * obj1) ;
        PyObject * __ilshift__(PyObject * obj1) ;
        PyObject * __irshift__(PyObject * obj1) ;
        PyObject * __iand__(PyObject * obj1) ;
        PyObject * __ixor__(PyObject * obj1) ;
        PyObject * __ior__(PyObject * obj1) ;

        PyObject * __lt__(PyObject * obj1) ;
        PyObject * __le__(PyObject * obj1) ;
        PyObject * __eq__(PyObject * obj1) ;
        PyObject * __ne__(PyObject * obj1) ;
        PyObject * __gt__(PyObject * obj1) ;
        PyObject * __ge__(PyObject * obj1) ;

        PyObject * __neg__() ;
        PyObject * __pos__() ;
        PyObject * __abs__() ;
        PyObject * __invert__() ;
        PyObject * __nonzero__() ;

        //PyObject * __complex__(PyObject * obj1) ;
        PyObject * __int__() ;
        PyObject * __long__() ;
        PyObject * __float__() ;

        PyObject * __oct__() ;
        PyObject * __hex__() ;

        PyObject * __len__() ;

        int __index__() ;
        PyObject * __bool__() ;
} ;

%pythoncode %{
#if SWIG_VERSION > 0x040000
def _trick_setattr_nondynamic_instance_variable(set):
    def set_instance_attr(self, name, value):
        if name == "thisown":
            self.this.own(value)
        elif name == "this":
            set(self, name, value)
        else:
            # f-strings are available in Python 3.6 and later
            #msg = f'You cannot add instance attribute \'{name}\' to Trick swig_int'
            # For compatibility, use format method
            msg = "You cannot add instance attribute '{}' to Trick swig_double".format(name)
            raise AttributeError(msg)
    return set_instance_attr

swig_int.__setattr__ = _trick_setattr_nondynamic_instance_variable(object.__setattr__)
#endif
%}

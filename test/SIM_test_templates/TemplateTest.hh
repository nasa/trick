/**
@file

@verbatim
PURPOSE:
    (Template tests)
@endverbatim
*******************************************************************************/

#ifndef TEMPLATETEST_HH
#define TEMPLATETEST_HH
#include "Foo.hh"

template <class A, class B>
class TTT1 {
    public:
        A aa ;
        B bb ;
        TTT1<A,B> * ttt ;

        typedef TTT1<A,B> C ;
        C * cc ;
        typedef TTT1<B,A> D ;
        D * dd ;
} ;

template <class T>
class TTT2 {
    public:
        TTT1<T, int> a;
        Foo<T[3]> b;
};

class TemplateTest {

  friend class InputProcessor ;
  friend void init_attrTemplateTest() ;

  public:
   TTT1< int , double > TTT_var_scalar_builtins ;
   TTT1< int[2] , double[3] > TTT_var_array_builtins ;
   TTT1< Bar, Bar[2] > TTT_var_enum ;

   TTT1< Foo<int>, Foo<double[2]>[3] > TTT_var_template_parameters ;
   TTT2< Foo< TTT1< int, TTT2<char> > > > TTT_templates_of_templates ;

};

#ifdef SWIG
%struct_str(TemplateTest)
#endif

#endif /* _BALL_HH_ */


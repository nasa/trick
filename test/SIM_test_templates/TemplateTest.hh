/**
@file

@verbatim
PURPOSE:
    (Template tests)
@endverbatim
*******************************************************************************/

#ifndef TEMPLATETEST_HH
#define TEMPLATETEST_HH

template <class A, class B>
class TTT {
    public:

        TTT() {
            aa = 0 ;
            bb = 0 ;
            cc = NULL ;
            dd = NULL ;
        } ;
        A aa ;
        B bb ;
        TTT<A,B> * ttt ;

        typedef TTT<A,B> C ;
        C * cc ;
        typedef TTT<B,A> D ;
        D * dd ;
} ;

class TemplateTest {

  friend class InputProcessor ;
  friend void init_attrTemplateTest() ;

  public:
   TTT< int , double > TTT_var ;

};

#ifdef SWIG
%struct_str(TemplateTest)
#endif

#endif /* _BALL_HH_ */


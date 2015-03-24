/**
@file

@verbatim
PURPOSE:
    (Ball model EOM state parameter definition.)
REFERENCES:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 dimensional space)
     (Translational EOM only))
LIBRARY DEPENDENCY:
    ()
PROGRAMMERS:
    (((Robert W. Bailey) (Sweet Systems Inc) (March 1997) (Tutorial Lesson 1))
     ((Edwin Z. Crues)(Titan Systems Corp.)(Jan 2002)(Crude C++ translation)))
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


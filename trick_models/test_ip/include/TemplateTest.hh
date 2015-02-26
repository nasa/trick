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

/*
 * $Log: Ball.hh,v $
 * Revision 5.1  2004-08-05 13:06:21-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:42  lin
 * Bump version number for 04
 *
 * Revision 1.2  2002/10/07 15:15:53  lin
 * Add rcs version info to all trick_models files
 *
 */

#ifndef _TEMPLATETEST_HH_
#define _TEMPLATETEST_HH_

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


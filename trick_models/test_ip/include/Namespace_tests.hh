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
    ((Ball_test.o))
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

#ifndef _NAMESPACE_HH_
#define _NAMESPACE_HH_

// System include files.
#include <string>
#include <iostream>
#include <sstream>

namespace my_ns {

class BB {
    public:
        std::string str ;
} ;

    namespace inner_ns {
        class InnerTest {
            int ii ;
        } ;
    } ;

} ;

#if SWIG_VERSION >= 0x010340
// These namespaces test including a template from one namespace to another
// The goal is to use a clang based convert swig that can handle this
// situation across namespaces without requiring a leading "::".  convert_swig
// will understand to add a leading "::" automatically in the .i file.
// NOTE: This code crashes swig on RHEL 5 machines with swig version < 1.3.40.
// I have added a guard against it.
namespace NS1 {
class P {
   public:
    int jj ;
} ;

template < class T >
class A : public P {
   public :
    T ii ;
} ;
}

namespace NS2 {
class B {
   public :
   //TODO: in the clang based convert_swig, we can use NS1 without the leading ::
   //NS1::A< int > m ;
   ::NS1::A< int > m ;
} ;
}
#endif

#endif /* _BALL_HH_ */


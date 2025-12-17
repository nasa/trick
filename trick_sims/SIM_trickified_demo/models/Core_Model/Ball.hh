/**
@file

@verbatim
PURPOSE:
    (Ball model EOM state parameter definition.)
LANGUAGE:
    (C++)
REFERENCES:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 dimensional space)
     (Translational EOM only))
LIBRARY DEPENDENCY:
    ((Ball.cpp)
     (BallStateDeriv.cpp)
     (BallStateInit.cpp)
     (BallStateInteg.cpp)
     (BallStatePrint.cpp)
     (BallForceField.cpp)
     (BallShutdown.cpp))
PROGRAMMERS:
    (((Robert W. Bailey) (Sweet Systems Inc) (March 1997) (Tutorial Lesson 1))
     ((Edwin Z. Crues)(Titan Systems Corp.)(Jan 2002)(Crude C++ translation)))
@endverbatim
*******************************************************************************/

#ifndef BALL_HH
#define BALL_HH

// System include files.

#include <iostream>

// Model include files.
#include "BallState.hh"
#include "BallForce.hh"

/** @class Ball
    @brief ball in C++
 */

#include "trick/mm_macros.hh"

class Ball {

  TRICK_MM_INTERFACE(Ball, Ball)

  public:
   // Default constructor and destructor.
   Ball();
  ~Ball();

   // Initialization functions.
   int state_init();

   // Derivative class jobs.
   int force_field();
   int state_deriv();

   // Integration class jobs.
   int state_integ();

   int state_print() ;

   int shutdown() ;

   // Trick requires all logged data to be public.
   BallState state; /**< -- Ball state object. */
   BallForce force; /**< -- Ball force object. */


};

#ifdef SWIG
// This SWIG statement give us the capability to print the entire Ball class with a single print
%struct_str(Ball)
#endif

#endif /* _BALL_HH_ */


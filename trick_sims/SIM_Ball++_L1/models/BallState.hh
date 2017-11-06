/**
  @file

PURPOSE:
    (Ball model state parameter definition.)
LANGUAGE:
    (C++)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 dimensional space)
     (Constant force)
     (Always toward a stationary point))
LIBRARY DEPENDENCY:
     ((BallState.o) (BallState_default_data.o))
PROGRAMMERS:
    (((Robert W. Bailey) (Sweet Systems Inc) (March 1997) (Tutorial Lesson 1))
     ((Edwin Z. Crues)(Titan Systems Corp.)(Jan 2002)(Crude C++ translation)))
*******************************************************************************/

#ifndef BALLSTATE_HH
#define BALLSTATE_HH

/** @class BallStateInput
    @brief ball state input parameters
 */
class BallStateInput {
 public:

  double mass;                        /**< trick_units(kg)  Total mass.                          */
  double position[2];                 /**< trick_units(m)   X(horizontal), Y(vertical) position. */
  double speed;                       /**< trick_units(m/s) Linear speed.                        */
  double elevation;                   /**< trick_units(rad) Trajectory angle with respect to the horizontal. */

  BallStateInput() ;
};

/** @class BallStateOutput
    @brief ball state output parameters
 */
class BallStateOutput {
 public:
  double position[2];                 /**< trick_units(m)    X(horizontal), Y(vertical) position. */
  double velocity[2];                 /**< trick_units(m/s)  X, Y velocity.                       */
  double acceleration[2];             /**< trick_units(m/s2) X, Y acceleration.                   */
  double external_force[2];           /**< trick_units(N)    Total external force on ball.        */

  BallStateOutput() ;
};

/** @class BallStateWork
    @brief ball state output parameters
 */
class BallStateWork {
 public:
  unsigned int num_external_force ;
  double ** external_force; /**< trick_units(N) External forces, from 'collect' */

  BallStateWork() ;
};

/** @class BallState
    @brief ball state
 */
class BallState {

  public:
   /* Member data. */
   BallStateInput  input;  /**< trick_units(--) User inputs    */
   BallStateOutput output; /**< trick_units(--) User outputs.  */
   BallStateWork   work;   /**< trick_units(--) EOM workspace. */

   int default_data() ;
   int add_external_force( double * in_ptr ) ;
};

#ifdef SWIG
// This SWIG statement give us the capability to print the entire BallState class with a single print
%struct_str(BallState)
%struct_str(BallStateInput)
%struct_str(BallStateOutput)
%struct_str(BallStateWork)
#endif


#endif /* _BALL_STATE_HH_ */


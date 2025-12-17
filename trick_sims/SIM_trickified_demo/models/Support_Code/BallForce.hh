/**
  @file

PURPOSE:
    (Ball model external force parameter definition.)
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
     ((BallForce.o) (BallForce_default_data.o))
PROGRAMMERS:
    (((Robert W. Bailey) (Sweet Systems Inc) (March 1997) (Tutorial Lesson 1))
     ((Edwin Z. Crues)(Titan Systems Corp.)(Jan 2002)(Crude C++ translation)))
*/

#ifndef BALLFORCE_HH
#define BALLFORCE_HH

/** @class BallForceInput
    @brief ball force input params
 */
class BallForceInput {
 public:
  double origin[2];           /**< trick_units(m)    Origin of force center */
  double force;               /**< trick_units(N)    Force magnitude        */

  BallForceInput() ;

};

/** @class BallForceOutput
    @brief ball force output params
 */
class BallForceOutput {
 public:
  double force[2];            /**< trick_units(N)    Resulting X,Y force on ball */

  BallForceOutput() ;
};

/** @class BallForce
    @brief ball force params
 */
class BallForce {

  public:
   /* Member data. */
   int default_data() ;
   BallForceInput  input;
   BallForceOutput output;

};

#ifdef SWIG
// This SWIG statement give us the capability to print the entire BallForce class with a single print
%struct_str(BallForce)
%struct_str(BallForceInput)
%struct_str(BallForceOutput)
#endif


#endif /* _BALL_FORCE_HH_ */


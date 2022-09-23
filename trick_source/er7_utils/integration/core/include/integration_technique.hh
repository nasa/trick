/**
 * @if Er7UtilsUseGroups
 * @addtogroup Er7Utils
 * @{
 * @addtogroup Integration
 * @{
 * @endif
 */

/**
 * @file
 * Defines the class Integration, which enumerates the integration techniques
 * provided by the ER7 utilities.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_INTEGRATION_TECHNIQUE_HH
#define ER7_UTILS_INTEGRATION_TECHNIQUE_HH


namespace er7_utils {

/**
 * Contains the enums Integration::Technique and Integration::ODEProblemType.
 * Note: This would be better as a namespace, but swig gets in the way.
 */
class Integration {

   // This is a static class.
   // The default constructor, copy constructor, destructor, and assignment
   // operator for this class are private / unimplemented.

public:

   /**
    * Enumerates the integration techniques supported directly by this package.
    */
   enum Technique {
      Unspecified            =   0, //!< Default initial value

      Euler                  =   1, //!< Basic Euler
      SymplecticEuler        =   2, //!< Symplectic Euler

      Beeman                 =  11, //!< Beeman's algorithm
      NystromLear2           =  14, //!< Second order Nystrom-Lear
      PositionVerlet         =  15, //!< Position verlet
      RK2Heun                =  16, //!< Heun's method
      RK2Midpoint            =  17, //!< Midpoint method
      VelocityVerlet         =  19, //!< Velocity verlet

      ModifiedMidpoint4      =  21, //!< Modified midpoint

      AdamsBashforthMoulton4 =  31, //!< Fourth order Adams-Bashforth-Moulton
      RungeKutta4            =  34, //!< Fourth order Runga Kutta
      RKGill4                =  35, //!< Fourth order Runga Kutta Gill

      RKNystrom4             =  36, //!< Fourth order Runga Kutta Nystrom

      RKFehlberg45           =  44, //!< Runge-Kutta Fehlberg 4/5

      RKFehlberg78           =  74, //!< Runge-Kutta Fehlberg 7/8

// Gauss Jackson isn't in this alpha release.
#if 0
      GaussJackson           = 121, //!< Gauss-Jackson
#endif

      Unsupported            = 998, //!< Catch-all for unsupported techniques
      Invalid                = 999  //!< Catch-all for foul ups
   };


   /**
    * Enumerates the types of problems to be solved.
    */
   enum ODEProblemType {

      FirstOrderODE,                   /**<
         Problem is a first order ODE where an initial state is to be propated
         in time using the state derivatives. The integrate function takes
         target stage, delta t, state, and state derivatives as inputs
         and updates the state in place. */

      SimpleSecondOrderODE,            /**<
         Problem is a second order ODE in which the time derivative of position
         is the velocity. An initial position and velocity are to be propated
         in time using acceleration. The integrate function takes
         target stage, delta t, position, velocity, and acceleration as inputs
         and updates position and velocity in place. */

      GeneralizedDerivSecondOrderODE,  /**<
         Problem is a generalized second order ODE in which the time derivative
         of generalized position is some function of position and velocity.
         As with the SimpleSecondOrderODE, an initial position and velocity are
         to be propated in time using acceleration. The integrate function takes
         target stage, delta t, position, velocity, and acceleration as inputs
         and updates position and velocity in place. */

      GeneralizedStepSecondOrderODE    /**<
         Problem is a generalized second order ODE in which generalized position
         is advances by some function of generalized position and velocity.
         As with the SimpleSecondOrderODE, an initial position and velocity are
         to be propated in time using acceleration. The integrate function takes
         target stage, delta t, position, velocity, and acceleration as inputs
         and updates position and velocity in place. */

   };


private:

   /**
    * Not implemented.
    */
   Integration (void);

   /**
    * Not implemented.
    */
   Integration (const Integration &);

   /**
    * Not implemented.
    */
   ~Integration (void);

   /**
    * Not implemented.
    */
   Integration & operator= (const Integration &);
};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

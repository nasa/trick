/*******************************************************************************

Purpose:
  (Define the class RKNystrom4SecondOrderODEIntegrator, which integrates a state
   comprising a zeroth derivative / first derivative pair via fourth order
   Runge Kutta Nystrom.)

Library dependencies:
  ((rkn4_second_order_ode_integrator.o))

Programmers:
  (((David Hammen) (OSR) (July 2008) (JEOD_2.0) (Initial version))
   ((David Hammen) (OSR) (March 2009) (JEOD_2.0 #22) (Cleanup))
   ((David Hammen) (OSR) (Sept 2011) (JEOD 2.2 #537)
    (Multi-cycle support)))

*******************************************************************************/


#ifndef ER7_UTILS_RKNYSTROM4_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_RKNYSTROM4_TWO_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Propagate state using the standard 4th order Runge-Kutta method.
 */
class RKNystrom4SecondOrderODEIntegrator : public SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RKNystrom4SecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * RKNystrom4SecondOrderODEIntegrator destructor.
    */
   virtual ~RKNystrom4SecondOrderODEIntegrator (void);


protected:

   // Constructors.

   /**
    * RKNystrom4SecondOrderODEIntegrator default constructor.
    * Notes:
    *  - This implicitly assumes the time derivative of position is velocity.
    *  - This is needed so that Trick can checkpoint/restart its integrators.
    */
   RKNystrom4SecondOrderODEIntegrator (void);

   /**
    * RKNystrom4SecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RKNystrom4SecondOrderODEIntegrator (
      const RKNystrom4SecondOrderODEIntegrator & src);

   /**
    * RKNystrom4SecondOrderODEIntegrator non-default constructor
    * for a simple second order ODE, one in which generalized velocity
    * is time derivative of generalized position.
    * @param[in]     size                State size
    * @param[in,out] controls            Integration controls
    */
   RKNystrom4SecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);

   /**
    * RKNystrom4SecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the position step function.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   RKNystrom4SecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls);


   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (RKNystrom4SecondOrderODEIntegrator & other);

   using SecondOrderODEIntegrator::swap;


   // Member data.

   double * init_pos; /**< trick_units(--) @n
      Position at the start of an integration cycle. */

   double * init_vel; /**< trick_units(--) @n
      Velocity at the start of an integration cycle. */

   double * mean_vel; /**< trick_units(--) @n
      Some mean velocity used to propagate position. */

   double * dtheta; /**< trick_units(--) @n
      Product of delta t and weighted sum of generalized velocities. */

   double * veldot_hist[4]; /**< trick_units(--) @n
      Velocity derivatives at each step in the integration cycle. */


private:

   /**
    * Not implemented.
    */
   RKNystrom4SecondOrderODEIntegrator & operator= (
      const RKNystrom4SecondOrderODEIntegrator &);
};


/**
 * Specialization of RKNystrom4SecondOrderODEIntegrator for the case of
 * generalized velocity being the time derivative of generalized position.
 */
class RKNystrom4SimpleSecondOrderODEIntegrator :
   public RKNystrom4SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RKNystrom4SimpleSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RKNystrom4SimpleSecondOrderODEIntegrator default constructor.
    */
   RKNystrom4SimpleSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      RKNystrom4SecondOrderODEIntegrator()
   {}

   /**
    * RKNystrom4SimpleSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RKNystrom4SimpleSecondOrderODEIntegrator (
      const RKNystrom4SimpleSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      RKNystrom4SecondOrderODEIntegrator(src)
   {}

   /**
    * RKNystrom4SimpleSecondOrderODEIntegrator non-default constructor.
    * @param[in]     size      Size of the position and velocity vectors
    * @param[in,out] controls  Integration controls
    */
   RKNystrom4SimpleSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      RKNystrom4SecondOrderODEIntegrator(size, controls)
   {}

   /**
    * RKNystrom4SimpleSecondOrderODEIntegrator destructor.
    */
   virtual ~RKNystrom4SimpleSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RKNystrom4SimpleSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RKNystrom4SimpleSecondOrderODEIntegrator & operator= (
      RKNystrom4SimpleSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RKNystrom4SimpleSecondOrderODEIntegrator * create_copy () const;


   /**
    * Propagate state via RKN4 for the special case of velocity being the
    * derivative of position.
    * @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
    * @param[in]     target_stage  The stage of the integration process
    *                              that the integrator should try to attain.
    * @param[in]     accel         Time derivative of the generalized velocity.
    * @param[in,out] velocity      Generalized velocity vector.
    * @param[in,out] position      Generalized position vector.
    *
    * @return The status (time advance, pass/fail status) of the integration.
    */
   virtual IntegratorResult integrate (
      double dyn_dt,
      unsigned int target_stage,
      double const * ER7_UTILS_RESTRICT accel,
      double * ER7_UTILS_RESTRICT velocity,
      double * ER7_UTILS_RESTRICT position);
};


/**
 * Specialization of RKNystrom4SecondOrderODEIntegrator for the case of
 * the generalized position being constrained to lie on a manifold.
 * The function compute_position_step makes a non-linear position step based
 * generalized position, generalized velocity, and the time step.
 */
class RKNystrom4GeneralizedStepSecondOrderODEIntegrator :
   public RKNystrom4SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RKNystrom4GeneralizedStepSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RKNystrom4GeneralizedStepSecondOrderODEIntegrator default constructor.
    */
   RKNystrom4GeneralizedStepSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      RKNystrom4SecondOrderODEIntegrator()
   {}

   /**
    * RKNystrom4GeneralizedStepSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RKNystrom4GeneralizedStepSecondOrderODEIntegrator (
      const RKNystrom4GeneralizedStepSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      RKNystrom4SecondOrderODEIntegrator(src)
   {}

   /**
    * RKNystrom4GeneralizedStepSecondOrderODEIntegrator non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   RKNystrom4GeneralizedStepSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      RKNystrom4SecondOrderODEIntegrator (position_size, velocity_size,
                                          step_funs, controls)
   {}

   /**
    * RKNystrom4GeneralizedStepSecondOrderODEIntegrator destructor.
    */
   virtual ~RKNystrom4GeneralizedStepSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RKNystrom4GeneralizedStepSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RKNystrom4GeneralizedStepSecondOrderODEIntegrator & operator= (
      RKNystrom4GeneralizedStepSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RKNystrom4GeneralizedStepSecondOrderODEIntegrator * create_copy ()
   const;

   /**
    * Propagate state via RKN4 for generalized position and generalized velocity
    * where generalized position is advanced using the position step function.
    * @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
    * @param[in]     target_stage  The stage of the integration process
    *                              that the integrator should try to attain.
    * @param[in]     accel         Time derivative of the generalized velocity.
    * @param[in,out] velocity      Generalized velocity vector.
    * @param[in,out] position      Generalized position vector.
    *
    * @return The status (time advance, pass/fail status) of the integration.
    */
   virtual IntegratorResult integrate (
      double dyn_dt,
      unsigned int target_stage,
      double const * ER7_UTILS_RESTRICT accel,
      double * ER7_UTILS_RESTRICT velocity,
      double * ER7_UTILS_RESTRICT position);
};

}


#endif

/**
 * @if Er7UtilsUseGroups
 * @addtogroup Er7Utils
 * @{
 * @addtogroup Trick
 * @{
 * @endif
 */

/**
 * @file
 * Defines the extensible class er7_utils::TrickIntegrator, which extends the
 * Trick::Integrator class to take advantage of the integration techniques
 * implemented in the ER7 utilities package.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_TRICK_INTEGRATOR_HH
#define ER7_UTILS_TRICK_INTEGRATOR_HH

// System includes
#include <cstddef>

// Trick includes
#include "sim_services/Integrator/include/Integrator.hh"

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/base_integration_group.hh"
#include "er7_utils/integration/core/include/integrator_constructor.hh"
#include "er7_utils/integration/core/include/integrator_interface.hh"
#include "er7_utils/integration/core/include/time_interface.hh"


namespace er7_utils {


/**
 * A TrickIntegrator extends the base Trick::Integrator class
 * to make use of er7_utils integration.
 *
 * The class TrickIntegrator inherits publicly (is-a) from Trick::Integrator.
 * It also inherits in protected access mode (implements-a) from
 * IntegratorInterface, TimeInterface, and BaseIntegrationGroup.
 */
class TrickIntegrator :
   public Trick::Integrator,
   protected IntegratorInterface,
   protected TimeInterface,
   protected BaseIntegrationGroup {

ER7_UTILS_MAKE_SIM_INTERFACES(TrickIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The copy constructor and assignment operators for this class are
   //     private / unimplemented.


public:

   /**
    * Indicates whether the first or second order ODE integrator
    * is to be used to propagate state.
    */
   enum IntegrationMode {
      InvalidIntegrationMode   = 0,  // Something is wrong
      UseFirstOrderIntegrator  = 1,  // Use the first_order_integrator
      UseSecondOrderIntegrator = 2   // Use the second_order_integrator
   };


   /**
    * Destructor.
    */
   virtual ~TrickIntegrator ()
   {
      deallocate_trick_arrays ();
      deallocate_integrators ();
   }


   // Trick::Integrator methods

   using Trick::Integrator::integrate;


   /**
    * Initialize the integrator.
    * @param[in] state_size  Size of the state vector
    * @param[in] delta_t     Nominal simulation engine time step
    */
   virtual void initialize (
      int state_size,
      double delta_t)
   {
      // Initialize as an integration group.
      initialize_group ();

      // Allocate storage, but first release any previously allocated storage.
      deallocate_integrators ();
      deallocate_trick_arrays ();
      initialize_trick_integrator (state_size, delta_t);
   }


   /**
    * Take the next integration step.
    * @param[in]      derivs_in     Time derivative of the state vector
    * @param[in,out]  state_in_out  State vector
    */
   virtual int integrate_1st_order_ode (
      double const * derivs_in,
      double * state_in_out)
   {
      if (intermediate_step == 0) {
         time_0 = time;
      }

      cached_derivs = derivs_in;
      cached_state = state_in_out;
      cached_accel = cached_velocity = cached_position = NULL;

      integ_mode = UseFirstOrderIntegrator;

      return integ_controls->integrate (
                time_0, dt,
                *this, *this, *this);
   }


   /**
    * Take the next integration step.
    * @param[in]      accel     Second time derivative of the position vector
    * @param[in,out]  velocity  Time derivative of the position vector
    * @param[in,out]  position  Position vector
    */
   virtual int integrate_2nd_order_ode (
      double const * accel,
      double * velocity,
      double * position)
   {
      if (intermediate_step == 0) {
         time_0 = time;
      }

      cached_accel    = accel;
      cached_velocity = velocity;
      cached_position = position;
      cached_derivs = cached_state = NULL;

      integ_mode = UseSecondOrderIntegrator;

      return integ_controls->integrate (
                time_0, dt,
                *this, *this, *this);
   }


protected:

   // Constructors.

   /**
    * TrickIntegrator default constructor.
    */
   TrickIntegrator ();


   /**
    * Surrogate TrickIntegrator copy constructor.
    * It doesn't make sense to copy the source object's integrator constructor.
    * The correct integrator constructor must instead be passed in from some
    * parent class.
    *
    * @param[in] source                The TrickIntegrator to be copied.
    * @param[in] integ_constructor_in  The integrator constructor object used to
    *                                  create the integrators.
    * Note well:@n
    *  - The newly constructed integrator will have it's own data buffers, with
    *    the contents copied from the source's data buffers.
    *    This is a deep copy.
    *  - Copying a default-constructed TrickIntegrator is undefined behavior.
    *  - Copying during the execution of an integration loop is also UB.
    */
   TrickIntegrator (
      const TrickIntegrator & source,
      IntegratorConstructor & integ_constructor_in);


   /**
    * TrickIntegrator non-default constructor.
    * @param[in] integ_constructor_in  The integrator constructor object used to
    *                                  create the integrators.
    * @param[in] use_deriv2_in         If true, integrate() is to use the
    *                                  Trick::Integrator deriv2 array as the
    *                                  source of second time derivatives.
    *                                  Otherwise, integrate() is to use the
    *                                  upper half of the Trick::Integrator
    *                                  deriv array as the source of second
    *                                  time derivatives.
    *                                  Note well: This flag pertains only to
    *                                  integration techniques that inherently
    *                                  are second order in nature.
    *                                  First order ODE techniques such as RK4
    *                                  do not use this flag.
    */
   explicit TrickIntegrator (
      IntegratorConstructor & integ_constructor_in,
      bool use_deriv2_in = false);


   /**
    * TrickIntegrator non-default constructor.
    * @param[in] integ_constructor_in  The integrator constructor object used to
    *                                  create the integrators.
    * @param[in] state_size            The size of the state vector.
    *                                  Note well: For a second order problem,
    *                                  the state size should be twice the
    *                                  dimensionality of the underlying problem.
    * @param[in] delta_t               The default time step.
    * @param[in] use_deriv2_in         If true, integrate() is to use the
    *                                  Trick::Integrator deriv2 array as the
    *                                  source of second time derivatives.
    *                                  Otherwise, integrate() is to use the
    *                                  upper half of the Trick::Integrator
    *                                  deriv array as the source of second
    *                                  time derivatives.
    *                                  Note well: This flag pertains only to
    *                                  integration techniques that inherently
    *                                  are second order in nature.
    *                                  First order ODE techniques such as RK4
    *                                  do not use this flag.
    */
   TrickIntegrator (
      IntegratorConstructor & integ_constructor_in,
      int state_size,
      double delta_t,
      bool use_deriv2_in = false);


   // TimeInterface methods

   /**
    * Update the time model given the simulation time.
    * @param[in] sim_time Simulation time
    */
   virtual void update_time (double sim_time)
   {
      time = sim_time;
   }

   /**
    * Return sim time to dynamic time scale factor.
    * Trick integration time is slaved to Trick simulation time.
    * @return Always returns 1.0
    */
   virtual double get_time_scale_factor () const
   {
      return 1.0;
   }


   // IntegratorInterface methods

   /**
    * Get the integration cycle time step.
    * @return                Simulation time delta t, in seconds
    */
   virtual double get_dt () const
   {
      return dt;
   }

   /**
    * Get the flag that tells the simulation engine to compute derivatives
    * on the initial step of each integration cycle.
    * @return Value of the first step derivatives flag
    */
   virtual bool get_first_step_derivs_flag () const
   {
      return first_step_deriv;
   }

   /**
    * Set the flag that tells the simulation engine to compute derivatives
    * on the initial step of each integration cycle.
    * @param[in] value       Value of the first step derivatives flag
    */
   virtual void set_first_step_derivs_flag (bool value)
   {
      first_step_deriv = value;
   }

   /**
    * Reset the flag that tells the simulation engine to compute derivatives
    * on the initial step of each integration cycle. Derivatives are always
    * needed just after a reset. The behavior should revert to nominal after
    * the reset has been performed.
    */
   virtual void reset_first_step_derivs_flag ()
   {
      default_first_step_deriv = first_step_deriv;
      first_step_deriv = true;
   }

   /**
    * Restore the flag that tells the simulation engine to compute derivatives
    * on the initial step of each integration cycle to it's value prior to
    * the most recent call to reset_first_step_derivs_flag.
    */
   virtual void restore_first_step_derivs_flag ()
   {
      first_step_deriv = default_first_step_deriv;
   }

   /**
    * Set the step number within an integration cycle.
    * @param[in] stepno      Step number
    */
   virtual void set_step_number (unsigned int stepno)
   {
      intermediate_step = stepno;
   }

   /**
    * Update the time model given the simulation time.
    * @param[in] sim_time Simulation time
    */
   virtual void set_time (double sim_time)
   {
      time = sim_time;
   }


   // BaseIntegrationGroup methods

   using BaseIntegrationGroup::initialize_group;


   /**
    * Update the interface with the sim engine to reflect integration status.
    * @param[in] step_number  The integration step number
    */
   virtual void update_integration_interface (
      unsigned int step_number)
   {
      intermediate_step = step_number;
   }


   /**
    * Integrate state.
    * @param cycle_dyndt   Integration cycle duration, in dynamic time seconds
    * @param target_stage  The stage the integrators are to try to attain
    * @return Integration status
    */
   virtual IntegratorResult integrate_bodies (
      double cycle_dyndt,
      unsigned int target_stage);


   /**
    * Reset the integrators.
    */
   virtual void reset_body_integrators ();


   // New methods.

   /**
    * Allocate state integrators for use with an integration technique that
    * solves a first order ODE. This function creates one integrator to solve
    * a first order ODE and another to solve a second order ODE. The latter
    * assumes an expanded state vector that comprises the state itself and
    * the first derivative of the state.
    * @param[in]     state_size         Number of elements in the state
    */
   void allocate_first_order_integrators (int state_size);

   /**
    * Allocate state integrators for use with an integration technique
    * that solves a second order ODE. This allocates an integrator to solve
    * a second order ODE, and if possible, a surrogate first order ODE solve
    * to solve a first order ODE.
    * @param[in]     state_size         Number of elements in the state
    */
   void allocate_second_order_integrators (int state_size);

   /**
    * Deallocate the state integrators.
    */
   void deallocate_integrators ();

   /**
    * Swap contents with some other TrickIntegrator.
    * @param[in,out] other  Object whose contents are to be swapped with
    *                       those of this object.
    */
   void swap (TrickIntegrator & other);

   using BaseIntegrationGroup::swap;


   // Member data

   double const * cached_derivs; /**< trick_units(--)@n
      Vector of derivatives used to communicate between
      integrate_1st_order_ode and integrate_bodies. */

   double * cached_state; /**< trick_units(--)@n
      State vector used to communicate between
      integrate_1st_order_ode and integrate_bodies. */

   double const * cached_accel; /**< trick_units(--)@n
      Vector of accelerations used to communicate between
      integrate_2nd_order_ode and integrate_bodies. */

   double * cached_velocity; /**< trick_units(--)@n
      Vector of velocities used to communicate between
      integrate_2nd_order_ode and integrate_bodies. */

   double * cached_position; /**< trick_units(--)@n
      Vector of positions used to communicate between
      integrate_2nd_order_ode and integrate_bodies. */


   double * er7_deriv; /**< trick_units(--)@n
      Allocated array of size buf_size used to trick out Trick's
      load_deriv. */

   double * er7_deriv2; /**< trick_units(--)@n
      Allocated array of size buf_size used to trick out Trick's
      load_deriv2. */

   const IntegratorConstructor * const
   trick_integ_constructor; /**< trick_units(--)@n
      The integrator constructor used to generate the state integrators
      this object. */

   FirstOrderODEIntegrator * first_order_integrator; /**< trick_units(--)@n
      The integrator that propagates state as a first order ODE. */

   SecondOrderODEIntegrator * second_order_integrator; /**< trick_units(--)@n
      The integrator that propagates state as a second order ODE. */


   unsigned int buf_size; /**< trick_units(--)@n
      Number of elements in the Trick::Integration group arrays. */

   int half_state_size; /**< trick_units(--)@n
      Number of elements in the Trick::Integration group arrays. */

   IntegrationMode integ_mode; /**< trick_units(--)@n
      Indicates which of the first or second order integrator is to be used. */

   bool default_first_step_deriv; /**< trick_units(--)@n
      Default value of the Trick::Integrator first_step_deriv flag. */


private:

   /**
    * Initialize as a Trick integrator.
    * @param[in] state_size  Number of elements in the state
    * @param[in] delta_t     Simulation engine time step
    */
   void initialize_trick_integrator (int state_size, double delta_t);

   /**
    * Initialize the Trick workspace.
    */
   void initialize_trick_workspace (void);

   /**
    * Initialize the buffers that mediate Trick and er7_utils integration.
    */
   void initialize_trick_interface (void);

   /**
    * Release memory allocated for the integrator.
    */
   void deallocate_trick_arrays (void);


   // Unimplemented member functions:
   // The copy constructor and assignment operators are not implemented.

   /**
    * Not implemented.
    */
   TrickIntegrator (const TrickIntegrator &);

   /**
    * Not implemented.
    */
   TrickIntegrator & operator= (const TrickIntegrator &);


   // OK! SWIG seems to think it's a good idea to copy from a Trick::Integrator,
   // even though copying from a TrickIntegrator is verbotten.
   // Makes sense to me! Or maybe not ...

   /**
    * Not implemented.
    */
   TrickIntegrator & operator= (const Trick::Integrator &);
};

}

#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

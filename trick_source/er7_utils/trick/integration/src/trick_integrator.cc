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
 * Implements the class Er7Integrator.
 */

/*
Purpose: ()
*/

// System includes
#include <iostream>

// Trick includes
#include "sim_services/Message/include/message_proto.h"
#include "sim_services/Message/include/message_type.h"

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Integration includes
#include "er7_utils/integration/core/include/integration_controls.hh"
#include "er7_utils/integration/core/include/first_order_ode_integrator.hh"
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"

// Local includes
#include "../include/trick_integrator.hh"


namespace er7_utils {


// TrickIntegrator default constructor.
TrickIntegrator::TrickIntegrator (
   void)
:
   // Construct parent classes with their default constructors.
   Trick::Integrator (),
   IntegratorInterface (),
   TimeInterface (),
   BaseIntegrationGroup (),

   // Initialize data members to nothing.
   cached_derivs (NULL),
   cached_state (NULL),
   cached_accel (NULL),
   cached_velocity (NULL),
   cached_position (NULL),
   trick_integ_constructor (NULL),
   first_order_integrator (NULL),
   second_order_integrator (NULL),
   buf_size (0),
   half_state_size (0),
   integ_mode (InvalidIntegrationMode)
{}


// TrickIntegrator copy constructor surrogate.
TrickIntegrator::TrickIntegrator (
   const TrickIntegrator & source,
   IntegratorConstructor & integ_constructor_in)
:
   // Except for the BaseIntegrationGroup, which is special,
   // construct parent classes with their default constructors.
   // Construct the BaseIntegrationGroup so it knows that this object
   // serves as both the integration interface and time interface.
   Trick::Integrator (),
   IntegratorInterface (),
   TimeInterface (),
   BaseIntegrationGroup (integ_constructor_in, *this, *this),

   cached_derivs (NULL),
   cached_state (NULL),
   cached_accel (NULL),
   cached_velocity (NULL),
   cached_position (NULL),
   trick_integ_constructor (&integ_constructor_in),
   first_order_integrator (NULL),
   second_order_integrator (NULL),
   buf_size (source.buf_size),
   half_state_size (source.half_state_size),
   integ_mode (source.integ_mode)
{
   // Copy Trick::Integrator members from the source.
   use_deriv2 = source.use_deriv2;
   num_state = source.num_state;
   dt = source.dt;

   // Replicate data and complete initialization if those data exist.
   if (num_state != 0) {
      er7_deriv  = alloc::replicate_array (num_state, er7_deriv);
      er7_deriv2 = alloc::replicate_array (num_state, er7_deriv2);

      state = alloc::replicate_array (num_state, state);

      initialize_trick_workspace ();
      initialize_trick_interface ();
   }
}


// Non-default constructor.
TrickIntegrator::TrickIntegrator (
   IntegratorConstructor & integ_constructor_in,
   bool use_deriv2_in)
:
   // Except for the BaseIntegrationGroup, which is special,
   // construct parent classes with their default constructors.
   // Construct the BaseIntegrationGroup so it knows that this object
   // serves as both the integration interface and time interface.
   Trick::Integrator(),
   IntegratorInterface(),
   TimeInterface (),
   BaseIntegrationGroup(integ_constructor_in, *this, *this),

   // Initialize data members based on the arguments.
   // Most are still null/0; trick_integ_constructor and buf_size are not.
   cached_derivs(NULL),
   cached_state(NULL),
   cached_accel(NULL),
   cached_velocity(NULL),
   cached_position(NULL),
   trick_integ_constructor(&integ_constructor_in),
   first_order_integrator(NULL),
   second_order_integrator(NULL),
   buf_size(integ_constructor_in.get_buffer_size()),
   integ_mode(InvalidIntegrationMode)
{
   // Set the Trick::Integrator::use_deriv2 data member.
   use_deriv2 = use_deriv2_in;
}


// Non-default constructor.
TrickIntegrator::TrickIntegrator (
   IntegratorConstructor & integ_constructor_in,
   int state_size,
   double delta_t,
   bool use_deriv2_in)
:
   // Except for the BaseIntegrationGroup, which is special,
   // construct parent classes with their default constructors.
   // Construct the BaseIntegrationGroup so it knows that this object
   // serves as both the integration interface and time interface.
   Trick::Integrator(),
   IntegratorInterface(),
   TimeInterface (),
   BaseIntegrationGroup (integ_constructor_in, *this, *this),

   cached_derivs(NULL),
   cached_state(NULL),
   cached_accel(NULL),
   cached_velocity(NULL),
   cached_position(NULL),
   trick_integ_constructor(&integ_constructor_in),
   first_order_integrator(NULL),
   second_order_integrator(NULL),
   buf_size(integ_constructor_in.get_buffer_size()),
   integ_mode(InvalidIntegrationMode)
{
   // Set the Trick::Integrator::use_deriv2 data member.
   use_deriv2 = use_deriv2_in;

   // Initialize the integrator.
   // Note that this always calls TrickIntegrator::initialize();
   // Derived classes don't exist yet.
   initialize (state_size, delta_t);
}


// Initialize the integrator.
void
TrickIntegrator::initialize_trick_integrator (
   int state_size,
   double delta_t)
{
   num_state = state_size;
   dt        = delta_t;

   half_state_size = state_size / 2;

   // Allocate space for the er7 utils side of things.
   er7_deriv  = alloc::allocate_array (num_state);
   er7_deriv2 = alloc::allocate_array (num_state);

   // Allocate space for the Trick::Integrator buffers
   state = alloc::allocate_array (num_state);

   // Initialize the Trick workspace and the Trick/er7_utils interface.
   initialize_trick_workspace ();
   initialize_trick_interface ();
}


// Initialize the Trick workspace.
void
TrickIntegrator::initialize_trick_workspace (
   void)
{
   // This is a nasty little hack, but it saves a whole lot of copying of data.
   deriv    = alloc::allocate_array<double*> (buf_size);
   deriv2   = alloc::allocate_array<double*> (buf_size);
   state_ws = alloc::allocate_array<double*> (buf_size);

   for (unsigned int ii = 0; ii < buf_size; ++ii) {
      deriv[ii]    = er7_deriv;
      deriv2[ii]   = er7_deriv2;
      state_ws[ii] = state;
   }
}


// Initialize the Trick/er7_utils interface.
void
TrickIntegrator::initialize_trick_interface (
   void)
{
   if (use_deriv2) {
      cached_accel = er7_deriv2;
   }
   else {
      cached_accel = er7_deriv + half_state_size;
   }
   cached_velocity = state + half_state_size;
   cached_position = state;

   cached_derivs = er7_deriv;
   cached_state  = state;
}


// Allocate state integrators for use with a first order ODE technique.
void
TrickIntegrator::allocate_first_order_integrators (
   int state_size)
{
   first_order_integrator =
      trick_integ_constructor->create_first_order_ode_integrator (
         state_size, *integ_controls);

   if (half_state_size*2 == state_size) {
      second_order_integrator =
         trick_integ_constructor->create_second_order_ode_integrator (
            half_state_size, *integ_controls);
   }
   else {
      second_order_integrator = NULL;
   }
}


// Allocate state integrators for use with a second order ODE technique.
void
TrickIntegrator::allocate_second_order_integrators (
   int state_size)
{

   if (2*half_state_size != state_size) {
      message_publish (
         MSG_ERROR,
         "Integrator ERROR: State size is not a multiple of two.\n");
   }

   if (trick_integ_constructor->provides (
          er7_utils::Integration::FirstOrderODE)) {
      first_order_integrator =
         trick_integ_constructor->create_first_order_ode_integrator (
            state_size, *integ_controls);
   }
   else {
      first_order_integrator = NULL;
   }

   second_order_integrator =
      trick_integ_constructor->create_second_order_ode_integrator (
                         half_state_size, *integ_controls);
}


// Deallocate the state integrators.
void TrickIntegrator::deallocate_integrators (
   void)
{
   alloc::delete_object (first_order_integrator);
   alloc::delete_object (second_order_integrator);
}


// Release memory allocated for the integrator.
void
TrickIntegrator::deallocate_trick_arrays (
   void)
{
   alloc::deallocate_array (er7_deriv);
   alloc::deallocate_array (er7_deriv2);
   alloc::deallocate_array (state);
   alloc::deallocate_array (deriv);
   alloc::deallocate_array (deriv2);
   alloc::deallocate_array (state_ws);
}


// Reset the integrators.
void
TrickIntegrator::reset_body_integrators (
   void)
{
   if (first_order_integrator != NULL) {
      first_order_integrator->reset_integrator ();
   }
   if (second_order_integrator != NULL) {
      second_order_integrator->reset_integrator ();
   }
}


// Integrate state.
IntegratorResult
TrickIntegrator::integrate_bodies (
   double dyndt,
   unsigned int next_stage)
{
   switch (integ_mode) {

   // Call in user code was to integrate() for a first order ODE technique,
   // or to integrate_1st_order_ode().
   // Arguments were saved as cached_derivs and cached_state prior to
   // call to integrate (time_0, dt) in TrickEr7Integrator.
   case UseFirstOrderIntegrator :
      return first_order_integrator->integrate (
                dyndt, next_stage, cached_derivs, cached_state);
      break;

   // Call in user code was to integrate() for a second order ODE technique,
   // or to integrate_2nd_order_ode().
   // Arguments were saved as cached_accel, cached_velocity, and cached_position
   // prior to call to integrate (time_0, dt) in TrickEr7Integrator.
   case UseSecondOrderIntegrator :
      return second_order_integrator->integrate (
                dyndt, next_stage,
                cached_accel, cached_velocity, cached_position);
      break;

   // Something went wrong.
   case InvalidIntegrationMode :
   default :
      message_publish (
         MSG_ERROR,
         "Integrator ERROR: Improper use of integration interface.\n");
      intermediate_step = 0;
      return 1.0;
      break;
   }
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

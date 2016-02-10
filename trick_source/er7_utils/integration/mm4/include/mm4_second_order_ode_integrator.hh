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
 * Defines the class MM4SecondOrderODEIntegrator, which integrates a state
 * comprising a zeroth derivative / first derivative pair via the MM4 method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_MM4_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_MM4_TWO_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Model includes
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Propagate state using the modified midpoint 4 method.
 */
class MM4SecondOrderODEIntegrator : public SecondOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(MM4SecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * MM4SecondOrderODEIntegrator destructor.
    */
   virtual ~MM4SecondOrderODEIntegrator (void);


protected:

   // Constructors.

   /**
    * MM4SecondOrderODEIntegrator default constructor.
    */
   MM4SecondOrderODEIntegrator (void);

   /**
    * MM4SecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   MM4SecondOrderODEIntegrator (const MM4SecondOrderODEIntegrator & src);

   /**
    * MM4SecondOrderODEIntegrator non-default constructor
    * for a simple second order ODE.
    * This constructor is used by the integrator constructor.
    * @param[in]     size                State size
    * @param[in,out] controls            Integration controls
    */
   MM4SecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);

   /**
    * MM4SecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the position derivative function.
    * @param[in]     position_size       Size of the generalized position
    * @param[in]     velocity_size       Size of the generalized velocity
    * @param[in]     deriv_funs          Position derivative functions container
    * @param[in,out] controls            Integration controls
    */
   MM4SecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls);


   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (MM4SecondOrderODEIntegrator & other);

   using SecondOrderODEIntegrator::swap;


   // Member data.

   double * pos_hist[3]; /**< trick_units(--) @n
      Position history, initial plus results from first two steps. */

   double * vel_hist[3]; /**< trick_units(--) @n
      Position history, initial plus results from first two steps. */

   double * posdot; /**< trick_units(--) @n
      Derivative of generalized position. */


private:

   /**
    * Not implemented.
    */
   MM4SecondOrderODEIntegrator & operator= (
      const MM4SecondOrderODEIntegrator &);
};


/**
 * Specialization of MM4SecondOrderODEIntegrator for the case of
 * generalized velocity being the time derivative of generalized position.
 */
class MM4SimpleSecondOrderODEIntegrator :
   public MM4SecondOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(MM4SimpleSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * MM4SimpleSecondOrderODEIntegrator default constructor.
    */
   MM4SimpleSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      MM4SecondOrderODEIntegrator ()
   {}

   /**
    * MM4SimpleSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   MM4SimpleSecondOrderODEIntegrator (
      const MM4SimpleSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      MM4SecondOrderODEIntegrator (src)
   {}

   /**
    * MM4SimpleSecondOrderODEIntegrator non-default constructor.
    * @param[in]     size      Size of the position and velocity vectors
    * @param[in,out] controls  Integration controls
    */
   MM4SimpleSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      MM4SecondOrderODEIntegrator (size, controls)
   {}

   /**
    * MM4SimpleSecondOrderODEIntegrator destructor.
    */
   virtual ~MM4SimpleSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * MM4SimpleSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   MM4SimpleSecondOrderODEIntegrator & operator= (
      MM4SimpleSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' MM4SimpleSecondOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual MM4SimpleSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state via MM4 for the special case of velocity being the
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
 * Specialization of MM4SecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class MM4GeneralizedDerivSecondOrderODEIntegrator :
   public MM4SecondOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(MM4GeneralizedDerivSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * MM4GeneralizedDerivSecondOrderODEIntegrator default constructor.
    */
   MM4GeneralizedDerivSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      MM4SecondOrderODEIntegrator ()
   {}

   /**
    * MM4GeneralizedDerivSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   MM4GeneralizedDerivSecondOrderODEIntegrator (
      const MM4GeneralizedDerivSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      MM4SecondOrderODEIntegrator (src)
   {}

   /**
    * MM4GeneralizedDerivSecondOrderODEIntegrator non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   MM4GeneralizedDerivSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      MM4SecondOrderODEIntegrator (position_size, velocity_size,
                                   deriv_funs, controls)
   {}

   /**
    * MM4GeneralizedDerivSecondOrderODEIntegrator destructor.
    */
   virtual ~MM4GeneralizedDerivSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * MM4GeneralizedDerivSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   MM4GeneralizedDerivSecondOrderODEIntegrator & operator= (
      MM4GeneralizedDerivSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual MM4GeneralizedDerivSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state via MM4 for generalized position and generalized velocity
    * where generalized position is advanced using the function that yields
    * the time derivative of generalized position.
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
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

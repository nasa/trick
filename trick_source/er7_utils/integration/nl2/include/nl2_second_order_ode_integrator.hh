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
 * Defines the class NystromLear2SecondOrderODEIntegrator, which integrates a
 * state comprising a zeroth derivative / first derivative pair via the
 * second order Nystrom-Lear integration technique.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_NL2_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_NL2_TWO_STATE_INTEGRATOR_HH

// System includes

// ER7 utilities includes
#include "er7_utils/interface/include/er7_class.hh"

// Model includes
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Propagate state using 2nd order Nystrom-Lear.
 */
class NystromLear2SecondOrderODEIntegrator : public SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(NystromLear2SecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * NystromLear2SecondOrderODEIntegrator destructor.
    */
   virtual ~NystromLear2SecondOrderODEIntegrator (void);


protected:

   // Constructors.

   /**
    * NystromLear2SecondOrderODEIntegrator default constructor.
    * Notes:
    *  - This implicitly assumes the time derivative of position is velocity.
    *  - This is needed so that Trick can checkpoint/restart its integrators.
    */
   NystromLear2SecondOrderODEIntegrator (void);

   /**
    * NystromLear2SecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   NystromLear2SecondOrderODEIntegrator (
      const NystromLear2SecondOrderODEIntegrator & src);

   /**
    * NystromLear2SecondOrderODEIntegrator non-default constructor
    * for a simple second order ODE, one in which generalized velocity
    * is time derivative of generalized position.
    * @param[in]     size                State size
    * @param[in,out] controls            Integration controls
    */
   NystromLear2SecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);

   /**
    * NystromLear2SecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the provided derivative function.
    * This constructor is used by the integrator constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   NystromLear2SecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls);


   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (NystromLear2SecondOrderODEIntegrator & other);

   using SecondOrderODEIntegrator::swap;


   // Member data.

   double * posdot; /**< trick_units(--) @n
      Derivative of generalized position. */


private:

   /**
    * Not implemented.
    */
   NystromLear2SecondOrderODEIntegrator & operator= (
      const NystromLear2SecondOrderODEIntegrator &);
};


/**
 * Specialization of NystromLear2SecondOrderODEIntegrator for the case of
 * generalized velocity being the time derivative of generalized position.
 */
class NystromLear2SimpleSecondOrderODEIntegrator :
   public NystromLear2SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(NystromLear2SimpleSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * NystromLear2SimpleSecondOrderODEIntegrator default constructor.
    */
   NystromLear2SimpleSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      NystromLear2SecondOrderODEIntegrator ()
   {}

   /**
    * NystromLear2SimpleSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   NystromLear2SimpleSecondOrderODEIntegrator (
      const NystromLear2SimpleSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      NystromLear2SecondOrderODEIntegrator(src)
   {}

   /**
    * NystromLear2SimpleSecondOrderODEIntegrator non-default constructor.
    * @param[in]     size      Size of the position and velocity vectors
    * @param[in,out] controls  Integration controls
    */
   NystromLear2SimpleSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      NystromLear2SecondOrderODEIntegrator (size, controls)
   {}

   /**
    * NystromLear2SimpleSecondOrderODEIntegrator destructor.
    */
   virtual ~NystromLear2SimpleSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * NystromLear2SimpleSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   NystromLear2SimpleSecondOrderODEIntegrator & operator= (
      NystromLear2SimpleSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this'.
    * @return Clone of 'this'.
    */
   virtual NystromLear2SimpleSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state using Nystrom Lear 2.
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
 * Specialization of NystromLear2SecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class NystromLear2GeneralizedDerivSecondOrderODEIntegrator :
   public NystromLear2SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(
   NystromLear2GeneralizedDerivSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * NystromLear2GeneralizedDerivSecondOrderODEIntegrator default constructor.
    */
   NystromLear2GeneralizedDerivSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      NystromLear2SecondOrderODEIntegrator ()
   {}

   /**
    * NystromLear2GeneralizedDerivSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   NystromLear2GeneralizedDerivSecondOrderODEIntegrator (
      const NystromLear2GeneralizedDerivSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      NystromLear2SecondOrderODEIntegrator (src)
   {}

   /**
    * NystromLear2GeneralizedDerivSecondOrderODEIntegrator
    * non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   NystromLear2GeneralizedDerivSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions deriv_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      NystromLear2SecondOrderODEIntegrator (position_size, velocity_size,
                                            deriv_funs, controls)
   {}

   /**
    * NystromLear2GeneralizedDerivSecondOrderODEIntegrator destructor.
    */
   virtual ~NystromLear2GeneralizedDerivSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * NystromLear2GeneralizedDerivSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   NystromLear2GeneralizedDerivSecondOrderODEIntegrator & operator= (
      NystromLear2GeneralizedDerivSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual NystromLear2GeneralizedDerivSecondOrderODEIntegrator * create_copy ()
   const;

   /**
    * Propagate state using Nystrom Lear 2.
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

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
 * Defines the class FirstOrderODEIntegrator, the base class for propagating
 * state that is conceptually monolithic.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_ONE_STATE_INTEGRATOR_HH
#define ER7_UTILS_ONE_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Local includes
#include "state_integrator.hh"
#include "integrator_result.hh"


namespace er7_utils {

class IntegrationControls;


/**
 * Base class for propagating monolithic states.
 */
class FirstOrderODEIntegrator : public StateIntegratorInterface {

ER7_UTILS_MAKE_SIM_INTERFACES(FirstOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * FirstOrderODEIntegrator destructor.
    */
   virtual ~FirstOrderODEIntegrator (void) { }


   // Member functions.

   /**
    * Create a copy of 'this', a derived FirstOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual FirstOrderODEIntegrator* create_copy () const = 0;

   /**
    * Set the controls object that guides this object's integration process.
    * The default implementation does nothing.
    * @param[in,out] controls  Integration controls (unused)
    */
   virtual void set_controls (
      IntegrationControls & controls ER7_UTILS_UNUSED)
   { }

   /**
    * Propagate state to the specified stage of the integration
    * process for an overall integration time interval of dyn_dt.
    *
    * All integration techniques can assume that the status parameter will
    * be default-initialized on entry: time scales set to 1.0, failure status
    * and merge count set to zero. An integration technique only needs to change
    * the time scales if they are something other than 1.0, the failure mode
    * if the technique somehow is not successful.
    *
    * Note that this is a pure virtual function; instantiable subclasses must
    * provide an implementation of this method.
    *
    * @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
    * @param[in]     target_stage  The stage of the integration process
    *                              that the integrator should try to attain.
    * @param[in,out] velocity      Generalized velocity vector.
    * @param[in,out] position      Generalized position vector.
    *
    * @return The status (time advance, pass/fail status) of the integration.
    */
   virtual IntegratorResult integrate (
      double dyn_dt,
      unsigned int target_stage,
      const double * ER7_UTILS_RESTRICT velocity,
      double * ER7_UTILS_RESTRICT position) = 0;


protected:

   // Constructors.

   /**
    * FirstOrderODEIntegrator default constructor.
    */
   FirstOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      StateIntegratorInterface (),
      state_size (0)
   { }

   /**
    * FirstOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   FirstOrderODEIntegrator (const FirstOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      StateIntegratorInterface (src),
      state_size (src.state_size)
   { }

   /**
    * FirstOrderODEIntegrator non-default constructor.
    * Notes:
    *  - The provided size is assumed to be non-zero.
    *    Behavior is undefined if size is zero.
    *  - The controls parameter exists for derived classes.
    *    It's unused in the default implementation.
    * @param[in]     size      State size
    * @param[in,out] controls  Integration controls (unused)
    */
   FirstOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls ER7_UTILS_UNUSED)
   :
      StateIntegratorInterface (),
      state_size (size)
   { }


   // Member functions.

   /**
    * Swap contents with that of another.
    * @param other  Other object with which contents are to be swapped.
    */
   virtual void swap (FirstOrderODEIntegrator & other);


   // Member data

   unsigned int state_size; /**< trick_units(--) @n
      The size of the state. */


private:

   /**
    * Not implemented.
    */
   FirstOrderODEIntegrator & operator= (const FirstOrderODEIntegrator &);
};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

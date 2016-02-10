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
 * Defines the class IntegrableObject.
 */

/*
Purpose: ()
*/

#ifndef ER7_UTILS_INTEGRABLE_BODY_HH
#define ER7_UTILS_INTEGRABLE_BODY_HH

// System includes
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"


namespace er7_utils {

class BaseIntegrationGroup;
class IntegrationControls;
class IntegratorConstructor;
class IntegratorResult;
class TimeInterface;

/**
 * An IntegrableObject is managed by an IntegrationGroup and under direction
 * of that group, the IntegrableObject integrates the state of some object.
 *
 * Note that an IntegrableObject is not an Er7UtilsDeletable.
 */
class IntegrableObject {
ER7_UTILS_MAKE_SIM_INTERFACES(IntegrableObject)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * Destructor.
    */
   virtual ~IntegrableObject () {}

   /**
    * Create the integration model state integrators that the object will
    * use to integrate state.@n
    * An implementation will typically call one of IntegratorConstructor's
    * create_XXX_integrator methods to create the necessary integrators.
    * @param[in] generator  Integrator constructor to be used to create
                            state integrators.
    * @param[in] controls   The integration controls created the integrator
                            constructor's create_integration_controls method.
    * @param[in] time_if    Time interface that timestamps the integrated state.
    */
   virtual void create_integrators (
      const IntegratorConstructor & generator,
      IntegrationControls & controls,
      const TimeInterface & time_if) = 0;

   /**
    * Destroy the integrators created by the create_integrators method.
    */
   virtual void destroy_integrators (void) = 0;

   /**
    * Restore the integrators on restart.
    */
   virtual void reset_integrators (void) = 0;

   /**
    * Integrate state by the specified dynamic time interval.
    *
    * All IntegrableObjects can assume that the status parameter will
    * be default-initialized on entry: time scales set to 1.0, failure status
    * and merge count set to zero. An integration technique only needs to change
    * the time scales if they are something other than 1.0, the failure mode
    * if the technique somehow is not successful.
    *
    * @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
    * @param[in]     target_stage  The stage of the integration process
    *                              that the integrator should try to attain.
    *
    * @return The status (time advance, pass/fail status) of the integration.
    */
   virtual IntegratorResult integrate (
      double dyn_dt,
      unsigned int target_stage)
   = 0;


   /**
    * Setter for the integration group.
    * @param[in] integ_group_in New integration group
    */
   void set_integration_group (
      BaseIntegrationGroup & integ_group_in)
   {
      if (integ_group != NULL) {
         destroy_integrators ();
      }
      integ_group = &integ_group_in;
   }

   /**
    * Reset (clear) the integration group.
    */
   void clear_integration_group (void)
   {
      if (integ_group != NULL) {
         destroy_integrators ();
      }
      integ_group = NULL;
   }

   /**
    * Getter for the integration group.
    * @return Current integration group
    */
   BaseIntegrationGroup * get_integration_group (void)
   {
      return integ_group;
   }


protected:

   /**
    * Default constructor.
    */
   IntegrableObject () : integ_group(NULL) {}

   /**
    * Copy constructor.
    * Note: The integration group is not copied. A copy of an IntegrableObject
    * does not belong to the same group as the source object.
    * It has to be put in some group, explicitly.
    */
   IntegrableObject (const IntegrableObject &) : integ_group(NULL) {}

   /**
    * Non-throwing swap.
    * @param other  Object whose contents are to be swapped.
    */
   void swap (IntegrableObject & other)
   {
      BaseIntegrationGroup * temp = integ_group;
      integ_group = other.integ_group;
      other.integ_group = temp;
   }

private:

   // Member data

   BaseIntegrationGroup * integ_group; /**< trick_units(--) @n
      The integration group that integrates this body, and possibly others. */


   // Unimplemented member functions

   /**
    * Not implemented.
    */
   IntegrableObject & operator= (const IntegrableObject &);

};

}


#endif

/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

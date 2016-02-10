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
 * Defines the class SingleCycleIntegrationControls, the class used for
 * controlling most of the integration techniques.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_SINGLE_CYCLE_INTEGRATION_CONTROLS_HH
#define ER7_UTILS_SINGLE_CYCLE_INTEGRATION_CONTROLS_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Local includes
#include "integration_controls.hh"


namespace er7_utils {

/**
   A single cycle integration controls object provides mechanisms for
    controlling integrators that need a simple single cycle process.
*/
class SingleCycleIntegrationControls : public IntegrationControls {
ER7_UTILS_MAKE_SIM_INTERFACES(SingleCycleIntegrationControls)

public:

   // Constructors and destructor.
   // Note: This class has no direct data members.
   // The implicitly-defined copy constructor and destructor
   // work quite nicely and are not declared.


   /**
    * SingleCycleIntegrationControls default constructor.
    */
   SingleCycleIntegrationControls () : IntegrationControls()
   { }


   /**
    * SingleCycleIntegrationControls non-default constructor.
    * @param[in] number_stages_in  Number operational stages
    */
   explicit SingleCycleIntegrationControls (unsigned int number_stages_in)
   :
      IntegrationControls(number_stages_in)
   { }


   // Member functions

   /**
    * Copy-and-swap SingleCycleIntegrationControls assignment operator.
    * @param[in] src  The IntegrationControls to be copied.
    */
   SingleCycleIntegrationControls &
   operator= (SingleCycleIntegrationControls src)
   {
      swap (src);
      return *this;
   }


   /**
    * Create a copy of 'this' SingleCycleIntegrationControls object.
    * @return Clone of 'this'.
    */
   virtual SingleCycleIntegrationControls * create_copy () const;


   /**
    * Perform one step of the integration process.
    * @return Step number; zero when finished.
    * @param[in] start_time
    *    The simulation engine time from which the integrators start.
    * @param[in] sim_dt
    *    The difference between the simulation time at the end and start of the
    *    integration tour.
    * @param[in,out] time_if
    *    Object external to the ER7 utilities suite that represents time.
    * @param[in,out] integ_interface
    *    Interface with the simulation engine for this integration controls.
    * @param[in,out] integ_group
    *    The integration group that contains this integration controls.
    */
   virtual unsigned int integrate (
      double start_time, double sim_dt,
      TimeInterface & time_if,
      IntegratorInterface & integ_interface,
      BaseIntegrationGroup & integ_group);
};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

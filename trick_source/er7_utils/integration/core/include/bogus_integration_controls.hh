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
 * Defines the class BogusIntegrationControls.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_BOGUS_INTEGRATION_CONTROLS_HH
#define ER7_UTILS_BOGUS_INTEGRATION_CONTROLS_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/alloc.hh"
#include "er7_utils/interface/include/er7_class.hh"
#include "er7_utils/interface/include/message_handler.hh"

// Local includes
#include "../include/integration_controls.hh"
#include "../include/integration_messages.hh"


namespace er7_utils {

/**
 * This class exists for one purpose and one purpose only, which is to enable
 * an integration group to always have a non-null integration controls, even
 * before the group has been properly initialized.
 * That a group has a non-null controls object means that it is always safe
 * (from a core dump perspective) to invoke that object's integrate method.
 * This avoids a run-time check for a null pointer at the integration rate.
 * The integrate method for a BogusIntegrationControls object dies,
 * but it does not drop core.
 */
class BogusIntegrationControls : public IntegrationControls {
ER7_UTILS_MAKE_SIM_INTERFACES(BogusIntegrationControls)

public:

   /**
    * BogusIntegrationControls assignment operator.
    * @param[in] src  Object to be copied.
    */
   BogusIntegrationControls &
   operator = (BogusIntegrationControls src)
   {
      swap (src);
      return *this;
   }

   /**
    * Nominally, integrate.
    * A BogusIntegrationControls dies instead.
    */
   virtual unsigned int integrate (
      double, double,
      TimeInterface&, IntegratorInterface&, BaseIntegrationGroup&);

   /**
    * Create a copy of 'this' BogusIntegrationControls object.
    * @return Clone of 'this'.
    */
   virtual BogusIntegrationControls * create_copy () const;
};

}

#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

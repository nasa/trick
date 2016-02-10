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
 * Defines member functions for the class IntegratorResultMergerContainer.
 */

/*
Purpose: ()
*/


// Local includes
#include "../include/integrator_result_merger_container.hh"
#include "../include/integrator_constructor.hh"

// System includes
#include <algorithm>
#include <cstddef>


namespace er7_utils {

// IntegratorResultMergerContainer default constructor.
IntegratorResultMergerContainer::IntegratorResultMergerContainer ()
:
   integ_merger(NULL)
{
   integ_merger = BogusIntegratorResultMerger().create_copy();
}


// IntegratorResultMergerContainer non-default constructor.
IntegratorResultMergerContainer::IntegratorResultMergerContainer (
   const er7_utils::IntegratorConstructor & integ_cotr)
:
   integ_merger(NULL)
{
   integ_merger = integ_cotr.create_integrator_results_merger();
}


// IntegratorResultMergerContainer copy constructor.
IntegratorResultMergerContainer::IntegratorResultMergerContainer (
   const IntegratorResultMergerContainer & src)
:
   integ_merger(NULL)
{
   integ_merger = src.integ_merger->create_copy();
}


// IntegratorResultMergerContainer destructor.
IntegratorResultMergerContainer::~IntegratorResultMergerContainer ()
{
   Er7UtilsDeletable::delete_instance (integ_merger);
}


// Swap.
void
swap (
   IntegratorResultMergerContainer & a,
   IntegratorResultMergerContainer & b)
{
   std::swap (a.integ_merger, b.integ_merger);
}


// Configure for use with a specific integration technique.
void
IntegratorResultMergerContainer::configure (
   const er7_utils::IntegratorConstructor & integ_cotr)
{
   Er7UtilsDeletable::delete_instance (integ_merger);
   integ_merger = integ_cotr.create_integrator_results_merger();
}


}


/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

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
 * Defines member functions for the class FirstOrderODEIntegrator.
 */

/*
Purpose: ()
*/

// System includes
#include <algorithm>

// Local includes
#include "../include/first_order_ode_integrator.hh"


namespace er7_utils {

// Swap
void
FirstOrderODEIntegrator::swap (
   FirstOrderODEIntegrator & other)
{
   std::swap (state_size, other.state_size);
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

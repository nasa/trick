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
 * Defines convenience function templates for use by integrator constructors.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_INTEGRATOR_CONSTRUCTOR_UTILS_HH
#define ER7_UTILS_INTEGRATOR_CONSTRUCTOR_UTILS_HH

// Interface includes
#include "er7_utils/interface/include/alloc.hh"


namespace er7_utils {

class IntegrationControls;
class IntegratorConstructor;
class GeneralizedPositionDerivativeFunctions;
class GeneralizedPositionStepFunctions;

namespace integ_utils {

/**
 * Create an integration controls instance.
 * @tparam T        Integration controls derived class
 * @param  nstages  Number of stages in the integration cycle
 */
template<typename T>
inline T* allocate_controls (
   unsigned int nstages)
{
   return alloc::allocate_object<T, unsigned int> (nstages);
}


/**
 * Create an integration controls instance.
 * @tparam T                   Integration controls derived class
 * @param  primer_constructor  Constructor that creates the primer
 * @param  history_length      History length
 * @param  nstages             Number of stages in the integration cycle
 */
template<typename T>
inline T* allocate_controls (
   const IntegratorConstructor & primer_constructor,
   unsigned int history_length,
   unsigned int nstages)
{
   return alloc::allocate_object<T,
                                 const IntegratorConstructor &,
                                 unsigned int,
                                 unsigned int> (
             primer_constructor, history_length, nstages);
}


/**
 * Create a state integrator instance.
 * @tparam T         State integrator derived class
 * @param  size      State size
 * @param  controls  Integration controls
 */
template<typename T>
inline T* allocate_integrator (
   unsigned int size,
   IntegrationControls & controls)
{
   return alloc::allocate_object<T,
                                 unsigned int,
                                 IntegrationControls &> (
             size, controls);
}


/**
 * Create a state integrator instance.
 * @tparam T                   State integrator derived class
 * @param  primer_constructor  Constructor that creates the primer
 * @param  size                State size
 * @param  controls            Integration controls
 */
template<typename T>
inline T* allocate_integrator (
   const IntegratorConstructor & primer_constructor,
   unsigned int size,
   IntegrationControls & controls)
{
   return alloc::allocate_object<T,
                                 const IntegratorConstructor &,
                                 unsigned int,
                                 IntegrationControls &> (
             primer_constructor, size, controls);
}


/**
 * Create a state integrator instance.
 * @tparam T              State integrator derived class
 * @param  position_size  Generalized position size
 * @param  velocity_size  Generalized velocity size
 * @param  deriv_funs     Position derivative functions container
 * @param  controls       Integration controls
 */
template<typename T>
inline T* allocate_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
{
   return alloc::allocate_object<T,
                                 unsigned int,
                                 unsigned int,
                                 const GeneralizedPositionDerivativeFunctions &,
                                 IntegrationControls &> (
             position_size, velocity_size, deriv_funs, controls);
}


/**
 * Create a state integrator instance.
 * @tparam T                   State integrator derived class
 * @param  primer_constructor  Constructor that creates the primer
 * @param  position_size       Generalized position size
 * @param  velocity_size       Generalized velocity size
 * @param  deriv_funs          Position derivative functions container
 * @param  controls            Integration controls
 */
template<typename T>
inline T* allocate_integrator (
   const IntegratorConstructor & primer_constructor,
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
{
   return alloc::allocate_object<T,
                                 const IntegratorConstructor &,
                                 unsigned int,
                                 unsigned int,
                                 const GeneralizedPositionDerivativeFunctions &,
                                 IntegrationControls &> (
             primer_constructor,
             position_size, velocity_size, deriv_funs, controls);
}


/**
 * Create a state integrator instance.
 * @tparam T              State integrator derived class
 * @param  position_size  Generalized position size
 * @param  velocity_size  Generalized velocity size
 * @param  step_funs      Position step functions container
 * @param  controls       Integration controls
 */
template<typename T>
inline T* allocate_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
{
   return alloc::allocate_object<T,
                                 unsigned int,
                                 unsigned int,
                                 const GeneralizedPositionStepFunctions &,
                                 IntegrationControls &> (
             position_size, velocity_size, step_funs, controls);
}


/**
 * Create a state integrator instance.
 * @tparam T                   State integrator derived class
 * @param  primer_constructor  Constructor that creates the primer
 * @param  position_size       Generalized position size
 * @param  velocity_size       Generalized velocity size
 * @param  step_funs           Position step functions container
 * @param  controls            Integration controls
 */
template<typename T>
inline T* allocate_integrator (
   const IntegratorConstructor & primer_constructor,
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
{
   return alloc::allocate_object<T,
                                 const IntegratorConstructor &,
                                 unsigned int,
                                 unsigned int,
                                 const GeneralizedPositionStepFunctions &,
                                 IntegrationControls &> (
             primer_constructor,
             position_size, velocity_size, step_funs, controls);
}

}

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

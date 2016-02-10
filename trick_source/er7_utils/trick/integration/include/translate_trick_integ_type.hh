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
 * Defines the free function translate_trick_integrator_type, which translates a
 * Trick Integrator_type to an er7_utils Integration::Technique.
 */

/*
Purpose: ()
*/

#ifndef ER7_UTILS_TRANSLATE_TRICK_INTEG_TYPE_HH
#define ER7_UTILS_TRANSLATE_TRICK_INTEG_TYPE_HH

// Trick includes
#include "sim_services/Integrator/include/Integrator.hh"

// Integration includes
#include "er7_utils/integration/core/include/integration_technique.hh"


namespace er7_utils {

/**
 * Translate a Trick Integrator_type to an er7_utils::Integration::Technique.
 * @param trick_integrator_type The Trick Integrator_type.
 * @return The corresponding Integration::Technique.
 */
inline Integration::Technique
translate_trick_integrator_type (
   Integrator_type trick_integrator_type)
{
   Integration::Technique result = Integration::Unspecified;

   switch (trick_integrator_type) {

   case Euler :
      result = Integration::Euler;
      break;

   case Euler_Cromer :
      result = Integration::SymplecticEuler;
      break;

   case Nystrom_Lear_2 :
      result = Integration::NystromLear2;
      break;

   case Runge_Kutta_2 :
      result = Integration::RK2Heun;
      break;

   case Modified_Midpoint_4 :
      result = Integration::ModifiedMidpoint4;
      break;

   case Runge_Kutta_4 :
      result = Integration::RungeKutta4;
      break;

   case Runge_Kutta_Gill_4 :
      result = Integration::RKGill4;
      break;

   case Runge_Kutta_Fehlberg_45 :
      result = Integration::RKFehlberg45;
      break;

   case Runge_Kutta_Fehlberg_78 :
      result = Integration::RKFehlberg78;
      break;

   case ABM_Method :
      result = Integration::AdamsBashforthMoulton4;
      break;

   case User_Defined :
      result = Integration::Unsupported;
      break;

   default :
      result = Integration::Invalid;
      break;

   }

   return result;
}

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

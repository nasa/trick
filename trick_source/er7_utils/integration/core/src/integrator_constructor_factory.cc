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
 * Defines member functions for the IntegratorConstructorFactory class.
 */

/*
Purpose: ()
*/

// System includes
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/alloc.hh"
#include "er7_utils/interface/include/message_handler.hh"

// Integration includes
#include "er7_utils/integration/abm4/include/abm4_integrator_constructor.hh"
#include "er7_utils/integration/beeman/include/beeman_integrator_constructor.hh"
#include "er7_utils/integration/euler/include/euler_integrator_constructor.hh"
#if 0
#include "er7_utils/integration/gauss_jackson/include/gauss_jackson_integrator_constructor.hh"
#endif
#include "er7_utils/integration/mm4/include/mm4_integrator_constructor.hh"
#include "er7_utils/integration/nl2/include/nl2_integrator_constructor.hh"
#include "er7_utils/integration/position_verlet/include/position_verlet_integrator_constructor.hh"
#include "er7_utils/integration/rk2_heun/include/rk2_heun_integrator_constructor.hh"
#include "er7_utils/integration/rk2_midpoint/include/rk2_midpoint_integrator_constructor.hh"
#include "er7_utils/integration/rk4/include/rk4_integrator_constructor.hh"
#include "er7_utils/integration/rkf45/include/rkf45_integrator_constructor.hh"
#include "er7_utils/integration/rkf78/include/rkf78_integrator_constructor.hh"
#include "er7_utils/integration/rkg4/include/rkg4_integrator_constructor.hh"
#if 0
#include "er7_utils/integration/rkn4/include/rkn4_integrator_constructor.hh"
#endif
#include "er7_utils/integration/symplectic_euler/include/symplectic_euler_integrator_constructor.hh"
#include "er7_utils/integration/velocity_verlet/include/velocity_verlet_integrator_constructor.hh"

// Local includes
#include "../include/integration_messages.hh"
#include "../include/integration_technique.hh"
#include "../include/integrator_constructor_factory.hh"


namespace er7_utils {


/**
 * Create an er7_utils IntegratorConstructor based on the input technique.
 * @param integ_type  Integration technique.
 * @return Created integrator constructor.
 */
IntegratorConstructor *
IntegratorConstructorFactory::create (
   Integration::Technique integ_type)
{
   IntegratorConstructor * constructor = NULL;

   switch (integ_type) {
   case Integration::Euler:
      constructor = EulerIntegratorConstructor::create_constructor();
      break;

   case Integration::SymplecticEuler:
      constructor = SymplecticEulerIntegratorConstructor::create_constructor();
      break;

   case Integration::Beeman:
      constructor = BeemanIntegratorConstructor::create_constructor();
      break;

   case Integration::NystromLear2:
      constructor = NystromLear2IntegratorConstructor::create_constructor();
      break;

   case Integration::PositionVerlet:
      constructor = PositionVerletIntegratorConstructor::create_constructor();
      break;

   case Integration::RK2Heun:
      constructor = RK2HeunIntegratorConstructor::create_constructor();
      break;

   case Integration::RK2Midpoint:
      constructor = RK2MidpointIntegratorConstructor::create_constructor();
      break;

   case Integration::VelocityVerlet:
      constructor = VelocityVerletIntegratorConstructor::create_constructor();
      break;

   case Integration::ModifiedMidpoint4:
      constructor = MM4IntegratorConstructor::create_constructor();
      break;

   case Integration::AdamsBashforthMoulton4:
      constructor = ABM4IntegratorConstructor::create_constructor();
      break;

   case Integration::RungeKutta4:
      constructor = RK4IntegratorConstructor::create_constructor();
      break;

   case Integration::RKGill4:
      constructor = RKGill4IntegratorConstructor::create_constructor();
      break;

#if 0
   case Integration::RKNystrom4:
      constructor = RKNystrom4IntegratorConstructor::create_constructor();
      break;
#endif

   case Integration::RKFehlberg45:
      constructor = RKFehlberg45IntegratorConstructor::create_constructor();
      break;

   case Integration::RKFehlberg78:
      constructor = RKFehlberg78IntegratorConstructor::create_constructor();
      break;

#if 0
   case Integration::GaussJackson:
      constructor = GaussJacksonIntegratorConstructor::create_constructor();
      break;
#endif

   // The remaining cases do not construct an integrator constructor.
   case Integration::Unspecified:
      MessageHandler::fail (__FILE__, __LINE__,
                            IntegrationMessages::invalid_request,
                            "Unspecified integration technique.");
      break;

   case Integration::Unsupported:
      MessageHandler::fail (__FILE__, __LINE__,
                            IntegrationMessages::invalid_request,
                            "Unsupported integration technique.");
      break;

   case Integration::Invalid:
   default:
      MessageHandler::fail (__FILE__, __LINE__,
                            IntegrationMessages::invalid_request,
                            "Invalid integration option %d\n.",
                            integ_type);
      break;
   }
   return constructor;
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

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
 * Defines the class IntegratorConstructorFactory, which creates an integrator
 * constructor based on a Trick integration structure.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_INTEGRATOR_CONSTRUCTOR_FACTORY_HH
#define ER7_UTILS_INTEGRATOR_CONSTRUCTOR_FACTORY_HH

// System includes

// ER7 utilities includes
#include "er7_utils/interface/include/er7_class.hh"

// Model includes
#include "integration_technique.hh"


namespace er7_utils {

class IntegratorConstructor;


/**
 * Factory method design pattern class for constructing an IntegratorConstructor
 * based on technique specified in a Trick integration structure.
 */
class IntegratorConstructorFactory {

   // This is a static class.
   // The default constructor, copy constructor, destructor, and assignment
   // operator for this class are private / unimplemented.

public:


   // Static member functions

   // create():
   // This static member function creates an IntegratorConstructor instance
   // based on the provided technique.
   static IntegratorConstructor * create (Integration::Technique type);


private:

   /**
    * Not implemented.
    */
   IntegratorConstructorFactory (void);

   /**
    * Not implemented.
    */
   IntegratorConstructorFactory (const IntegratorConstructorFactory &);

   /**
    * Not implemented.
    */
   ~IntegratorConstructorFactory (void);

   /**
    * Not implemented.
    */
   IntegratorConstructorFactory & operator= (
      const IntegratorConstructorFactory &);

};

}


#ifdef ER7_UTILS_NEED_AUX_INCLUDES
#include "integrator_constructor.hh"

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
#endif


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

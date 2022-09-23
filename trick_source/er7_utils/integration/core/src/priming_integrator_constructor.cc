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
 * Defines member functions for the PrimingIntegratorConstructor class.
 */

/*
Purpose: ()
*/

// System includes
#include <algorithm>
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Local includes
#include "../include/priming_integrator_constructor.hh"


namespace er7_utils {

// PrimingIntegratorConstructor default constructor
PrimingIntegratorConstructor::PrimingIntegratorConstructor (
   void)
:
   Er7UtilsDeletable (),
   IntegratorConstructor (),
   primer_constructor (NULL)
{
   // Empty
}


// PrimingIntegratorConstructor non-default constructor.
PrimingIntegratorConstructor::PrimingIntegratorConstructor (
   const IntegratorConstructor & primer)
:
   Er7UtilsDeletable (),
   IntegratorConstructor (),
   primer_constructor (primer.create_copy())
{
   // Empty
}


// PrimingIntegratorConstructor copy constructor.
PrimingIntegratorConstructor::PrimingIntegratorConstructor (
   const PrimingIntegratorConstructor & src)
:
   Er7UtilsDeletable (),
   IntegratorConstructor (),
   primer_constructor (src.primer_constructor->create_copy())
{
   // Empty
}


// PrimingIntegratorConstructor non-throwing swap.
void
PrimingIntegratorConstructor::swap (
   PrimingIntegratorConstructor & src)
{
   std::swap (primer_constructor, src.primer_constructor);
}


// PrimingIntegratorConstructor destructor.
PrimingIntegratorConstructor::~PrimingIntegratorConstructor (
   void)
{
   alloc::delete_object (primer_constructor);
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

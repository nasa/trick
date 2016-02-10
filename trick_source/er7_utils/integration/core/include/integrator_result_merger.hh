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
 * Defines the class IntegratorResultMerger, the base class for merging
 * an IntegratorResult from a state integrator into a merged IntegratorResult.
 */

/*
Purpose: ()
*/

#ifndef ER7_UTILS_INTEGRATOR_RESULT_MERGER_HH
#define ER7_UTILS_INTEGRATOR_RESULT_MERGER_HH


// Interface includes
#include "er7_utils/interface/include/config.hh"
#include "er7_utils/interface/include/deletable.hh"


// Forward declarations
namespace er7_utils {
class IntegratorResult;
}


namespace er7_utils {


/**
 * Class that merges a new integrator result into a combined result.
 * Note that this class is stateless; it contains no data members.
 *  Derived classes must also be stateless.
 */
class IntegratorResultMerger : public Er7UtilsDeletable {
public:


   // Note: The default constructor, copy constructor, and assignment operator
   // are not declared for this class.
   // The C++ implicitly-defined versions of these functions are in force.


   /**
    * IntegratorResultMerger destructor.
    */
   virtual ~IntegratorResultMerger() {}


   /**
    * Merge an IntegratorResult into another.
    * The default implementation pertains to simple, fixed step integration
    * techniques. Adaptive techniques or techniques that can have a failure
    * mode other than zero or one must override this default.
    * @return True if merger was successful, false if some error occurred.
    * @param[in]      new_result     Size of the generalized position vector
    * @param[in,out]  merged_result  Size of the generalized position vector
    */
   virtual bool merge_integrator_result (
      const IntegratorResult & new_result,
      IntegratorResult & merged_result) const;


   /**
    * Create a copy of 'this' IntegratorResultMerger object.
    * @return Clone of 'this'.
    */
   virtual IntegratorResultMerger * create_copy (void) const;
};


/**
 * Bogus IntegratorResultMerger.
 * The merge_integrator_result function croaks.
 */
class BogusIntegratorResultMerger : public IntegratorResultMerger {
public:


   // Note: The default constructor, copy constructor, and assignment operator
   // are not declared for this class.
   // The C++ implicitly-defined versions of these functions are in force.


   /**
    * BogusIntegratorResultMerger destructor.
    */
   virtual ~BogusIntegratorResultMerger() {}


   /**
    * Croak.
    */
   virtual bool merge_integrator_result (
      const IntegratorResult &, IntegratorResult &) const;


   /**
    * Create a copy of 'this' BogusIntegratorResultMerger object.
    * @return Clone of 'this'.
    */
   virtual IntegratorResultMerger * create_copy (void) const;
};


}

#endif

/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

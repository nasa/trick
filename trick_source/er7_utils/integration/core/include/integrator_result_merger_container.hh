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
 * Defines the class IntegratorResultMergerContainer, which encapsulates an
 * IntegratorResultMerger using the RAII idiom.
 */

/*
Purpose: ()
*/

#ifndef ER7_UTILS_INTEGRATOR_RESULT_MERGER_CONTAINER_HH
#define ER7_UTILS_INTEGRATOR_RESULT_MERGER_CONTAINER_HH


// Local includes
#include "integrator_result_merger.hh"

// Interface includes
#include "er7_utils/interface/include/config.hh"
#include "er7_utils/interface/include/er7_class.hh"

// Forward declarations
namespace er7_utils {
class IntegratorConstructor;
class IntegratorResult;
}


namespace er7_utils {

/**
 * A IntegratorResultMergerContainer is an RAII class that encapsulates an
 * IntegratorResultMerger object so that other objects need not worry about
 * memory management for an IntegratorResultMerger object.
 */
class IntegratorResultMergerContainer {

ER7_UTILS_MAKE_SIM_INTERFACES(IntegratorResultMergerContainer)

public:

   /**
    * IntegratorResultMergerContainer default constructor.
    */
   IntegratorResultMergerContainer ();

   /**
    * IntegratorResultMergerContainer non-default constructor.
    * @param[in] integ_cotr  Integrator constructor
    */
   IntegratorResultMergerContainer (
      const IntegratorConstructor & integ_cotr);


   /**
    * IntegratorResultMergerContainer copy constructor.
    * @param[in] src  Object to be copied.
    */
   IntegratorResultMergerContainer (
      const IntegratorResultMergerContainer & src);


   /**
    * IntegratorResultMergerContainer destructor.
    */
   ~IntegratorResultMergerContainer ();


   /**
    * Swap.
    */
   friend void swap (
      IntegratorResultMergerContainer & a, IntegratorResultMergerContainer & b);


   /**
    * IntegratorResultMergerContainer assignment operator.
    * @param src  Object to be copied.
    */
   IntegratorResultMergerContainer & operator= (
      IntegratorResultMergerContainer src)
   {
      swap (*this, src);
      return *this;
   }


   /**
    * Configure the IntegratorResultMergerContainer for use with a specific
    * integration technique.
    * @param integ_cotr  The integrator constructor for the technique.
    */
   void configure (
      const IntegratorConstructor & integ_cotr);


   /**
    * Merge an IntegratorResult into another.
    * The default implementation pertains to simple, fixed step integration
    * techniques. Adaptive techniques or techniques that can have a failure
    * mode other than zero or one must override this default.
    * @return True if merger was successful, false if some error occurred.
    * @param[in]      new_result     Size of the generalized position vector
    * @param[in,out]  merged_result  Size of the generalized position vector
    */
   bool merge_integrator_result (
      const IntegratorResult & new_result,
      IntegratorResult & merged_result)
   const ER7_UTILS_ALWAYS_INLINE
   {
      return integ_merger->merge_integrator_result (new_result, merged_result);
   }


private:

   /**
    * The object that merges results from multiple integrators.
    */
   er7_utils::IntegratorResultMerger * integ_merger; //!< trick_units(--)

};

}


#endif

/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

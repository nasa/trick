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
 * Defines the class IntegratorResult, which encapsulates the return value
 * from an integrator.
 */

/*
Purpose: ()
*/

#ifndef ER7_UTILS_INTEGRATOR_RESULT_HH
#define ER7_UTILS_INTEGRATOR_RESULT_HH


// Interface includes
#include "er7_utils/interface/include/config.hh"
#include "er7_utils/interface/include/er7_class.hh"


namespace er7_utils {


/**
 * Class that encapsulates the return value from an integrator and the results
 * of merging results from multiple integrators.
 *
 * A state integrator returns two values, an indication of whether the
 * integration attained the target stage, and if successful, the fractional
 * amount of the integration time step by which time should be advanced.
 * An IntegratorResult combines these two return values in a single object.
 *
 * An IntegratorResult contains one additional field, the number of results
 * from multiple state integrators that have been merged to form a merged
 * IntegratorResult result. Some state integrators may disagree on whether
 * the target stage has been successfully achieved or on how time should be
 * advanced. A technique-specific IntegratorResultMerger object is responsible
 * for merging results from multiple state integrators.
 *
 * @note This class has a non-virtual destructor. It is a final class.
 */
class IntegratorResult {

ER7_UTILS_MAKE_SIM_INTERFACES(IntegratorResult)

public:


   // Note: The destructor, copy constructor, and assignment operator are not
   // declared for this class.
   // The C++ implicitly-defined versions of these functions are in force.


   /**
    * IntegratorResult default constructor.
    * The two time scales are initialized to 1.0 as these are the canonical
    * values. The failure mode is initialized to 0 to indicate success.
    * The merge count is initialized to 0 or 1 depending on the value of
    * the provided argument. The default value results in a merge_count of 1.
    *
    * @param is_integrated Indicates how the merge_count is to be initialized.
    *    Integration controls should explicitly use false to create an
    *    empty IntegratorResult. The default setting is for IntegratorResult
    *    created just before calling an integrator.
    */
   IntegratorResult (bool is_integrated=true) ER7_UTILS_ALWAYS_INLINE
   :
      time_scale(1.0),
      failure_mode(0),
      merge_count(is_integrated ? 1 : 0)
   { }


   /**
    * IntegratorResult non-default constructor, intended for use by
    * simple integrators that always pass.
    * The failure_mode and merge_count members are set to default values.
    * The time_scale member is set to the provided value.
    *
    * @param step_factor Indicates how the time_scale is to be initialized.
    */
   IntegratorResult (double step_factor) ER7_UTILS_ALWAYS_INLINE
   :
      time_scale(step_factor),
      failure_mode(0),
      merge_count(1)
   { }


   // Getters and setters.

   /**
    * Get the IntegratorResult's time_scale data member.
    */
   double get_time_scale () const ER7_UTILS_ALWAYS_INLINE
   { return time_scale; }

   /**
    * Determine whether the integration attained the target stage.
    */
   bool get_passed () const ER7_UTILS_ALWAYS_INLINE
   { return failure_mode == 0; }

   /**
    * Get the IntegratorResult's failure_mode data member.
    */
   int get_failure_mode () const ER7_UTILS_ALWAYS_INLINE
   { return failure_mode; }

   /**
    * Get the IntegratorResult's merge_count data member.
    */
   int get_merge_count () const ER7_UTILS_ALWAYS_INLINE
   { return merge_count; }


   /**
    * Set the IntegratorResult's time_scale to the specified value.
    */
   void set_time_scale (double value) ER7_UTILS_ALWAYS_INLINE
   { time_scale = value; }

   /**
    * Denote that the integration has attained the target stage.
    */
   void set_passed () ER7_UTILS_ALWAYS_INLINE
   { failure_mode = 0; }

   /**
    * Denote that the integration has failed to attain the target stage.
    */
   void set_failed () ER7_UTILS_ALWAYS_INLINE
   { failure_mode = 1; }

   /**
    * Set the IntegratorResult's failure_mode to the specified value.
    */
   void set_failure_mode (int value) ER7_UTILS_ALWAYS_INLINE
   { failure_mode = value; }

   /**
    * Mark an IntegratorResult as containing a true result.
    * This function should be used when
    *  - an IntegratorResult was created as an empty object for use by an
    *    individual integrator, and
    *  - That integration was performed.
    */
   void mark_as_integrated () ER7_UTILS_ALWAYS_INLINE
   { merge_count = 1; }

   /**
    * Mark an IntegratorResult as containing a null result.
    * This function should be used when
    *  - an IntegratorResult was created as a default object for use by an
    *    individual integrator, and
    *  - That integration was not performed.
    */
   void mark_as_not_integrated () ER7_UTILS_ALWAYS_INLINE
   { merge_count = 0; }

   /**
    * Increment the IntegratorResult's merge_count by the specified value.
    */
   void increment_merge_count (int value = 1) ER7_UTILS_ALWAYS_INLINE
   { merge_count += value; }

private:

   // Member data.

   double time_scale; /**< trick_units(--) @n
      Fraction of the integration interval time step by which the time should be
      advanced. A value of 0.0 means time should be set to the start of the
      integration interval; a value of 1.0 means time should be advanced to
      the end of the interval. */

   int failure_mode; /**< trick_units(--) @n
      Zero indicates the integrator successfully reached the target stage.
      A non-zero value indicates some technique-dependent failure. */

   int merge_count; /**< trick_units(--) @n
      The number of times another IntegratorResult has been merged into this
      IntegratorResult. A value of zero means this is a virgin result. */
};


}

#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

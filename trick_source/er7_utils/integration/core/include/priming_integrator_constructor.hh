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
 * Defines the class PrimingIntegratorConstructor, which integrators for a
 * technique that needs to be primed.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_PRIMING_INTEGRATOR_CONSTRUCTOR_HH
#define ER7_UTILS_PRIMING_INTEGRATOR_CONSTRUCTOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Model includes
#include "integrator_constructor.hh"


namespace er7_utils {

/**
 * Create state and time integrators for techniques that need priming.@n
 * Note well:@n
 * The IntegratorConstructor used to prime the PrimingIntegratorConstructor
 * must not be a PrimingIntegratorConstructor thanks to the nature of the
 * non-default and copy constructors.
 */
class PrimingIntegratorConstructor : public IntegratorConstructor {
ER7_UTILS_MAKE_SIM_INTERFACES(PrimingIntegratorConstructor)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * PrimingIntegratorConstructor destructor.
    */
   virtual ~PrimingIntegratorConstructor (void);


   /**
    * Getter for the primer_constructor.@n
    * Note that there is no setter for this member.
    * @return Const reference to the primer_constructor.
    */
   const IntegratorConstructor & get_primer_constructor (void) const
   {
      return *primer_constructor;
   }

   /**
    * Indicate whether the integration technique will always takes a
    * pre-determined number of integration steps per integration cycle.@n
    * This implemenetation assumes the primary technique is a fixed step
    * integrator, but that the primer might not be.
    * @return True if the primer is also fixed step, false otherwise.
    */
   virtual bool is_fixed_step_integrator (void) const
   { return primer_constructor->is_fixed_step_integrator(); }

   /**
    * Return the buffer size needed by engines such as Trick that need to
    * allocate space separately from the integration technique.@n
    * This implementation returns the larger of the primer's buffer size and
    * the primary technique's transition table size.
    * @return Largest number of steps in any integration cycle
    */
   virtual unsigned int get_buffer_size (void) const
   {
      unsigned int primer_size  = primer_constructor->get_buffer_size();
      unsigned int this_table_size = get_transition_table_size();
      return (primer_size > this_table_size) ? primer_size : this_table_size;
   }


   /**
    * Get the size of the history buffer needed by the technique.
    * @return Required history buffer length.
    */
   virtual unsigned int get_history_length (void) const = 0;


protected:

   // Member functions.

   /**
    * PrimingIntegratorConstructor default constructor.
    */
   PrimingIntegratorConstructor (void);

   /**
    * PrimingIntegratorConstructor copy constructor.
    * @param[in] src  Object to be copied.
    */
   PrimingIntegratorConstructor (const PrimingIntegratorConstructor & src);

   /**
    * PrimingIntegratorConstructor non-default constructor.
    * @param[in] primer  Integrator constructor that creates the primers.
    */
   explicit PrimingIntegratorConstructor (const IntegratorConstructor & primer);

   /**
    * Non-throwing swap.
    * @param[in,out] src  Object with which contents are to be swapped.
    */
   virtual void swap (PrimingIntegratorConstructor & src);


   // Member data.

   IntegratorConstructor * primer_constructor; /**< trick_units(--) @n
      Constructor used to create the primers. */


private:

   /**
    * Not implemented.
    */
   PrimingIntegratorConstructor & operator= (
      const PrimingIntegratorConstructor &);

};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

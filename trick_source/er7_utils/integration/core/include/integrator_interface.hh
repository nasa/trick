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
 * Defines a generic interface for accessing / updating elements of a simulation
 * integrator object.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_INTEGRATOR_INTERFACE_HH
#define ER7_UTILS_INTEGRATOR_INTERFACE_HH

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"


namespace er7_utils {


/**
 * The simulation engine has some kind of mechanism for guiding the integration
 * process. The IntegratorInterface class represents in a generic sense the
 * interface to those mechanism.
 */
class IntegratorInterface {
ER7_UTILS_MAKE_SIM_INTERFACES(IntegratorInterface)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * Destructor.
    */
   virtual ~IntegratorInterface () {}


   // Getters and setters

   /**
    * Get the integration cycle time step.
    * @return                Simulation time delta t@n
    * Units: s
    */
   virtual double get_dt () const = 0;

   /**
    * Get the flag that tells the simulation engine to compute derivatives
    * on the initial step of each integration cycle.
    * @return Value of the first step derivatives flag
    */
   virtual bool get_first_step_derivs_flag () const = 0;

   /**
    * Set the flag that tells the simulation engine to compute derivatives
    * on the initial step of each integration cycle.
    * @param[in] value       Value of the first step derivatives flag
    */
   virtual void set_first_step_derivs_flag (bool value) = 0;

   /**
    * Reset the flag that tells the simulation engine to compute derivatives
    * on the initial step of each integration cycle. Derivatives are always
    * needed just after a reset.
    */
   virtual void reset_first_step_derivs_flag () = 0;

   /**
    * Restore the flag that tells the simulation engine to compute derivatives
    * on the initial step of each integration cycle to it's value prior to
    * the most recent call to reset_first_step_derivs_flag.
    */
   virtual void restore_first_step_derivs_flag () = 0;

   /**
    * Set the step number within an integration cycle.
    * @param[in] stepno      Step number
    */
   virtual void set_step_number (unsigned int stepno) = 0;

   /**
    * Set the simulation time for the start of the next integration cycle.
    * @param[in] time        Simulation time, in simulation time seconds
    */
   virtual void set_time (double time) = 0;


protected:

   /**
    * IntegratorInterface default constructor.
    */
   IntegratorInterface () {}

   /**
    * IntegratorInterface copy constructor.
    */
   IntegratorInterface (const IntegratorInterface &) {}


private:

   /**
    * Not implemented.
    */
   IntegratorInterface & operator = (const IntegratorInterface &);
};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

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
 * Defines the class TimeInterface.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_TIME_INTERFACE_HH
#define ER7_UTILS_TIME_INTERFACE_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"


namespace er7_utils {


/**
 * At the most abstract level, a time interface is something that updates an
 * internal representation of time based on some simulation time.
 */
class TimeInterface {
ER7_UTILS_MAKE_SIM_INTERFACES(TimeInterface)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * Destructor.
    */
   virtual ~TimeInterface () {}

   /**
    * Update the time model given the simulation time.
    * \param[in] sim_time Simulation time
    */
   virtual void update_time (double sim_time) = 0;

   /**
    * Get the current scale factor from simulation time
    * to dynamic time.
    * \return Simulation time to dynamic time scale factor
    */
   virtual double get_time_scale_factor () const = 0;


protected:

   /**
    * TimeInterface default constructor.
    */
   TimeInterface () {}

   /**
    * TimeInterface copy constructor.
    */
   TimeInterface (const TimeInterface &) {}


private:

   /**
    * Not implemented.
    */
   TimeInterface & operator = (const TimeInterface &);

};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

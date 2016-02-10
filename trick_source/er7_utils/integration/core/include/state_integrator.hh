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
 * Defines the class StateIntegratorInterface, the base class for propagating
 * state.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_STATE_INTEGRATOR_HH
#define ER7_UTILS_STATE_INTEGRATOR_HH

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"
#include "er7_utils/interface/include/deletable.hh"


namespace er7_utils {

/**
 * The base class for classes that propagate state.
 * A state integrator class propagates state and indicates upon return
 * whether it successfully reached the target and indicates the fractional
 * amount by which it assumed time is to be advanced.
 * Note that this interface class does not declare an integrate method.
 */
class StateIntegratorInterface : virtual public Er7UtilsDeletable {


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:


   /**
    * Destructor.
    */
   virtual ~StateIntegratorInterface (void) {}

   /**
    * Called to tell an integrator to reset itself when the historical state has
    * become invalid because of a change the time step or time direction, or
    * because of some discrete change in state such as a big thruster firing
    * or docking/undocking.
    *
    * The default implementation, which is to do nothing, is valid for single
    * step integrators that do not maintain a history. Multistep integrators
    * such as Adams-Bashforth-Moulton and Gauss-Jackson must override this
    * default.
    */
   virtual void reset_integrator (void) {}

   /**
    * Every integrator needs to be able to create a copy of itself.
    * Note: This must be defined in this class until C++11.
    * This class needs a pure virtual to ensure is_abstract<> true.
    * @return Clone of 'this'.
    */
   virtual StateIntegratorInterface* create_copy () const = 0;


protected:

   /**
    * Default constructor.
    */
   StateIntegratorInterface (void)
   : Er7UtilsDeletable()
   { }

   /**
    * Copy constructor.
    */
   StateIntegratorInterface (const StateIntegratorInterface &)
   : Er7UtilsDeletable()
   { }


private:

   /**
    * Not implemented.
    */
   StateIntegratorInterface & operator= (const StateIntegratorInterface &);

};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

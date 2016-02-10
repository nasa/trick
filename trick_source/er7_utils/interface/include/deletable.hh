/**
 * @if Er7UtilsUseGroups
 * @addtogroup Er7Utils
 * @{
 * @addtogroup Interface
 * @{
 * @endif
 */

/**
 * @file
 * Defines the class Er7UtilsDeletable, which provides a simple mechanism for
 * deleting er7_utils objects that were created with er7_utils allocation.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_DELETABLE_HH
#define ER7_UTILS_DELETABLE_HH

// System includes
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"



namespace er7_utils {

/**
 * The base class for er7_utils/integration classes.
 * An object pointer 'foo' that points to an instance of a class that derives
 * from this class can be deleted via
 * @code
 * Er7UtilsDeletable::delete_instance (foo);
 * @endcode
 * The deletion will use er7_utils memory allocation, which means that the
 * object must have been created with er7_utils memory allocation.
 */
class Er7UtilsDeletable {

public:

   /**
    * Delete an instance of a class that derives from Er7UtilsDeletable.
    *
    * ## Assumptions and limitations:
    *  - The pointer can be null. Deleting a null pointer is no harm / no foul.
    *  - The object must have been allocated using er7_utils memory allocation.
    *
    * @tparam T           The type of object to be deleted; this must be a
    *                     derived class of Er7UtilsDeletable.
    * @param[in,out] obj  Pointer to the object to be deleted. The pointer
    *                     is set to null after deleting.
    */
   template<typename T>
   static void
   delete_instance (
      T *& obj)
   {
      Er7UtilsDeletable::delete_instance_internal (obj);
      obj = NULL;
   }


   // Note: The default constructor, copy constructor, and assignment operator
   // for this class are not declared. The C++ defaults are in force.

   /**
    * Destructor.
    */
   virtual ~Er7UtilsDeletable (void) {}


private:

   // Delete an instance.
   static void delete_instance_internal (Er7UtilsDeletable* instance);
};


}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

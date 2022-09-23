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
 * Configures the ER7 utilities for use by some simulation engine and
 * define the ER7_UTILS class declaration macros ER7_UTILS_MAKE_SIM_INTERFACES.
 *
 * In a Trick environment, this macros gives the Trick input processor,
 * the Trick checkpoint / checkpoint-restart facility, and the ICG-generated
 * io_src file for the header full visibility of the class's contents.
 * The intent is to provide the same capability outside the Trick.
 *
 * Every ER7 utilities header file that defines a class that contains
 * protected or private data members must include this file and must
 * use the ER7_UTILS_MAKE_SIM_INTERFACES macro in each such class.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_CLASS_HH
#define ER7_UTILS_CLASS_HH

// ER7_UTILS includes
#include "config.hh"


// Some installations may not be able to fit within the confines of the main
// body of this header. Include the alternate implementation if defined.
// The alternate implementation must define the macros
// ER7_UTILS_MAKE_SIM_INTERFACES and ER7_UTILS_DECLARE_SIM_INTERFACES.
#ifdef ER7_UTILS_CLASS_IMPLEMENTATION_HEADER
#include ER7_UTILS_CLASS_IMPLEMENTATION_HEADER

// The main body of this header is within the body of this #else.
#else

// ER7_UTILS_MAKE_SIM_INTERFACES(class_name)
// Three alternate definition mechanisms are provided below:
// - As an alias for JEOD_CLASS_ESTABLISH_FRIENDS if defined,
// - As a wrapper around JEOD_CLASS_FRIEND_CLASS if defined,
// - A null implementation if neither of the above is defined.

/**
 * ER7_UTILS_MAKE_SIM_INTERFACES(class_name)
 * Defines friends of the given class. This macro is to be invoked in the body
 * of all JEOD classes. The intent is to make all parts of the class visible to
 * the designated simulation engine classes and functions.
 *
 * @param class_name Name of the class being defined.
 */
#ifndef ER7_UTILS_MAKE_SIM_INTERFACES

#if (defined ER7_UTILS_CLASS_ESTABLISH_FRIENDS)
#define ER7_UTILS_MAKE_SIM_INTERFACES(class_name) \
   ER7_UTILS_CLASS_ESTABLISH_FRIENDS(class_name)

#elif defined(ER7_UTILS_CLASS_FRIEND_CLASS)
#define ER7_UTILS_MAKE_SIM_INTERFACES(class_name) \
   friend class ER7_UTILS_CLASS_FRIEND_CLASS;

#else
#define ER7_UTILS_MAKE_SIM_INTERFACES(class_name)
#endif

#endif  // ER7_UTILS_MAKE_SIM_INTERFACES


#endif  // ER7_UTILS_CLASS_IMPLEMENTATION_HEADER

#endif  // One-time include
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

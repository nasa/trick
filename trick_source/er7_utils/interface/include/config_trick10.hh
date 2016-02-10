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
 * Configures the ER7 utilities for use in a Trick10 / Trick13 environment.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_CONFIG_TRICK10_HH
#define ER7_UTILS_CONFIG_TRICK10_HH


/**
 * @def ER7_UTILS_CLASS_ESTABLISH_FRIENDS
 * Make protected and private data visible to the simulation engine.
 * @param class_name Name of class being defined.
 */
#ifndef ER7_UTILS_CLASS_IMPLEMENTATION_HEADER

#ifndef SWIG

#define ER7_UTILS_CLASS_ESTABLISH_FRIENDS(class_name) \
   friend class InputProcessor; \
   friend void init_attrer7_utils__ ## class_name();

#else
#define ER7_UTILS_CLASS_ESTABLISH_FRIENDS(class_name)
#endif

#endif


/**
 * @def ER7_UTILS_NEED_AUX_INCLUDES
 * If defined, indicates that header files that nominally are not needed
 * do need to be included for the sake of the simulation engine.
 * Header files that define classes that contain pointer data members should
 * include the headers that define the types of those members if this
 * preprocessor symbol is defined.
 * This symbol is defined in a Trick context.
 */
#ifndef ER7_UTILS_NEED_AUX_INCLUDES
#define ER7_UTILS_NEED_AUX_INCLUDES
#endif


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

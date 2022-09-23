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
 * Configures the ER7 utilities for use by some simulation engine.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_CONFIG_HH
#define ER7_UTILS_CONFIG_HH

// For Trick-based applications,
// Use the version-specific configuration for Trick-based builds.
#if (defined TRICK_VER)

#if (TRICK_VER >= 10)
#include "config_trick10.hh"

#else
#error "Invalid Trick version"
#endif

// Standalone unit tests use the test harness configuration.
#elif (defined ER7_UTILS_UNIT_TEST)
#include "config_test_harness.hh"

// FUTURE, if needed: configure for the non-Trick demo capability.


// Non-Trick installations should consolidate requisite configuration
// information in a single header file and compile JEOD with
// JEOD_CONFIG_HEADER defined to be this config file.
#elif (defined ER7_UTILS_CONFIG_HEADER)
#include ER7_UTILS_CONFIG_HEADER

#else
// We're out of options. Compilation will be hosed.
#endif


// Define macros that enable the package to take advantage of performance
// enhancing extensions to c++, if possible.

#if (! defined SWIG) && \
    ((defined __GNUC__) || (defined __llvm__) || (defined __clang__))

/**
 * @def ER7_UTILS_RESTRICT
 * Specifying an argument as restricted means that it is not aliased
 * to any other arguments. Functions with restricted arguments must
 * not be called with arguments that alias one another. That arguments
 * are not aliased enables significant optimizations.
 */
#define ER7_UTILS_RESTRICT      __restrict__

/**
 * @def ER7_UTILS_ALWAYS_INLINE
 * Functions qualified as always inline are inlined even when
 * compiled unoptimized.
 */
#define ER7_UTILS_ALWAYS_INLINE __attribute__((always_inline))

/**
 * @def ER7_UTILS_UNUSED
 * Arguments that aren't used should either have no name or
 * be marked as ER7_UTILS_UNUSED.
 */
#define ER7_UTILS_UNUSED        __attribute__((unused))


#else
#define ER7_UTILS_RESTRICT
#define ER7_UTILS_ALWAYS_INLINE
#define ER7_UTILS_UNUSED
#endif


/**
 * @def ER7_UTILS_HAVE_ABI
 * Defined if the header file <cxxabi.h> is available.
 */
#if (defined __GNUC__)
#define ER7_UTILS_HAVE_ABI
#elif (defined __has_include)
#if __has_include(<cxxabi.h>)
#define ER7_UTILS_HAVE_ABI
#endif
#endif


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

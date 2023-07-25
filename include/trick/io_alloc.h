#ifndef IO_ALLOC_H
#define IO_ALLOC_H
/*
    PURPOSE: (The ALLOC_INFO structure describes a chunk of
              memory that the MemoryManager knows about.)
*/
#include "trick/attributes.h"
#include "trick/parameter_types.h"

/**
 * enum TRICK_STCL - indicates whether or not the MemoryManager allocated the object.
 * -------------------------------------------------------------------------------------------
 * TRICK_LOCAL  - The memory manager allocated the object, via a form of declare_var..().
 * TRICK_EXTERN - The memory manager did not allocate the object, but was given a pointer to
 *                an object via a form of declare_extern_var..().
 * -------------------------------------------------------------------------------------------
 */
typedef enum {
   TRICK_LOCAL  = 0,
   TRICK_EXTERN = 1,
} TRICK_STCL;


/**
 * enum TRICK_ALLOC_TYPE - indicates how an object that known to the MemoryManager was allocated.
 * -------------------------------------------------------------------------------------------
 * TRICK_ALLOC_MALLOC - indicates that the memory object described by ALLOC_INFO was allocated
 *                      by calloc() or malloc().
 * TRICK_ALLOC_IOSRC  - indicates that the memory object was allocated by calling an 
 *                      io_src_allocate_<CLASS_NAME>() function that was generated
 *                      for that objects data-type by ICG. 
 * TRICK_ALLOC_OTHER  - indicates that the ALLOC_INFO record doesn't "know" how the the object
 *                      was allocated. This is the case when its storage class is TRICK_EXTERN.
 * -------------------------------------------------------------------------------------------
 */
typedef enum {
   TRICK_ALLOC_MALLOC = 0,
   TRICK_ALLOC_IOSRC  = 1,
   TRICK_ALLOC_OTHER  = 2,
} TRICK_ALLOC_TYPE;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * The ALLOC_INFO structure describes a chunk of memory that the MemoryManager knows about.
 */
typedef struct {

    void       *start;            /**< ** Address where this allocation starts */
    void       *end;              /**< ** Address where this allocation ends */
    char       *name;             /**< ** Name of parameter */
    int        size;              /**< ** Size of element */
    int        sentinel_bytes;    /**< ** Number of sentinel bytes.  Sentinel bytes are extra bytes requested by operator new[] */
    Language   language;          /**< ** Language */
    TRICK_TYPE type;              /**< ** Type specifier */
    TRICK_STCL stcl;              /**< ** Storage class */
    TRICK_ALLOC_TYPE alloc_type;  /**< ** Allocation type for local variables */
    char       *user_type_name;   /**< ** name of class/struct or enumeration type */
    ATTRIBUTES *attr;             /**< ** class/struct or enumeration attributes */
    int        num;               /**< ** Total number of elements. Is the product of all the fixed dimension sizes.*/
    int num_index;                /**< ** number of dimensions. */
    int index[TRICK_MAX_INDEX];   /**< ** dimension sizes. */

    unsigned int id ;             /**< ** id number assigned when added to ALLOC_INFO_MAP */
    int alloced_in_memory_init ;  /**< ** boolean indicating memory was set in memory_init in S_source.cpp */
} ALLOC_INFO;

#ifdef __cplusplus
}
#endif
#endif

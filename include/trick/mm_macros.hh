
#ifndef MM_MACORS_HH
#define MM_MACORS_HH

#include <stdlib.h>
#include <new>
#include "trick/memorymanager_c_intf.h"
#include "trick/MemoryManager.hh"

#ifdef SWIG
#define TRICK_MM_FRIENDS(class_name_with_underscores)
#define TRICK_MM_INTERFACE(class_name, class_name_with_underscores)
#else
#define TRICK_MM_FRIENDS(class_name_with_underscores) \
    friend class InputProcessor; \
    friend void init_attr ## class_name_with_underscores();

// The delete operators test to see if the pointer exists in the memory manager before calling
// TMM_delete_var_a.  This was done to avoid this recursive call sequence
// TMM_delete_var_a -> io_src_delete_class -> io_src_delete_<class_name> -> delete -> TMM_delete_var_a
#define TRICK_MM_INTERFACE(class_name, class_name_with_underscores) \
    friend class InputProcessor; \
    friend void init_attr ## class_name_with_underscores(); \
    public: \
        static void* operator new (size_t size, const char * alloc_name = "" ) { \
            void * new_ptr = calloc( 1, size ) ; \
            TMM_declare_ext_var(new_ptr, TRICK_STRUCTURED,#class_name, 0, alloc_name, 0, NULL) ; \
            return new_ptr ; \
        } ; \
        static void* operator new[] (size_t size, const char * alloc_name = "" ) { \
            void * new_ptr = calloc( 1, size ) ; \
            TMM_declare_ext_var(new_ptr, TRICK_STRUCTURED,#class_name, 0, alloc_name , 0, NULL) ; \
            return new_ptr ; \
        } ; \
        static void* operator new (size_t size __attribute__((unused)), class_name * addr, const char * alloc_name = "") { \
            TMM_declare_ext_var(addr, TRICK_STRUCTURED,#class_name, 0, alloc_name, 0, NULL) ; \
            return addr ; \
        } ; \
        static void operator delete (void *p) { \
            if ( get_alloc_info_at(p) ) { trick_MM->delete_var(p, false) ; free(p) ; } \
        } ; \
        static void operator delete[] (void *p) { \
            if ( get_alloc_info_at(p) ) { trick_MM->delete_var(p, false) ; free(p) ; } \
        } ;
#endif

#endif

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
 * Defines memory management macros and template functions.
 *
 * This file comprises multiple parts:
 *  - Declaration of a name demangler function, if available.
 *  - Low level, Trick-specific memory management template functions.
 *  - Implementation-specific definitions of set of memory management macros.
 *    While these macros are defined in implementation-specific manner,
 *    the arguments and products are implementation independent.
 *  - High-level memory management template functions that are implemented
 *    in terms of those four macros. These templates obviate the need
 *    for using those macros outside of this file.
 *    The ER7 utilities code should use the high level interfaces rather
 *    than the memory management macros.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_ALLOC_HH
#define ER7_UTILS_ALLOC_HH

// SWIG gets utterly confused by this.
#ifndef SWIG

// Interface includes
#include "config.hh"

// System includes
#include <cstddef>
#include <cstring>
#include <string>
#include <new>

#ifdef ER7_UTILS_HAVE_ABI
#include <typeinfo>
#endif


// Declare er7_utils::alloc.
namespace er7_utils {

   /**
    * Contains free functions that pertain to initialization (setting to NULL),
    * allocating memory, and releasing allocated memory.
    */
   namespace alloc {
   }
}


/******************************************************************************/

// The macros below need a demangle function, if available.


#ifdef ER7_UTILS_HAVE_ABI
namespace er7_utils {
   namespace alloc {

      /**
       * Demangle a type ID.
       * @param info The type ID whose name is to be demangled.
       * @return Demangled type name.
       */
      const std::string demangle (const std::type_info & info);
   }
}
#endif


/******************************************************************************/

// Part one, allocation/deallocation macros.

/*
 * Trick-based implementations of the four macros.
 */
#ifdef TRICK_VER

#include <new>
#include <string>
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"
#include "type_traits.hh"


namespace er7_utils {

   /**
    * Contains helper functions for the Trick-specific implementations
    * of the ER7 utilities allocation macros.
    */
   namespace trick_interface {

#ifdef ER7_UTILS_HAVE_ABI
      /**
       * Get the name of a type as a string.
       * @tparam T Type whose name is to be retrieved.
       * @return Demangled type name.
       */
      template<typename T>
      inline const std::string type_name (std::string) {
         return alloc::demangle (typeid(T));
      }
#else
      /**
       * Try to get the name of a type as a string.
       * @tparam T Type whose name is to be retrieved.
       * @return Type name.
       */
      template<typename T>
      inline const std::string type_name (std::string name_of_type) {
         if (name_of_type.find("::") == 0) {
            name_of_type.erase (0, 2);
         }
         else if ((name_of_type.find("Trick::") != 0) &&
                  (name_of_type.find("er7_utils::") != 0)) {
            name_of_type.insert (0, "er7_utils::");
         }
         return name_of_type;
      }
#endif

      /**
       * Allocate an array of objects of the specified type.
       * The returned array contains default-constructed objects.
       * @tparam T Type of objects to be allocated.
       * @param name_of_type Demangled name of the type.
       * @param count        Number of elements to be allocated.
       * @return Array of allocated objects.
       */
      template <typename T>
      inline T * trick_alloc (
         std::string name_of_type,
         unsigned int count)
      {
         name_of_type = type_name<T> (name_of_type);
         T * ptr = reinterpret_cast<T*> (
                      TMM_declare_var_1d (name_of_type.c_str(), count));
         if (!ptr) {
            throw std::bad_alloc();
         }
         return ptr;
      }

      /**
       * Allocate an object of the specified type.
       * The returned object is not constructed.
       * @tparam T Type of the object to be allocated.
       * @param name_of_type Demangled name of the type.
       * @return Array of allocated objects.
       */
      template <typename T>
      inline T * alloc_object (std::string name_of_type) {
         T * ptr = trick_alloc<T> (name_of_type, 1);
         ptr->~T();
         return ptr;
      }

      /**
       * Destruct and deallocate a previously allocated object.
       * @tparam T Type of the object to be deallocated.
       * @param obj Object to be destructed, as a reference to a pointer.
       */
      template <typename T>
      inline void delete_object (T*& obj) {
         if (obj) {
            TMM_delete_var_a (type_traits::get_allocated_pointer (obj));
            obj = 0;
         }
      }
   }
}

#endif


/******************************************************************************/

// Part two, allocation/deallocation macros.

/*
 * Trick-based implementations of the four allocation/deallocation macros.
 * These use the er7_utils::trick_interface template functions defined above.
 */
#ifdef TRICK_VER

/**
 * Allocate an array of some primitive type.
 * @param size Size of the array to be allocated.
 * @param type Type of the array.
 */
#define ER7_UTILS_ALLOC_PRIM_ARRAY(size,type) \
   er7_utils::trick_interface::trick_alloc<type> (#type, size)

/**
 * Allocate an object of some class type.
 * @param type Type of the object.
 * @param cotr Constructor arguments, in parentheses.
 */
#define ER7_UTILS_ALLOC_CLASS_OBJECT(type,cotr) \
   new (er7_utils::trick_interface::alloc_object<type> (#type)) type cotr

/**
 * Delete a previously allocated array.
 * @param arr Array to be deleted.
 */
#define ER7_UTILS_DELETE_ARRAY(arr) \
   er7_utils::trick_interface::delete_object (arr)

/**
 * Delete a previously allocated instance of a class.
 * @param obj Object to be deleted.
 */
#define ER7_UTILS_DELETE_OBJECT(obj) \
   er7_utils::trick_interface::delete_object (obj)


/*
 * JEOD-based implementations of the four macros.
 * Note that the JEOD implementations come into play only in the case
 * of a non-Trick implementation.
 */
#elif (defined JEOD)

#include "utils/memory/include/jeod_alloc.hh"

#define ER7_UTILS_ALLOC_PRIM_ARRAY(size,type) \
   JEOD_ALLOC_PRIM_ARRAY (size, type)

#define ER7_UTILS_ALLOC_CLASS_OBJECT(type,cotr) \
   JEOD_ALLOC_PRIM_ARRAY (type, cotr)

#define ER7_UTILS_DELETE_ARRAY(arr) \
   JEOD_DELETE_ARRAY(arr)

#define ER7_UTILS_DELETE_OBJECT(obj) \
   JEOD_DELETE_OBJECT(obj)


/*
 * Generic implementations of the four macros.
 * Note that the generic implementations come into play only in the case
 * of a non-Trick, non-JEOD implementation.
 */
#else
namespace er7_utils {
   namespace default_interface {

      template <typename T>
      inline T* allocate_array (std::size_t size) {
         T* arr = new T[size];
         std::memset (arr, 0, size*sizeof(T));
         return arr;
      }

      template <typename T>
      inline void delete_array (T*& obj) {
         if (obj) {
            delete[] obj;
            obj = 0;
         }
      }

      template <typename T>
      inline void delete_object (T*& obj) {
         if (obj) {
            delete obj;
            obj = 0;
         }
      }
   }
}

#define ER7_UTILS_ALLOC_PRIM_ARRAY(size,type) \
   er7_utils::default_interface::allocate_array<type> (size)

#define ER7_UTILS_ALLOC_CLASS_OBJECT(type,cotr) \
   new type cotr

#define ER7_UTILS_DELETE_ARRAY(arr) \
   er7_utils::default_interface::delete_array (arr)

#define ER7_UTILS_DELETE_OBJECT(obj) \
   er7_utils::default_interface::delete_object (obj)

#endif


/******************************************************************************/

// Part three, allocation/deallocation template functions.


namespace er7_utils {

   /**
    * Contains free functions that pertain to initialization (setting to NULL),
    * allocating memory, and releasing allocated memory.
    */
   namespace alloc {

      /**
       * Allocate an object of some type using the default constructor.
       * @tparam T    Array type.
       * @return      Allocated object.
       */
      template<typename T>
      inline T * allocate_object ()
      {
         return ER7_UTILS_ALLOC_CLASS_OBJECT (T, ());
      }

      /**
       * Allocate an object of some type using the copy constructor.
       * @tparam T    Array type.
       * @param  src  Argument.
       * @return      Allocated object.
       */
      template<typename T>
      inline T * replicate_object (const T & src)
      {
         return ER7_UTILS_ALLOC_CLASS_OBJECT (T, (src));
      }

      /**
       * Allocate an object of some type using a one argument constructor.
       * @tparam T     Array type.
       * @tparam A1    Argument type.
       * @param  arg1  Argument.
       * @return       Allocated object.
       */
      template<typename T, typename A1>
       inline T * allocate_object (
         A1 arg1)
      {
         return ER7_UTILS_ALLOC_CLASS_OBJECT (T, (arg1));
      }

      /**
       * Allocate an object of some type using a two argument constructor.
       * @tparam T     Array type.
       * @tparam A1    Argument 1 type.
       * @tparam A2    Argument 2 type.
       * @param  arg1  Argument 1.
       * @param  arg2  Argument 2.
       * @return       Allocated object.
       */
      template<typename T, typename A1, typename A2>
       inline T * allocate_object (
         A1 arg1,
         A2 arg2)
      {
         return ER7_UTILS_ALLOC_CLASS_OBJECT (T, (arg1, arg2));
      }

      /**
       * Allocate an object of some type using a three argument constructor.
       * @tparam T     Array type.
       * @tparam A1    Argument 1 type.
       * @tparam A2    Argument 2 type.
       * @tparam A3    Argument 3 type.
       * @param  arg1  Argument 1.
       * @param  arg2  Argument 2.
       * @param  arg3  Argument 3.
       * @return       Allocated object.
       */
      template<typename T, typename A1, typename A2, typename A3>
       inline T * allocate_object (
         A1 arg1,
         A2 arg2,
         A3 arg3)
      {
         return ER7_UTILS_ALLOC_CLASS_OBJECT (T, (arg1, arg2, arg3));
      }

      /**
       * Allocate an object of some type using a four argument constructor.
       * @tparam T     Array type.
       * @tparam A1    Argument 1 type.
       * @tparam A2    Argument 2 type.
       * @tparam A3    Argument 3 type.
       * @tparam A4    Argument 4 type.
       * @param  arg1  Argument 1.
       * @param  arg2  Argument 2.
       * @param  arg3  Argument 3.
       * @param  arg4  Argument 4.
       * @return       Allocated object.
       */
      template<typename T, typename A1, typename A2, typename A3, typename A4>
       inline T * allocate_object (
         A1 arg1,
         A2 arg2,
         A3 arg3,
         A4 arg4)
      {
         return ER7_UTILS_ALLOC_CLASS_OBJECT (T, (arg1, arg2, arg3, arg4));
      }

      /**
       * Allocate an object of some type using a five argument constructor.
       * @tparam T     Array type.
       * @tparam A1    Argument 1 type.
       * @tparam A2    Argument 2 type.
       * @tparam A3    Argument 3 type.
       * @tparam A4    Argument 4 type.
       * @tparam A5    Argument 5 type.
       * @param  arg1  Argument 1.
       * @param  arg2  Argument 2.
       * @param  arg3  Argument 3.
       * @param  arg4  Argument 4.
       * @param  arg5  Argument 5.
       * @return       Allocated object.
       */
      template<typename T,
               typename A1,
               typename A2,
               typename A3,
               typename A4,
               typename A5>
       inline T * allocate_object (
         A1 arg1,
         A2 arg2,
         A3 arg3,
         A4 arg4,
         A5 arg5)
      {
         return ER7_UTILS_ALLOC_CLASS_OBJECT (
                   T, (arg1, arg2, arg3, arg4, arg5));
      }

      /**
       * Allocate an object of some type using a six argument constructor.
       * @tparam T     Array type.
       * @tparam A1    Argument 1 type.
       * @tparam A2    Argument 2 type.
       * @tparam A3    Argument 3 type.
       * @tparam A4    Argument 4 type.
       * @tparam A5    Argument 5 type.
       * @tparam A6    Argument 6 type.
       * @param  arg1  Argument 1.
       * @param  arg2  Argument 2.
       * @param  arg3  Argument 3.
       * @param  arg4  Argument 4.
       * @param  arg5  Argument 5.
       * @param  arg6  Argument 6.
       * @return       Allocated object.
       */
      template<typename T,
               typename A1,
               typename A2,
               typename A3,
               typename A4,
               typename A5,
               typename A6>
       inline T * allocate_object (
         A1 arg1,
         A2 arg2,
         A3 arg3,
         A4 arg4,
         A5 arg5,
         A6 arg6)
      {
         return ER7_UTILS_ALLOC_CLASS_OBJECT (
                   T, (arg1, arg2, arg3, arg4, arg5, arg6));
      }


      /**
       * Delete (and destruct) an instance of some class.
       * @tparam T     Object class.
       * @param  obj   The object to be deleted.
       */
      template<typename T>
      inline void delete_object (
         T *& obj)
      {
         ER7_UTILS_DELETE_OBJECT (obj);
      }


      /**
       * Allocate an array of some primitive type.
       * Note that because none of the arguments involve the type T,
       * invocations of the function template must always specify T.
       * @tparam T    Array type.
       * @param size  Number of elements to allocate, assumed to be positive.
       * @return      Allocated array.
       */
      template<typename T>
      inline T * allocate_array (unsigned int size)
      {
         return ER7_UTILS_ALLOC_PRIM_ARRAY (size, T);
      }

      /**
       * Non-template specialization of allocate_array for double.
       * Note that since this is not a function template, calling
       * allocate_array without a template argument will always resolve
       * to this function.
       * @param size  Number of elements to allocate, assumed to be positive.
       * @return      Allocated array.
       */
      inline double * allocate_array (unsigned int size)
      {
         return ER7_UTILS_ALLOC_PRIM_ARRAY (size, double);
      }


      /**
       * Replicate an array of some primitive type
       * @tparam T    Array type.
       * @param size  Number of elements to allocate, assumed to be positive.
       * @param src   Array to be replicated.
       * @return      Allocated array.
       */
      template<typename T>
       inline T * replicate_array (unsigned int size, const T* src)
      {
         T* result = ER7_UTILS_ALLOC_PRIM_ARRAY (size, T);
         for (unsigned int ii = 0; ii < size; ++ii) {
            result[ii] = src[ii];
         }
         return result;
      }


      /**
       * Deallocate an array of some primitive type
       * @tparam T    Array type.
       * @param arr   Array to be deallocated.
       */
      template<typename T>
      inline void deallocate_array (T *& arr)
      {
         ER7_UTILS_DELETE_ARRAY (arr);
      }


      /**
       * Zero out an array of items
       * @param[in]  size  Array size
       * @param[out] arr   Array to zero
       */
      template<typename T>
      inline void initialize_array (
         unsigned int size,
         T * arr)
      {
         std::memset (arr, 0, size*sizeof(T));
      }



      /**
       * Initialize (zero-out) a 2D array.
       * @tparam        T     Array type
       * @tparam        M     Array outer dimension
       * @param[out]    arr   Array to be initialized
       */
      template<typename T, unsigned int M>
      inline void initialize_2D_array (
         T * arr[M])
      {
         initialize_array<T*> (M, arr);
      }

      /**
       * Specialization of initialize_2D_array for double.
       * @tparam        M     Array outer dimension
       * @param[out]    arr   Array to be initialized
       */
      template<unsigned int M>
      inline void initialize_2D_array (
         double * arr[M])
      {
         initialize_array<double*> (M, arr);
      }


      /**
       * Allocate contents of a 2D array, where the array is declared as
       * @code T * array[M] @endcode
       * @tparam        T     Array type
       * @tparam        M     Array outer dimension
       * @param[in]     size  Array inner dimension
       * @param[out]    arr   Array to be allocated
       */
      template<typename T, unsigned int M>
      inline void allocate_2D_array (
         unsigned int size,
         T * arr[M])
      {
         arr[0] = allocate_array<T> (size*M);
         for (unsigned int ii = 1; ii < M; ++ii) {
            arr[ii] = arr[ii-1] + size;
         }
      }

      /**
       * Specialization of allocate_2D_array for double.
       * @tparam        M     Array outer dimension
       * @param[in]     size  Array inner dimension
       * @param[out]    arr   Array to be allocated
       */
      template<unsigned int M>
      inline void allocate_2D_array (
         unsigned int size,
         double * arr[M])
      {
         arr[0] = allocate_array<double> (size*M);
         for (unsigned int ii = 1; ii < M; ++ii) {
            arr[ii] = arr[ii-1] + size;
         }
      }


      /**
       * Allocate contents of a 2D array, where the array is declared as
       * @code T ** array @endcode
       * @tparam        T     Array type
       * @param         outer Array outer dimension
       * @param         inner Array inner dimension
       * @return Allocated 2D array.
       */
      template<typename T>
      inline T** allocate_2D_array (
         unsigned int outer,
         unsigned int inner)
      {
         T** arr = allocate_array<T*> (outer);
         arr[0] = allocate_array<T> (inner*outer);
         for (unsigned int ii = 1; ii < outer; ++ii) {
            arr[ii] = arr[ii-1] + inner;
         }
         return arr;
      }


      /**
       * Replicate contents of a 2D array, where the array is declared as
       * @code T * array[M] @endcode
       * @tparam        T     Array type
       * @tparam        M     Array outer dimension
       * @param[in]     size  Array inner dimension
       * @param[in]     src   Source 2D array
       * @param[out]    tgt   Array to be allocated and populated
       */
      template<typename T, unsigned int M>
      inline void replicate_2D_array (
         unsigned int size,
         T * const src[M],
         T * tgt[M])
      {
         for (unsigned int ii = 0; ii < M; ++ii) {
            if (ii == 0) {
               tgt[ii] = allocate_array<T> (size*M);
            }
            else {
               tgt[ii] = tgt[ii-1] + size;
            }
            for (unsigned int jj = 0; jj < size; ++jj) {
               tgt[ii][jj] = src[ii][jj];
            }
         }
      }

      /**
       * Specialization of replicate_2D_array for double.
       * @tparam        M     Array outer dimension
       * @param[in]     size  Array inner dimension
       * @param[in]     src   Source 2D array
       * @param[out]    tgt   Array to be allocated and populated
       */
      template<unsigned int M>
      inline void replicate_2D_array (
         unsigned int size,
         double * const src[M],
         double * tgt[M])
      {
         for (unsigned int ii = 0; ii < M; ++ii) {
            if (ii == 0) {
               tgt[ii] = allocate_array<double> (size*M);
            }
            else {
               tgt[ii] = tgt[ii-1] + size;
            }
            for (unsigned int jj = 0; jj < size; ++jj) {
               tgt[ii][jj] = src[ii][jj];
            }
         }
      }


      /**
       * Replicate contents of a 2D array, where the array is declared as
       * @code T ** array @endcode
       * @tparam        T      Array type
       * @param         outer  Array outer dimension
       * @param         inner  Array inner dimension
       * @param         source Array to be replicated
       * @return Allocated and replicated 2D array.
       */
      template<typename T>
      inline T** replicate_2D_array (
         unsigned int outer,
         unsigned int inner,
         T** source)
      {
         T** tgt = allocate_array<T*> (outer);
         for (unsigned int ii = 0; ii < outer; ++ii) {
            if (ii == 0) {
               tgt[ii] = allocate_array<T> (inner*outer);
            }
            else {
               tgt[ii] = tgt[ii-1] + inner;
            }
            for (unsigned int jj = 0; jj < inner; ++jj) {
               tgt[ii][jj] = source[ii][jj];
            }
         }
         return tgt;
      }


      /**
       * Deallocate a 2D array.
       * @tparam        T     Array type
       * @tparam        M     Array outer dimension
       * @param[in,out] arr   Array to be deallocated
       */
      template<typename T, unsigned int M>
      inline void deallocate_2D_array (
         T * arr[M])
      {
         deallocate_array<T> (arr[0]);
      }

      /**
       * Specialization of deallocate_2D_array for double.
       * @tparam        M     Array outer dimension
       * @param[in,out] arr   Array to be deallocated
       */
      template<unsigned int M>
      inline void deallocate_2D_array (
         double * arr[M])
      {
         deallocate_array<double> (arr[0]);
      }



      /**
       * Deallocate a 2D array.
       * @tparam        T     Array type
       * @param[in,out] arr   Array to be deallocated
       */
      template<typename T>
      inline void deallocate_2D_array (
         unsigned int,
         T **& arr)
      {
         if (arr != NULL) {
            deallocate_array<T> (arr[0]);
            deallocate_array<T*> (arr);
         }
      }
   }

}


#endif

#endif

/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

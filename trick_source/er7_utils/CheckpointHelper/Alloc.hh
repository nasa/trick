/*******************************************************************************

 PURPOSE:
 (alloc template for allocating and reallocating elements.)

 LIBRARY DEPENDENCY:
 ((Alloc.cpp))

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (May 2013) (--)))

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_alloc_HH_
#define CHECKPOINTHELPER_alloc_HH_
#if (defined TRICK_VER) && (TRICK_VER >= 10)
#include "sim_services/MemoryManager/include/MemoryManager.hh"
#include "sim_services/Message/include/message_proto.h"
#endif
#include <string>
#include <sstream>
#include <algorithm>
#include "Manager.hh"

namespace CheckpointHelper
{

extern Manager * helperManager;

#if (defined TRICK_VER) && (TRICK_VER >= 10)
typedef TRICK_TYPE MM_TYPE;

std::string treatAllocName(const std::string & nameIn);
bool updateAllocName(void * parentPtr, void * ptr, const std::string & name, const std::string & funcName,
      bool printError);

template<class T>
void delete_alloc(T ** x, const std::string & whereAt, bool external)
{
   if (!external)
   {
      if (trick_MM->is_alloced(*x))
      {
         trick_MM->delete_var((void *) *x);
         if (helperManager != 0x0)
         {
            helperManager->removeConstructorAlloc(*x);
         }
         *x = 0x0;
      }
      else if (*x != 0x0)
      {
         std::stringstream ss;
         ss << "CheckpointHelper::delete_alloc ERROR attempting to delete ptr at " << whereAt
               << " that is not allocated.\n";
         message_publish(MSG_WARNING, ss.str().c_str());
         *x = 0x0;
      }
   }
   else
   {
      if (trick_MM->is_alloced(*x))
      {
         trick_MM->delete_extern_var((void *) *x);
         if (helperManager != 0x0)
         {
            helperManager->removeConstructorAlloc(*x);
            size_t numEntries = helperManager->getExternNum(*x);
            helperManager->removeExternAlloc(*x);
            for (int ii = numEntries - 1; ii >= 0; --ii)
            {
               (*x)[ii].~T();
            }
            free(*x);
            *x = 0x0;
         }
      }
      else if (*x != 0x0)
      {
         std::stringstream ss;
         ss << "CheckpointHelper::delete_alloc ERROR attempting to delete external ptr at " << whereAt
               << " that is not allocated.\n";
         message_publish(MSG_WARNING, ss.str().c_str());
         *x = 0x0;
      }
   }
}

template<class U, class T>
void operatornew_alloc(void * parentPtr, T ** x, int n, const std::string & TYPE, const std::string & name,
      const std::string & funcNameIn)
{
   if (n)
   {
      if (trick_MM->is_alloced(*x))
      {
         int orig_size = trick_MM->get_size((char *) *x);
         if (n > orig_size)
         {
            helperManager->removeConstructorAlloc(*x);
            *x = (U *) trick_MM->resize_array((void *) *x, n);
            helperManager->addConstructorAlloc(parentPtr, *x, name, funcNameIn);
         }
      }
      else
      {
         ALLOC_INFO * info = trick_MM->get_alloc_info_of(x);
         if (info != 0x0)
         {
            if (info->name != 0x0)
            {
               *x = static_cast<U *>(trick_MM->declare_operatornew_var(TYPE, sizeof(U) * n, sizeof(U)));
               std::string pathName = trick_MM->ref_name_from_address(x);
               pathName = treatAllocName(pathName);
               trick_MM->set_name_at(*x, pathName.c_str());
            }
            else
            {
               *x = static_cast<U *>(trick_MM->declare_operatornew_var(TYPE, sizeof(U) * n, sizeof(U)));
               if (helperManager != 0x0)
               {
                  helperManager->addConstructorAlloc(parentPtr, *x, name, funcNameIn);
               }
            }
         }
         else
         {
            *x = static_cast<U *>(trick_MM->declare_operatornew_var(TYPE, sizeof(U) * n, sizeof(U)));
            if (helperManager != 0x0)
            {
               helperManager->addConstructorAlloc(parentPtr, *x, name, funcNameIn);
            }
         }
      }
   }
   else
   {
      *x = 0;
   }
}

template<class U, class T>
void alloc(void * parentPtr, T ** x, TRICK_TYPE type, const std::string & TYPE_NAME, int n_stars,
      const std::string & name, int n_cdims, int cdims[8], bool external, const std::string & funcNameIn)
{
   if (cdims[0])
   {
      if (trick_MM->is_alloced(*x))
      {
         int orig_size = trick_MM->get_size((char *) *x);
         if (cdims[0] > orig_size)
         {
            helperManager->removeConstructorAlloc(*x);
            *x = (U *) trick_MM->resize_array((void *) *x, cdims[0]);
            helperManager->addConstructorAlloc(parentPtr, *x, name, funcNameIn);
         }
      }
      else
      {
         ALLOC_INFO * info = trick_MM->get_alloc_info_of(x);
         if (info != 0x0)
         {
            if (info->name != 0x0)
            {
               std::string pathName = trick_MM->ref_name_from_address(x);
               pathName = treatAllocName(pathName);
               if (external)
               {
                  if (*x == 0x0)
                  {
                     *x = reinterpret_cast<U *>(calloc(cdims[0], sizeof(U)));
                     new (*x) U[cdims[0]]();
                     if (helperManager != 0x0)
                     {
                        helperManager->addExternAlloc(*x, cdims[0]);
                     }
                  }
                  trick_MM->declare_extern_var(*x, type, TYPE_NAME, n_stars, pathName, n_cdims, cdims);
               }
               else
               {
                  *x = (U *) trick_MM->declare_var(type, TYPE_NAME, n_stars, pathName, n_cdims, cdims);
               }
            }
            else
            {
               if (external)
               {
                  if (*x == 0x0)
                  {
                     *x = reinterpret_cast<U *>(calloc(cdims[0], sizeof(U)));
                     new (*x) U[cdims[0]]();
                     if (helperManager != 0x0)
                     {
                        helperManager->addExternAlloc(*x, cdims[0]);
                     }
                  }
                  trick_MM->declare_extern_var(*x, type, TYPE_NAME, n_stars, "", n_cdims, cdims);
               }
               else
               {
                  *x = (U *) trick_MM->declare_var(type, TYPE_NAME, n_stars, "", n_cdims, cdims);
               }
               if (helperManager != 0x0)
               {
                  helperManager->addConstructorAlloc(parentPtr, *x, name, funcNameIn);
               }
            }
         }
         else
         {
            if (external)
            {
               if (*x == 0x0)
               {
                  *x = reinterpret_cast<U *>(calloc(cdims[0], sizeof(U)));
                  new (*x) U[cdims[0]]();
                  if (helperManager != 0x0)
                  {
                     helperManager->addExternAlloc(*x, cdims[0]);
                  }
               }
               trick_MM->declare_extern_var(*x, type, TYPE_NAME, n_stars, "", n_cdims, cdims);
            }
            else
            {
               *x = (U *) trick_MM->declare_var(type, TYPE_NAME, n_stars, "", n_cdims, cdims);
            }
            if (helperManager != 0x0)
            {
               helperManager->addConstructorAlloc(parentPtr, *x, name, funcNameIn);
            }
         }
      }
   }
   else
   {
      *x = 0;
   }
}

template<class U, class T>
void alloc(void * parentPtr, T ** x, int n, const std::string & TYPE, const std::string & name, bool external,
      const std::string & funcNameIn)
{
   int cdims[8] =
   { n, 0, 0, 0, 0, 0, 0, 0 };
   std::string treatedTYPE = TYPE;
   size_t ret;
   while ((ret = treatedTYPE.find(' ')) != std::string::npos)
   {
      treatedTYPE.erase(ret);
   }
   while ((ret = treatedTYPE.find('*')) != std::string::npos)
   {
      treatedTYPE.erase(ret);
   }
   CheckpointHelper::alloc<U>(parentPtr, x, TRICK_OPAQUE_TYPE, treatedTYPE, 0, name, 1, cdims, external, funcNameIn);
}

template<class U, class T>
void alloc(void * parentPtr, T *** x, int n, const std::string & TYPE, const std::string & name, bool external,
      const std::string & funcNameIn)
{
   int cdims[8] =
   { n, 0, 0, 0, 0, 0, 0, 0 };
   std::string treatedTYPE = TYPE;
   size_t ret;
   while ((ret = treatedTYPE.find(' ')) != std::string::npos)
   {
      treatedTYPE.erase(ret);
   }
   while ((ret = treatedTYPE.find('*')) != std::string::npos)
   {
      treatedTYPE.erase(ret);
   }
   CheckpointHelper::alloc<U>(parentPtr, x, TRICK_OPAQUE_TYPE, treatedTYPE, 1, name, 1, cdims, external, funcNameIn);
}

template<class U, class T>
void alloc(void * parentPtr, T **** x, int n, const std::string & TYPE, const std::string & name, bool external,
      const std::string & funcNameIn)
{
   int cdims[8] =
   { n, 0, 0, 0, 0, 0, 0, 0 };
   std::string treatedTYPE = TYPE;
   size_t ret;
   while ((ret = treatedTYPE.find(' ')) != std::string::npos)
   {
      treatedTYPE.erase(ret);
   }
   while ((ret = treatedTYPE.find('*')) != std::string::npos)
   {
      treatedTYPE.erase(ret);
   }
   CheckpointHelper::alloc<U>(parentPtr, x, TRICK_OPAQUE_TYPE, treatedTYPE, 2, name, 1, cdims, external, funcNameIn);
}

#define CHECKPOINTHELPER_PRIM_ALLOC_TEMPS( primType, trickType ) \
template<> \
inline void alloc<primType>(void * parentPtr, primType ** x, int n, const std::string & TYPE, const std::string & name, \
      bool external, const std::string & funcNameIn) \
{ \
   int cdims[8] = { n, 0, 0, 0, 0, 0, 0, 0 }; \
   CheckpointHelper::alloc<primType>(parentPtr, x, trickType, "", 0, name, 1, cdims, external, funcNameIn); \
} \
\
template<> \
inline void alloc<primType *, primType>(void * parentPtr, primType *** x, int n, const std::string & TYPE, const std::string & name, \
      bool external, const std::string & funcNameIn) \
{ \
   int cdims[8] = { n, 0, 0, 0, 0, 0, 0, 0 }; \
   CheckpointHelper::alloc<primType *>(parentPtr, x, trickType, "", 1, name, 1, cdims, external, funcNameIn); \
} \
\
template<> \
inline void alloc<primType **, primType>(void * parentPtr, primType **** x, int n, const std::string & TYPE, const std::string & name, \
      bool external, const std::string & funcNameIn) \
{ \
   int cdims[8] = { n, 0, 0, 0, 0, 0, 0, 0 }; \
   CheckpointHelper::alloc<primType **>(parentPtr, x, trickType, "", 2, name, 1, cdims, external, funcNameIn); \
}

CHECKPOINTHELPER_PRIM_ALLOC_TEMPS(double, TRICK_DOUBLE)
CHECKPOINTHELPER_PRIM_ALLOC_TEMPS(char, TRICK_CHARACTER)
CHECKPOINTHELPER_PRIM_ALLOC_TEMPS(unsigned char, TRICK_UNSIGNED_CHARACTER)
CHECKPOINTHELPER_PRIM_ALLOC_TEMPS(std::string, TRICK_STRING)
CHECKPOINTHELPER_PRIM_ALLOC_TEMPS(int, TRICK_INTEGER)
CHECKPOINTHELPER_PRIM_ALLOC_TEMPS(unsigned int, TRICK_UNSIGNED_INTEGER)
CHECKPOINTHELPER_PRIM_ALLOC_TEMPS(long, TRICK_LONG)
CHECKPOINTHELPER_PRIM_ALLOC_TEMPS(unsigned long, TRICK_UNSIGNED_LONG)
CHECKPOINTHELPER_PRIM_ALLOC_TEMPS(float, TRICK_FLOAT)
CHECKPOINTHELPER_PRIM_ALLOC_TEMPS(long long, TRICK_LONG_LONG)
CHECKPOINTHELPER_PRIM_ALLOC_TEMPS(unsigned long long, TRICK_UNSIGNED_LONG_LONG)
CHECKPOINTHELPER_PRIM_ALLOC_TEMPS(bool, TRICK_BOOLEAN)
CHECKPOINTHELPER_PRIM_ALLOC_TEMPS(wchar_t, TRICK_WCHAR)

#else

typedef int MM_TYPE;

template<class T>
void alloc( void * parentPtr, T ** x, int n, std::string TYPE, std::string name )
{
   (void) parentPtr;
   (void) x;
   (void) n;
   (void) TYPE;
   (void) name;

   *x = new T[n];
}

template<class T>
void delete_alloc(T ** x)
{
   if (x != 0x0)
   {
      if(*x != 0x0)
      {
         delete[] *x;
      }
   }
}
#endif

#ifndef CHECKPOINTHELPER_STRINGIFY
#define CHECKPOINTHELPER_STRINGIFY(x) #x
#endif
#ifndef CHECKPOINTHELPER_TOSTRING
#define CHECKPOINTHELPER_TOSTRING(x) CHECKPOINTHELPER_STRINGIFY(x)
#endif

#ifndef CHECKPOINTHELPER_FILELINE
#define CHECKPOINTHELPER_FILELINE __FILE__ ":" CHECKPOINTHELPER_TOSTRING(__LINE__)
#endif

// get number of arguments with __NARG__
#define __NARG__(...)  __NARG_I_(__VA_ARGS__,__RSEQ_N())
#define __NARG_I_(...) __ARG_N(__VA_ARGS__)
#define __ARG_N( \
      _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
     _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
     _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
     _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
     _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
     _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
     _61,_62,_63,N,...) N
#define __RSEQ_N() \
     63,62,61,60,                   \
     59,58,57,56,55,54,53,52,51,50, \
     49,48,47,46,45,44,43,42,41,40, \
     39,38,37,36,35,34,33,32,31,30, \
     29,28,27,26,25,24,23,22,21,20, \
     19,18,17,16,15,14,13,12,11,10, \
     9,8,7,6,5,4,3,2,1,0

// general definition for any function name
#define _VFUNC_(name, n) name##n
#define _VFUNC(name, n) _VFUNC_(name, n)
#define VFUNC(func, ...) _VFUNC(func, __NARG__(__VA_ARGS__)) (__VA_ARGS__)

// definition our overloaded macros
#define CHECKPOINTHELPER_ALLOC_WCSTR6(parent, item, num, type, name, cstr) \
   CheckpointHelper::operatornew_alloc<type>(parent, &item, num, #type, name, CHECKPOINTHELPER_FILELINE); \
   for(int ii = 0; ii < num; ++ii) { \
      new (&item[ii]) type cstr; \
   }

#define CHECKPOINTHELPER_ALLOC_WCSTR5(item, num, type, name, cstr) \
   CheckpointHelper::operatornew_alloc<type>(this, &item, num, #type, name, CHECKPOINTHELPER_FILELINE); \
   for(int ii = 0; ii < num; ++ii) { \
      new (&item[ii]) type cstr; \
   }

#define CHECKPOINTHELPER_ALLOC_WCSTR4(item, num, type, cstr) \
   CheckpointHelper::operatornew_alloc<type>(this, &item, num, #type, #item, CHECKPOINTHELPER_FILELINE); \
   for(int ii = 0; ii < num; ++ii) { \
      new (&item[ii]) type cstr; \
   }

#define CHECKPOINTHELPER_ALLOC_WCSTR3(item, type, cstr) \
   CheckpointHelper::operatornew_alloc<type>(this, &item, 1, #type, #item, CHECKPOINTHELPER_FILELINE); \
   new (item) type cstr;

#define CHECKPOINTHELPER_ALLOC5(parent, item, num, type, name) CheckpointHelper::alloc<type>(parent, &item, num, #type, name, false, CHECKPOINTHELPER_FILELINE)
#define CHECKPOINTHELPER_ALLOC4(item, num, type, name) CheckpointHelper::alloc<type>(this, &item, num, #type, name, false, CHECKPOINTHELPER_FILELINE)
#define CHECKPOINTHELPER_ALLOC3(item, num, type) CheckpointHelper::alloc<type>(this, &item, num, #type, #item, false, CHECKPOINTHELPER_FILELINE)
#define CHECKPOINTHELPER_ALLOC2(item, type) CheckpointHelper::alloc<type>(this, &item, 1, #type, #item, false, CHECKPOINTHELPER_FILELINE)

#define CHECKPOINTHELPER_EXTERN_ALLOC_WCSTR6(parent, item, num, type, name, cstr)  \
   item = reinterpret_cast<type *>(calloc(num, sizeof(type))); \
   for(int ii = 0; ii < num; ++ii) { \
      new (&item[ii]) type cstr; \
   } \
   CheckpointHelper::alloc<type>(parent, &item, num, #type, name, true, CHECKPOINTHELPER_FILELINE)

#define CHECKPOINTHELPER_EXTERN_ALLOC_WCSTR5(item, num, type, name, cstr) \
   item = reinterpret_cast<type *>(calloc(num, sizeof(type))); \
   for(int ii = 0; ii < num; ++ii) { \
      new (&item[ii]) type cstr; \
   } \
   CheckpointHelper::alloc<type>(this, &item, num, #type, name, true, CHECKPOINTHELPER_FILELINE)

#define CHECKPOINTHELPER_EXTERN_ALLOC_WCSTR4(item, num, type, cstr) \
   item = reinterpret_cast<type *>(calloc(num, sizeof(type))); \
   for(int ii = 0; ii < num; ++ii) { \
      new (&item[ii]) type cstr; \
   } \
   CheckpointHelper::alloc<type>(this, &item, num, #type, #item, true, CHECKPOINTHELPER_FILELINE)

#define CHECKPOINTHELPER_EXTERN_ALLOC_WCSTR3(item, type, cstr) \
   item = reinterpret_cast<type *>(calloc(1, sizeof(type))); \
   new (item) type cstr; \
   CheckpointHelper::alloc<type>(this, &item, 1, #type, #item, true, CHECKPOINTHELPER_FILELINE)

#define CHECKPOINTHELPER_EXTERN_ALLOC5(parent, item, num, type, name) CheckpointHelper::alloc<type>(parent, &item, num, #type, name, true, CHECKPOINTHELPER_FILELINE)
#define CHECKPOINTHELPER_EXTERN_ALLOC4(item, num, type, name) CheckpointHelper::alloc<type>(this, &item, num, #type, name, true, CHECKPOINTHELPER_FILELINE)
#define CHECKPOINTHELPER_EXTERN_ALLOC3(item, num, type) CheckpointHelper::alloc<type>(this, &item, num, #type, #item, true, CHECKPOINTHELPER_FILELINE)
#define CHECKPOINTHELPER_EXTERN_ALLOC2(item, type) CheckpointHelper::alloc<type>(this, &item, 1, #type, #item, true, CHECKPOINTHELPER_FILELINE)

#define CHECKPOINTHELPER_ALLOC_WCSTR(...) VFUNC(CHECKPOINTHELPER_ALLOC_WCSTR, __VA_ARGS__)
#define CHECKPOINTHELPER_ALLOC(...) VFUNC(CHECKPOINTHELPER_ALLOC, __VA_ARGS__)
#define CHECKPOINTHELPER_EXTERN_ALLOC_WCSTR(...) VFUNC(CHECKPOINTHELPER_EXTERN_ALLOC_WCSTR, __VA_ARGS__)
#define CHECKPOINTHELPER_EXTERN_ALLOC(...) VFUNC(CHECKPOINTHELPER_EXTERN_ALLOC, __VA_ARGS__)

#define CHECKPOINTHELPER_DELETE(item) CheckpointHelper::delete_alloc(&item, CHECKPOINTHELPER_FILELINE " " #item, false)
#define CHECKPOINTHELPER_EXTERN_DELETE(item) CheckpointHelper::delete_alloc(&item, CHECKPOINTHELPER_FILELINE " " #item, true)

}

#endif /* STLHELPER_alloc_HH_ */

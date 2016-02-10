/*******************************************************************************

 PURPOSE:
 (Set for checkpointable set container.)

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (May 2013) (--)))

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_SET_HH_
#define CHECKPOINTHELPER_SET_HH_

#include "CheckpointItem.hh"
#include <set>

namespace CheckpointHelper
{

template<class T>
class set: public CheckpointItem, public std::set<T>
{
public:
   T ** chkpt;
   std::string mmstr;

   set(T ** chkptIn, std::string mmstrIn) :
         chkpt(chkptIn), mmstr(mmstrIn)
   {
      *chkpt = 0x0;
   }

   set<T>& operator=(const std::set<T>& pr)
   {
      std::set<T>::operator=(pr);
      return *this;
   }

   virtual ~set()
   {
   }

   virtual void checkpoint()
   {
      chkpt_seq<typename std::set<T>::iterator, T>(*this);
   }
   virtual void restart()
   {
      restart_seq<typename std::set<T>::iterator>(*this);
   }
};

template<class T>
class multiset: public CheckpointHelper::CheckpointItem, public std::multiset<T>
{
public:
   T ** chkpt;
   std::string mmstr;

   multiset(T ** chkptIn, std::string mmstrIn) :
         chkpt(chkptIn), mmstr(mmstrIn)
   {
      *chkpt = 0x0;
   }

   multiset<T>& operator=(const std::multiset<T>& pr)
   {
      std::multiset<T>::operator=(pr);
      return *this;
   }

   virtual ~multiset()
   {
   }

   virtual void checkpoint()
   {
      chkpt_seq<typename std::multiset<T>::iterator, T>(*this);
   }
   virtual void restart()
   {
      restart_seq<typename std::multiset<T>::iterator>(*this);
   }
};

}

#if  (!defined(SWIG) && !defined(TRICK_ICG))
#define STLSET( type, varName ) \
   type* _##varName; \
   CheckpointHelper::set<type> varName
#define STLMULTISET( type, varName ) \
   type* _##varName; \
   CheckpointHelper::multiset<type> varName
#else
#define STLSET( type, varName ) \
   type* _##varName
#define STLMULTISET( type, varName ) \
   type* _##varName;
#endif

#if  (!defined(SWIG) && !defined(TRICK_ICG))
#ifndef STL_CSTR1
#define STL_CSTR1( varName, type  ) \
   varName( &_##varName, #type )
#endif
#else
#ifndef STL_CSTR1
#define STL_CSTR1( varName, type  ) \
   _##varName()
#endif
#endif

#define STLSETCSTR STL_CSTR1
#define STLMULTISETCSTR STL_CSTR1

#endif /* STLHELPER_SET_HH_ */

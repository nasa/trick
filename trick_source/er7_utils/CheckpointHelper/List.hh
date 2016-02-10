/*******************************************************************************

 PURPOSE:
 (List for checkpointable list container.)

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (May 2013) (--)))

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_LIST_HH_
#define CHECKPOINTHELPER_LIST_HH_

#include "CheckpointItem.hh"
#include <list>

namespace CheckpointHelper
{

template<class T>
class list: public CheckpointItem, public std::list<T>
{
public:
   T ** chkpt;
   std::string mmstr;

   list(T ** chkptIn, std::string mmstrIn) :
         chkpt(chkptIn), mmstr(mmstrIn)
   {
      *chkpt = 0x0;
   }

   list<T>& operator=(const std::list<T>& pr)
   {
      std::list<T>::operator=(pr);
      return *this;
   }

   virtual ~list()
   {
   }

   virtual void checkpoint()
   {
      chkpt_seq<typename std::list<T>::iterator, T>(*this);
   }
   virtual void restart()
   {
      restart_seq<typename std::list<T>::iterator>(*this);
   }
};

}

#if  (!defined(SWIG) && !defined(TRICK_ICG))
#define STLLIST( type, varName ) \
   type* _##varName; \
   CheckpointHelper::list<type> varName
#else
#define STLLIST( type, varName ) \
   type* _##varName
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

#define STLLISTCSTR STL_CSTR1

#endif /* STLHELPER_LIST_HH_ */

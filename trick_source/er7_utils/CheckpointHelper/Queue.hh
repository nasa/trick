/*******************************************************************************

 PURPOSE:
 (Queue for checkpointable queue container.)

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (May 2013) (--)))

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_QUEUE_HH_
#define CHECKPOINTHELPER_QUEUE_HH_

#include "CheckpointItem.hh"
#include <queue>

namespace CheckpointHelper
{

template<class T>
class queue: public CheckpointItem, public std::queue<T>
{
public:
   T ** chkpt;
   std::string mmstr;

   queue(T ** chkptIn, std::string mmstrIn) :
         chkpt(chkptIn), mmstr(mmstrIn)
   {
      *chkpt = 0x0;
   }

   queue<T>& operator=(const std::queue<T>& pr)
   {
      std::queue<T>::operator=(pr);
      return *this;
   }

   virtual ~queue()
   {
   }

   virtual void checkpoint()
   {
      chkpt_queue<typename std::queue<T>, T>(*this);
   }
   virtual void restart()
   {
      restart_queue(*this);
   }
};

}

#if  (!defined(SWIG) && !defined(TRICK_ICG))
#define STLQUEUE( type, varName ) \
   type* _##varName; \
   CheckpointHelper::queue<type> varName
#else
#define STLQUEUE( type, varName ) \
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

#define STLQUEUECSTR STL_CSTR1

#endif /* STLHELPER_QUEUE_HH_ */

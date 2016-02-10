/*******************************************************************************

 PURPOSE:
 (List for checkpointable deque container.)

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (May 2013) (--)))

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_DEQUE_HH_
#define CHECKPOINTHELPER_DEQUE_HH_

#include "CheckpointItem.hh"
#include <deque>

namespace CheckpointHelper
{

template<class T>
class deque: public CheckpointItem, public std::deque<T>
{
public:
   T ** chkpt;
   std::string mmstr;

   deque(T ** chkptIn, std::string mmstrIn) :
         chkpt(chkptIn), mmstr(mmstrIn)
   {
      *chkpt = 0x0;
   }

   deque<T>& operator=(const std::deque<T>& pr)
   {
      std::deque<T>::operator=(pr);
      return *this;
   }

   virtual ~deque()
   {
   }

   virtual void checkpoint()
   {
      chkpt_seq<typename std::deque<T>::iterator, T>(*this);
   }
   virtual void restart()
   {
      restart_seq<typename std::deque<T>::iterator>(*this);
   }
};

}

#if  (!defined(SWIG) && !defined(TRICK_ICG))
#define STLDEQUE( type, varName ) \
   type* _##varName; \
   CheckpointHelper::deque<type> varName
#else
#define STLDEQUE( type, varName ) \
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

#define STLDEQUECSTR STL_CSTR1

#endif /* STLHELPER_DEQUE_HH_ */

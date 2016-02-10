/*******************************************************************************

 PURPOSE:
 (Stack for checkpointable stack container.)

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (May 2013) (--)))

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_STACK_HH_
#define CHECKPOINTHELPER_STACK_HH_

#include "CheckpointItem.hh"
#include <stack>

namespace CheckpointHelper
{

template<class T>
class stack: public CheckpointItem, public std::stack<T>
{
public:
   T ** chkpt;
   std::string mmstr;

   stack(T ** chkptIn, std::string mmstrIn) :
         chkpt(chkptIn), mmstr(mmstrIn)
   {
      *chkpt = 0x0;
   }

   stack<T>& operator=(const std::stack<T>& pr)
   {
      std::stack<T>::operator=(pr);
      return *this;
   }

   virtual ~stack()
   {
   }

   virtual void checkpoint()
   {
      chkpt_stack<typename std::stack<T>, T>(*this);
   }
   virtual void restart()
   {
      restart_stack(*this);
   }
};

}

#if  (!defined(SWIG) && !defined(TRICK_ICG))
#define STLSTACK( type, varName ) \
   type* _##varName; \
   CheckpointHelper::stack<type> varName
#else
#define STLSTACK( type, varName ) \
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

#define STLSTACKCSTR STL_CSTR1

#endif /* STLHELPER_STACK_HH_ */

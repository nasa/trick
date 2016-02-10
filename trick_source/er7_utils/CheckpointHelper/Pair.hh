/*******************************************************************************

 PURPOSE:
 (Pair for checkpointable pair container.)

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (May 2013) (--)))

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_PAIR_HH_
#define CHECKPOINTHELPER_PAIR_HH_

#include "CheckpointItem.hh"
#include <utility>

namespace CheckpointHelper
{

template<class T, class U>
class pair: public CheckpointItem, public std::pair<T, U>
{
public:
   T * chkptFirst;
   U * chkptSecond;
   std::string mmstrKey;
   std::string mmstrValue;

   pair(T * chkptKeysIn, U * chkptValuesIn, std::string mmstrKeyIn, std::string mmstrValueIn) :
         chkptFirst(chkptKeysIn), chkptSecond(chkptValuesIn), mmstrKey(mmstrKeyIn), mmstrValue(mmstrValueIn)
   {
      *chkptFirst = 0x0;
      *chkptSecond = 0x0;
   }

   pair<T, U>& operator=(const std::pair<T, U>& pr)
   {
      std::pair<T, U>::operator=(pr);
      return *this;
   }

   virtual ~pair()
   {
   }

   virtual void checkpoint()
   {
      chkpt_pair(*this);
   }
   virtual void restart()
   {
      restart_pair(*this);
   }
};

}

#if  (!defined(SWIG) && !defined(TRICK_ICG))
#define STLPAIR( type1, type2, varName ) \
   type1 _##varName##T1; \
   type2 _##varName##T2; \
   CheckpointHelper::pair<type1, type2> varName
#else
#define STLPAIR( type1, type2, varName ) \
   type1 _##varName##T1; \
   type2 _##varName##T2;
#endif

#if  (!defined(SWIG) && !defined(TRICK_ICG))
#ifndef STL_CSTR2
#define STL_CSTR2( varName, type1, type2 ) \
   varName( &_##varName##T1, &_##varName##T2, #type1, #type2 )
#endif
#else
#ifndef STL_CSTR2
#define STL_CSTR2( varName, type1, type2 ) \
   _##varName##T1(), _##varName##T2()
#endif
#endif

#define STLPAIRCSTR STL_CSTR2

#endif /* STLHELPER_PAIR_HH_ */

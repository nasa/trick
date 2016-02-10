/*******************************************************************************

 PURPOSE:
 (Map for checkpointable map container.)

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (May 2013) (--)))

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_MAP_HH_
#define CHECKPOINTHELPER_MAP_HH_

#include "CheckpointItem.hh"
#include <map>

namespace CheckpointHelper
{

template<class T, class U>
class map: public CheckpointItem, public std::map<T, U>
{
public:
   T ** chkptKeys;
   U ** chkptValues;
   std::string mmstrKey;
   std::string mmstrValue;

   map(T ** chkptKeysIn, U **chkptValuesIn, std::string mmstrKeyIn, std::string mmstrValueIn) :
         chkptKeys(chkptKeysIn), chkptValues(chkptValuesIn), mmstrKey(mmstrKeyIn), mmstrValue(mmstrValueIn)
   {
      *chkptKeys = 0x0;
      *chkptValues = 0x0;
   }

   map<T, U>& operator=(const std::map<T, U>& pr)
   {
      std::map<T, U>::operator=(pr);
      return *this;
   }

   virtual ~map()
   {
   }

   virtual void checkpoint()
   {
      chkpt_map<typename std::map<T, U>::iterator, T, U>(*this);
   }
   virtual void restart()
   {
      restart_map<typename std::map<T, U>::iterator>(*this);
   }
};

template<class T, class U>
class multimap: public CheckpointHelper::CheckpointItem, public std::multimap<T, U>
{
public:
   T ** chkptKeys;
   U ** chkptValues;
   std::string mmstrKey;
   std::string mmstrValue;

   multimap(T ** chkptKeysIn, U **chkptValuesIn, std::string mmstrKeyIn, std::string mmstrValueIn) :
         chkptKeys(chkptKeysIn), chkptValues(chkptValuesIn), mmstrKey(mmstrKeyIn), mmstrValue(mmstrValueIn)
   {
      *chkptKeys = 0x0;
      *chkptValues = 0x0;
   }

   virtual ~multimap()
   {
   }

   virtual void checkpoint()
   {
      chkpt_map<typename std::multimap<T, U>::iterator, T, U>(*this);
   }
   virtual void restart()
   {
      restart_map<typename std::multimap<T, U>::iterator>(*this);
   }
};

}

#if  (!defined(SWIG) && !defined(TRICK_ICG))
#define STLMAP( type1, type2, varName ) \
   type1* _##varName##T1; \
   type2* _##varName##T2; \
   CheckpointHelper::map<type1, type2> varName
#define STLMULTIMAP( type1, type2, varName ) \
   type1* _##varName##T1; \
   type2* _##varName##T2; \
   CheckpointHelper::multimap<type1, type2> varName
#else
#define STLMAP( type1, type2,  varName ) \
   type1* _##varName##T1; \
   type2* _##varName##T2;
#define STLMULTIMAP( type1, type2, varName ) \
   type1* _##varName##T1; \
   type2* _##varName##T2;
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

#define STLMAPCSTR STL_CSTR2
#define STLMULTIMAPCSTR STL_CSTR2

#endif /* STLHELPER_MAP_HH_ */

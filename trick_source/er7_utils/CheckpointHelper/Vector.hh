/*******************************************************************************

 PURPOSE:
 (Vector for checkpointable vector container.)

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (May 2013) (--)))

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_VECTOR_HH_
#define CHECKPOINTHELPER_VECTOR_HH_

#include "CheckpointItem.hh"
#include "Unsafevector.hh"
#include <vector>

namespace CheckpointHelper
{

template<class T>
class vector: public CheckpointItem, public std::vector<T>
{
public:
   T ** chkpt;
   std::string mmstr;

   vector(T ** chkptIn, std::string mmstrIn) :
         chkpt(chkptIn), mmstr(mmstrIn)
   {
      *chkpt = 0x0;
   }

   vector<T>& operator=(const std::vector<T>& pr)
   {
      std::vector<T>::operator=(pr);
      return *this;
   }

   virtual ~vector()
   {
   }

   virtual void checkpoint()
   {
      chkpt_seq<typename std::vector<T>::iterator, T>(*this);
   }
   virtual void restart()
   {
      restart_seq<typename std::vector<T>::iterator>(*this);
   }
};

}

#if  (!defined(SWIG) && !defined(TRICK_ICG))
#define STLVECTOR( type, varName ) \
   type* _##varName; \
   CheckpointHelper::vector<type> varName
#else
#define STLVECTOR( type, varName ) \
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

#define STLVECTORCSTR STL_CSTR1

#endif /* STLHELPER_VECTOR_HH_ */

/*******************************************************************************

 PURPOSE:
 (Definition of Unsafevector container class. Minimal protections and
 forced-inlining for this container to acheive maximum performance..)

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (Apr 2014) (--)))

 ICG:(No.)

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_UNSAFECHKPTVECTOR_HH_
#define CHECKPOINTHELPER_UNSAFECHKPTVECTOR_HH_
#include "Unsafevector.hh"
#include "CheckpointItem.hh"

#define stlhelper_inline inline __attribute__((always_inline))
#define stlhelper_force_optimize __attribute__((optimize("-O3")))

//#define stlhelper_inline inline
//#define stlhelper_force_optimize

namespace CheckpointHelper
{

template<class T>
class UnsafeChkptVector: public Unsafevector<T>, public CheckpointItem
{
public:
   T ** chkpt;
   std::string mmstr;

   stlhelper_inline UnsafeChkptVector(T ** chkptIn, std::string mmstrIn) :
         chkpt(chkptIn), mmstr(mmstrIn)
   {
      *chkpt = 0x0;
   }

   stlhelper_inline UnsafeChkptVector(const UnsafeChkptVector<T> & other) :
         Unsafevector<T>(other), chkpt(other.chkpt), mmstr(other.mmstrIn)
   {
      *chkpt = 0x0;
   }

   virtual void checkpoint()
   {
      chkpt_unsafevec<T>(*this);
   }

   virtual void restart()
   {
      restart_unsafevec(*this);
   }
};

}

#if  (!defined(SWIG) && !defined(TRICK_ICG))
#define STLUNSAFECHKPTVECTOR( type, varName ) \
   type* _##varName; \
   CheckpointHelper::UnsafeChkptVector<type> varName
#else
#define STLUNSAFECHKPTVECTOR( type, varName ) \
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

#define STLUNSAFECHKPTVECTORCSTR STL_CSTR1

#endif /* UNSAFEVECTOR_HH_ */

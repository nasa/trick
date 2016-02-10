/*******************************************************************************

 PURPOSE:
 (DoublePtrCollect is a checkpointable dynamic collect to replace Trick's
 collect mechanism for collected "double *"'s.)

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (Aug 2012) (--)))

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_CHECKPOINTABLECOLLECT_HH_
#define CHECKPOINTHELPER_CHECKPOINTABLECOLLECT_HH_
#include "Vector.hh"
#include <algorithm>

namespace CheckpointHelper
{

class DoublePtrCollect
{
public:
   DoublePtrCollect() : STLVECTORCSTR( collect, double *)
   {
   }

   virtual ~DoublePtrCollect()
   {
   }

   void add_collect(double * vIn)
   {
#if  (!defined(SWIG) && !defined(TRICK_ICG))
      collect.push_back(vIn);
#endif
   }

   void remove_collect(double * vIn)
   {
#if  (!defined(SWIG) && !defined(TRICK_ICG))
      std::vector<double *>::iterator it = std::find(collect.begin(), collect.end(), vIn);
      if (it != collect.end())
      {
         collect.erase(it);
      }
#endif
   }

   STLVECTOR( double *, collect);
};

}

#endif /* STLHELPER_CHECKPOINTABLECOLLECT_HH_ */

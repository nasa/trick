/*******************************************************************************

 PURPOSE:
 (Definition of Unsafemap container class. Minimal protections and
 forced-inlining for this container to acheive maximum performance..)

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (Apr 2014) (--)))

 ICG:(No.)

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_UNSAFEMAP_HH_
#define CHECKPOINTHELPER_UNSAFEMAP_HH_
#include <stdlib.h>
#include "Unsafevector.hh"

namespace CheckpointHelper
{

template<class U, class T>
class Unsafemap
{
public:
   stlhelper_inline Unsafemap()
   {
   }

   stlhelper_inline ~Unsafemap()
   {
      int vsize = m.size();
      for (int ii = 0; ii < vsize; ++ii)
      {
         internal * it = m[ii];
         delete it;
      }
   }

   class internal
   {
   public:
      stlhelper_inline internal() :
            first(), second()
      {
      }
      stlhelper_inline internal(const U & firstIn) :
            first(firstIn), second()
      {
      }

      U first;
      T second;
   };

   Unsafevector<internal *> m;

   stlhelper_inline static int comparekeys(const void * a, const void * b)
   {
      U & key = *((U*) a);
      internal * bPtr = *((internal **) b);
      if (key < bPtr->first)
         return -1;
      if (key == bPtr->first)
         return 0;
      return 1;
   }

   stlhelper_inline static int compareelems(const void * a, const void * b)
   {
      internal * aPtr = *((internal **) a);
      internal * bPtr = *((internal **) b);
      if (aPtr->first < bPtr->first)
         return -1;
      if (aPtr->first == bPtr->first)
         return 0;
      return 1;
   }

   stlhelper_inline int size()
   {
      return m.size();
   }

   stlhelper_inline void clear()
   {
      for (int ii = 0, end = m.size(); ii != end; ++ii)
      {
         internal * Elem = m[ii];
         delete Elem;
      }
      m.clear();
   }

   stlhelper_inline T & getElement(int idx)
   {
      return m.v[idx]->second;
   }

#define compareKeys (int (*)( const void *, const void * )) Unsafemap<U, T>::comparekeys
#define compareElems (int (*)( const void *, const void * )) Unsafemap<U, T>::compareelems

   stlhelper_inline internal * find(const U & keyIn)
   {
      int res = searchForIndex(&keyIn);
      if (res == -1)
      {
         return 0x0;
      }
      else
      {
         return m[res];
      }
   }

   stlhelper_inline T & operator[](const U & keyIn)
   {
      int res = searchForIndex(&keyIn);
      if (res == -1)
      {
         internal * Elem = new internal(keyIn);
         m.addElement(Elem);
         qsort(m.v, m.size(), sizeof(internal *), compareElems);
         return Elem->second;
      }
      else
      {
         return m[res]->second;
      }
   }

   stlhelper_inline void remove(const U & keyIn)
   {
      int res = searchForIndex(&keyIn);
      if (res != -1)
      {
         m.removeElement(res);
      }
   }

protected:
   stlhelper_inline int searchForIndex(const U * keyIn)
   {
      size_t lowerIdx, upperIdx, currIdx;
      const void *currAddr;
      int comparResult;

      lowerIdx = 0;
      upperIdx = m.size();
      while (lowerIdx < upperIdx)
      {
         currIdx = (lowerIdx + upperIdx) / 2;
         currAddr = (void *) (((const char *) m.v) + (currIdx * sizeof(internal *)));
         comparResult = (*compareKeys )(keyIn, currAddr);
         if (comparResult < 0)
            upperIdx = currIdx;
         else if (comparResult > 0)
            lowerIdx = currIdx + 1;
         else
         {
            return currIdx;
         }
      }
      return -1;
   }

#undef compareKeys
#undef compareElems
};

}

#endif /* UNSAFEMAP_HH_ */

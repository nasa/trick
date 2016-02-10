/*******************************************************************************

 PURPOSE:
 (Definition of Unsafevector container class. Minimal protections and
 forced-inlining for this container to acheive maximum performance..)

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (Apr 2014) (--)))

 ICG:(No.)

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_UNSAFEVECTOR_HH_
#define CHECKPOINTHELPER_UNSAFEVECTOR_HH_

#include <cstring>

#ifndef DEBUG
#define stlhelper_inline inline __attribute__((always_inline))
#define stlhelper_force_optimize __attribute__((optimize("-O3")))
#else
#define stlhelper_inline
#define stlhelper_force_optimize
#endif

//#define stlhelper_inline inline
//#define stlhelper_force_optimize

#include <vector>
#include <cstdlib>

namespace CheckpointHelper
{

static const int minSize = 16;

template<class T>
class Unsafevector
{

public:

   stlhelper_inline Unsafevector() :
         v(new T[minSize]), numElems(0), capacity(minSize)
   {
   }

   stlhelper_inline Unsafevector(int newNumElems) :
         numElems(0), capacity(minSize)
   {
      while (capacity <= newNumElems)
      {
         capacity *= 2;
      }
      v = new T[capacity];
   }

   stlhelper_inline Unsafevector(const Unsafevector<T> & other) :
         numElems(other.numElems), capacity(other.numElems)
   {
      v = new T[other.numElems];
      for (int ii = 0; ii < other.numElems; ++ii)
      {
         v[ii] = other.v[ii];
      }
   }

   stlhelper_inline Unsafevector<T> & operator=(const Unsafevector<T> & other)
   {
      reserve_nocopy(other.numElems);
      for (int ii = 0; ii < other.numElems; ++ii)
      {
         v[ii] = other.v[ii];
      }
      capacity = other.numElems;
      numElems = other.numElems;
      return *this;
   }

   stlhelper_inline Unsafevector<T> & operator=(const std::vector<T> & other)
   {
      int vSize = other.size();
      reserve_nocopy(vSize);
      for (int ii = 0; ii < vSize; ++ii)
      {
         v[ii] = other[ii];
      }
      numElems = vSize;
      return *this;
   }

   stlhelper_inline ~Unsafevector()
   {
      delete[] v;
   }

   stlhelper_inline void reserve(int newSize)
   {
      if (newSize > capacity)
      {
         while (capacity <= newSize)
         {
            capacity *= 2;
         }
         T * newPtr = new T[capacity];
         for (int ii = 0; ii < numElems; ++ii)
         {
            newPtr[ii] = v[ii];
         }
         delete[] v;
         v = newPtr;
      }
   }

   stlhelper_inline void addVectorNoCheck(std::vector<T> & vecIn)
   {
      int vecInSize = vecIn.size();
      for (int ii = numElems, jj = 0; jj < vecInSize; ++ii, ++jj)
      {
         v[ii] = vecIn[jj];
      }
      numElems += vecInSize;
   }

   stlhelper_inline void addVector(std::vector<T> & vecIn)
   {
      int vecInSize = vecIn.size();
      reserve(numElems + vecInSize);
      for (int ii = numElems, jj = 0; jj < vecInSize; ++ii, ++jj)
      {
         v[ii] = vecIn[jj];
      }
      numElems += vecInSize;
   }

   stlhelper_inline void addVectorNoCheck(Unsafevector<T> & vecIn)
   {
      int vecInSize = vecIn.size();
      for (int ii = numElems, jj = 0; jj < vecInSize; ++ii, ++jj)
      {
         v[ii] = vecIn[jj];
      }
      numElems += vecInSize;
   }

   stlhelper_inline void addVector(Unsafevector<T> & vecIn)
   {
      int vecInSize = vecIn.size();
      reserve(numElems + vecInSize);
      for (int ii = numElems, jj = 0; jj < vecInSize; ++ii, ++jj)
      {
         v[ii] = vecIn[jj];
      }
      numElems += vecInSize;
   }

   stlhelper_inline void addElementNoCheck(const T & elemIn)
   {
      v[numElems++] = elemIn;
   }

   stlhelper_inline void addElement(const T & elemIn)
   {
      if (numElems >= capacity)
      {
         reserve(capacity * 2);
      }
      v[numElems++] = elemIn;
   }

   stlhelper_inline void removeElement(int idx)
   {
      if ((numElems - 1) != idx)
      {
         for (int ii = idx, end = numElems; ii != end; ++ii)
         {
            v[ii] = v[ii + 1];
         }
         --numElems;
      }
      else
      {
         --numElems;
      }
   }

   stlhelper_inline int find(const T & testElem)
   {
      for (int ii = 0, end = numElems; ii != end; ++ii)
      {
         if (v[ii] == testElem)
         {
            return ii;
         }
      }
      return -1;
   }

   stlhelper_inline void clear()
   {
      numElems = 0;
   }

   stlhelper_inline int size()
   {
      return numElems;
   }

   stlhelper_inline bool isEmpty()
   {
      return (numElems == 0);
   }

   stlhelper_inline T & operator[](int idx)
   {
      return v[idx];
   }
   stlhelper_inline T operator[](int idx) const
   {
      return v[idx];
   }

   stlhelper_inline void swap(Unsafevector<T> & other)
   {
      std::swap(v, other.v);
      std::swap(numElems, other.numElems);
      std::swap(capacity, other.capacity);
   }

   T * v;
   int numElems;
   int capacity;

protected:

   stlhelper_inline void reserve_nocopy(int newSize)
   {
      if (newSize > capacity)
      {
         delete[] v;
         v = new T[newSize];
         capacity = newSize;
      }
   }
};

template<class T>
class Unsafevector<T *>
{
public:

   stlhelper_inline Unsafevector() :
         v(new T *[minSize]), numElems(0), capacity(minSize)
   {
   }

   stlhelper_inline Unsafevector(int newNumElems) :
         numElems(0), capacity(minSize)
   {
      while (capacity <= newNumElems)
      {
         capacity *= 2;
      }
      v = new T *[capacity];
   }

   stlhelper_inline Unsafevector(const Unsafevector<T *> & other)
   {
      v = new T *[other.numElems];
      memcpy(v, other.v, sizeof(T *) * other.numElems);
      capacity = other.numElems;
      numElems = other.numElems;
   }

   stlhelper_inline Unsafevector<T *> & operator=(const Unsafevector<T *> & other)
   {
      reserve_nocopy(other.numElems);
      memcpy(v, other.v, sizeof(T *) * other.numElems);
      capacity = other.numElems;
      numElems = other.numElems;
      return *this;
   }

   stlhelper_inline Unsafevector<T *> & operator=(const std::vector<T *> & other)
   {
      int vSize = other.size();
      reserve_nocopy(vSize);
      memcpy(v, &other[0], sizeof(T *) * vSize);
      numElems = vSize;
      return *this;
   }

   stlhelper_inline ~Unsafevector()
   {
      delete[] v;
   }

   stlhelper_inline void reserve(int newSize)
   {
      if (newSize > capacity)
      {
         while (capacity <= newSize)
         {
            capacity *= 2;
         }
         T ** newPtr = new T *[capacity];
         memcpy(newPtr, v, sizeof(T *) * numElems);
         delete[] v;
         v = newPtr;
      }
   }

   stlhelper_inline void addVectorNoCheck(std::vector<T *> & vecIn)
   {
      int vecInSize = vecIn.size();
      if (vecInSize)
      {
         memcpy(&v[numElems], &vecIn[0], sizeof(T *) * vecInSize);
         numElems += vecInSize;
      }
   }

   stlhelper_inline void addVector(std::vector<T *> & vecIn)
   {
      int vecInSize = vecIn.size();
      if (vecInSize)
      {
         reserve(numElems + vecInSize);
         memcpy(&v[numElems], &vecIn[0], sizeof(T *) * vecInSize);
         numElems += vecInSize;
      }
   }

   stlhelper_inline void addVectorNoCheck(Unsafevector<T *> & vecIn)
   {
      int vecInSize = vecIn.size();
      if (vecInSize)
      {
         memcpy(&v[numElems], vecIn.v, sizeof(T *) * vecInSize);
         numElems += vecInSize;
      }
   }

   stlhelper_inline void addVector(Unsafevector<T *> & vecIn)
   {
      int vecInSize = vecIn.size();
      if (vecInSize)
      {
         reserve(numElems + vecInSize);
         memcpy(&v[numElems], vecIn.v, sizeof(T *) * vecInSize);
         numElems += vecInSize;
      }
   }

   stlhelper_inline void addElementNoCheck(T * elemIn)
   {
      v[numElems++] = elemIn;
   }

   stlhelper_inline void addElement(T * elemIn)
   {
      if (numElems >= capacity)
      {
         reserve(capacity * 2);
      }
      v[numElems++] = elemIn;
   }

   void addElementSortedUnique(T * elemIn)
   {
      for (int ii = 0; ii < numElems; ++ii)
      {
         if (elemIn == v[ii])
         {
            return;
         }
      }
      addElement(elemIn);
      sortSelf();
   }

   stlhelper_inline void removeElement(int idx)
   {
      memmove(&v[idx], &v[idx + 1], sizeof(T*) * (numElems - idx - 1));
      --numElems;
   }

   stlhelper_inline int find(const T * testElem)
   {
      for (int ii = 0, end = numElems; ii != end; ++ii)
      {
         if (v[ii] == testElem)
         {
            return ii;
         }
      }
      return -1;
   }

   stlhelper_inline void clear()
   {
      numElems = 0;
   }

   stlhelper_inline int size()
   {
      return numElems;
   }

   stlhelper_inline bool isEmpty()
   {
      return (numElems == 0);
   }

   stlhelper_inline T * & operator[](int idx)
   {
      return v[idx];
   }
   stlhelper_inline T * operator[](int idx) const
   {
      return v[idx];
   }

   stlhelper_inline void swap(Unsafevector<T *> & other)
   {
      std::swap(v, other.v);
      std::swap(numElems, other.numElems);
      std::swap(capacity, other.capacity);
   }

   stlhelper_inline static int compareelems(const void * a, const void * b)
   {
      T * aPtr = *((T **) a);
      T * bPtr = *((T **) b);
      if (aPtr < bPtr)
         return -1;
      if (aPtr == bPtr)
         return 0;
      return 1;
   }

   stlhelper_inline static int comparekey(const void * a, const void * b)
   {
      T * aPtr = (T *) a;
      T * bPtr = *((T **) b);
      if (aPtr < bPtr)
         return -1;
      if (aPtr == bPtr)
         return 0;
      return 1;
   }

   stlhelper_inline bool contains(const T & keyIn)
   {
      return (search(&keyIn) != 0x0);
   }

#define compareElems (int (*)( const void *, const void * )) Unsafevector<T *>::compareelems
#define compareKey (int (*)( const void *, const void * )) Unsafevector<T *>::comparekey

   stlhelper_inline void sortSelf()
   {
      std::qsort(v, numElems, sizeof(T *), compareElems);
   }

   stlhelper_inline void sortSelf(int (*compareIn)(const void *, const void *))
   {
      std::qsort(v, numElems, sizeof(T *), compareIn);
   }

   T ** v;
   int numElems;
   int capacity;

protected:
   stlhelper_inline T ** search(const T * keyIn)
   {
      size_t lowerIdx, upperIdx, currIdx;
      const void *currAddr;
      int comparResult;

      lowerIdx = 0;
      upperIdx = numElems;
      while (lowerIdx < upperIdx)
      {
         currIdx = (lowerIdx + upperIdx) / 2;
         currAddr = (void *) (((const char *) v) + (currIdx * sizeof(T *)));
         comparResult = (*compareKey )(keyIn, currAddr);
         if (comparResult < 0)
            upperIdx = currIdx;
         else if (comparResult > 0)
            lowerIdx = currIdx + 1;
         else
            return (T **) currAddr;
      }

      return NULL;
   }

   stlhelper_inline void reserve_nocopy(int newSize)
   {
      if (newSize > capacity)
      {
         delete[] v;
         v = new T *[newSize];
         capacity = newSize;
      }
   }
#undef compareElems
#undef compareKey
};

}

#endif /* UNSAFEVECTOR_HH_ */

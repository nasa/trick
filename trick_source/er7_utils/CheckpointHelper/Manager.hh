/*******************************************************************************

 PURPOSE:
 (Manager for globally calling checkpoint and restart methods of STLs)

 LIBRARY DEPENDENCY:
 ((Manager.cpp))

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (May 2013) (--)))

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_MANAGER_HH_
#define CHECKPOINTHELPER_MANAGER_HH_

#include <deque>
#include <string>
#include <map>
#include <list>
#include <pthread.h>

namespace CheckpointHelper
{

class CheckpointItem;

class Manager
{
   friend class InputProcessor;
   friend void init_attrCheckpointHelper__Manager();
public:
   Manager();
   virtual ~Manager();

   class AllocEntry
   {
   public:
      AllocEntry(void * parentPtrIn, void * ptrIn, const std::string nameIn, const std::string funcNameIn);
      void * parentPtr;
      void * ptr;
      std::string name;
      std::string funcName;

      bool operator==(void * ptrComp)
      {
         return this->ptr == ptrComp;
      }
   };

   void addConstructorAlloc(void * parentPtr, void * ptr, const std::string name, const std::string funcNameIn);
   void removeConstructorAlloc(void * ptr);
   void processConstructorAllocs();

   void checkpoint();
   void restart();

   void addItem(CheckpointItem & stlIn);
   void removeItem(CheckpointItem & stlIn);

   void addExternAlloc(void * ptr, size_t numElems);
   void removeExternAlloc(void * ptr);
   size_t getExternNum(void * ptr);

protected:
   std::list<CheckpointItem *> checkpointObjects;
   CheckpointItem ** _checkpointObjects;
   std::deque<AllocEntry> constructorAllocs;
   pthread_mutex_t allocQueueMutex; /* ** Do not checkpoint */
   pthread_mutex_t objListMutex; /* ** Do not checkpoint */
   pthread_mutex_t externMapMutex; /* ** Do not checkpoint */
   std::map<void *, size_t> externMemAllocs;

};

}

#endif /* STLHELPER_MANAGER_HH_ */

/*******************************************************************************

 PURPOSE:
 (Manager for globally calling checkpoint and restart methods of STLs)

 LIBRARY DEPENDENCY:
 ((Manager.cpp)
 (CheckpointItem.cpp))

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (May 2013) (--)))

 *******************************************************************************/

#include "Alloc.hh"
#include "Manager.hh"
#if (defined TRICK_VER) && (TRICK_VER >= 10)
#include "sim_services/Message/include/message_proto.h"
#endif
#include "CheckpointItem.hh"
#include <algorithm>
#include <iostream>

namespace CheckpointHelper
{

Manager * helperManager = 0x0;

Manager::Manager()
{
   _checkpointObjects = NULL;
   if (helperManager == 0x0)
   {
      helperManager = this;
   }
   else
   {
      std::cerr << "WARNING: Only one instance of CheckpointHelper::Manager is allowed. Invalid Manager instance at  "
            << this << std::endl;
   }
   pthread_mutex_init(&allocQueueMutex, 0x0);
   pthread_mutex_init(&objListMutex, 0x0);
   pthread_mutex_init(&externMapMutex, 0x0);
}

Manager::~Manager()
{
   if (helperManager != 0x0)
   {
      helperManager = 0x0;
   }
}
#if (defined TRICK_VER) && (TRICK_VER >= 10)

void Manager::checkpoint()
{
   processConstructorAllocs();
   pthread_mutex_lock(&objListMutex);
   int numObjs = checkpointObjects.size();
   if (numObjs)
   {
      alloc<CheckpointItem *>(this, &_checkpointObjects, numObjs, "CheckpointHelper::CheckpointItem", "", false,
      CHECKPOINTHELPER_FILELINE);
      int ii = 0;
      for (std::list<CheckpointItem *>::iterator it = checkpointObjects.begin(), end = checkpointObjects.end();
            it != end; ++it, ++ii)
      {
         CheckpointItem * item = *it;
         item->checkpoint();
         _checkpointObjects[ii] = item;
      }
   }
   pthread_mutex_unlock(&objListMutex);
}
void Manager::restart()
{
   pthread_mutex_lock(&allocQueueMutex);
   constructorAllocs.clear();
   pthread_mutex_unlock(&allocQueueMutex);
   pthread_mutex_lock(&objListMutex);
   checkpointObjects.clear();
   if (_checkpointObjects != NULL)
   {
      int numObjs = trick_MM->get_size((char *) _checkpointObjects);
      for (int ii = 0; ii < numObjs; ++ii)
      {
         CheckpointItem * item = _checkpointObjects[ii];
         checkpointObjects.push_back(_checkpointObjects[ii]);
         item->restart();
      }
   }
   pthread_mutex_unlock(&objListMutex);
}

#else
void Manager::checkpoint()
{
}
void Manager::restart()
{
}

#endif

void Manager::addItem(CheckpointItem & stlIn)
{
   pthread_mutex_lock(&objListMutex);
   std::list<CheckpointItem *>::iterator it = find(checkpointObjects.begin(), checkpointObjects.end(), &stlIn);
   if (it == checkpointObjects.end())
   {
      checkpointObjects.push_back(&stlIn);
   }
   pthread_mutex_unlock(&objListMutex);
}

void Manager::removeItem(CheckpointItem & stlIn)
{
   pthread_mutex_lock(&objListMutex);
   std::list<CheckpointItem *>::iterator it = find(checkpointObjects.begin(), checkpointObjects.end(), &stlIn);
   if (it != checkpointObjects.end())
   {
      checkpointObjects.erase(it);
   }
   pthread_mutex_unlock(&objListMutex);
}

Manager::AllocEntry::AllocEntry(void * parentPtrIn, void * ptrIn, const std::string nameIn,
      const std::string funcNameIn) :
      parentPtr(parentPtrIn), ptr(ptrIn), name(nameIn), funcName(funcNameIn)
{
}

void Manager::addConstructorAlloc(void* parentPtr, void* ptr, const std::string name, const std::string funcNameIn)
{
   pthread_mutex_lock(&allocQueueMutex);
   constructorAllocs.push_back(AllocEntry(parentPtr, ptr, name, funcNameIn));
   pthread_mutex_unlock(&allocQueueMutex);
}

void Manager::removeConstructorAlloc(void * ptrIn)
{
   pthread_mutex_lock(&allocQueueMutex);
   std::deque<AllocEntry>::iterator it = std::find(constructorAllocs.begin(), constructorAllocs.end(), ptrIn);
   if (it != constructorAllocs.end())
   {
      constructorAllocs.erase(it);
   }
   pthread_mutex_unlock(&allocQueueMutex);
}

void Manager::processConstructorAllocs()
{
   pthread_mutex_lock(&allocQueueMutex);
   std::deque<AllocEntry> preQueue;
   std::deque<AllocEntry> postQueue;
   while (!constructorAllocs.empty())
   {
#if (defined TRICK_VER) && (TRICK_VER >= 10)
      AllocEntry & entry = constructorAllocs.front();

      if (!updateAllocName(entry.parentPtr, entry.ptr, entry.name, entry.funcName, false))
      {
         postQueue.push_back(AllocEntry(entry.parentPtr, entry.ptr, entry.name, entry.funcName));
      }
#endif
      constructorAllocs.pop_front();
   }
   pthread_mutex_unlock(&allocQueueMutex);

   size_t preSize = preQueue.size();
   size_t postSize = postQueue.size();

   while (preSize != postSize)
   {
      preQueue = postQueue;
      preSize = preQueue.size();
      postQueue.clear();
      while (!preQueue.empty())
      {
#if (defined TRICK_VER) && (TRICK_VER >= 10)
         AllocEntry & entry = preQueue.front();

         if (!updateAllocName(entry.parentPtr, entry.ptr, entry.name, entry.funcName, false))
         {
            postQueue.push_back(AllocEntry(entry.parentPtr, entry.ptr, entry.name, entry.funcName));
         }
#endif
         preQueue.pop_front();
      }
      postSize = postQueue.size();
   }

   while (!postQueue.empty())
   {
#if (defined TRICK_VER) && (TRICK_VER >= 10)
      AllocEntry & entry = postQueue.front();

      updateAllocName(entry.parentPtr, entry.ptr, entry.name, entry.funcName, true);
#endif
      postQueue.pop_front();
   }
}

void Manager::addExternAlloc(void* ptr, size_t numElems)
{
   pthread_mutex_lock(&externMapMutex);
   externMemAllocs[ptr] = numElems;
   pthread_mutex_unlock(&externMapMutex);
}

void Manager::removeExternAlloc(void* ptr)
{
   pthread_mutex_lock(&externMapMutex);
   externMemAllocs.erase(ptr);
   pthread_mutex_unlock(&externMapMutex);
}

size_t Manager::getExternNum(void* ptr)
{
   pthread_mutex_lock(&externMapMutex);
   size_t num;
   std::map<void *, size_t>::iterator it = externMemAllocs.find(ptr);
   if (it != externMemAllocs.end())
   {
      num = it->second;
   }
   else
   {
      num = 0;
   }
   pthread_mutex_unlock(&externMapMutex);
   return num;
}

}


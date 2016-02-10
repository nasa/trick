/*******************************************************************************

 PURPOSE:
 (CheckpointItem is the abstract class to be used for automatic registration and
 checkpointing of items. This package provides STL container equivalents deriving
 from this classn.)

 LIBRARY DEPENDENCY:
 ((CheckpointItem.cpp))

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (May 2013) (--)))

 *******************************************************************************/

#ifndef CHECKPOINTHELPER_CHECKPOINTITEM_HH_
#define CHECKPOINTHELPER_CHECKPOINTITEM_HH_

#include <queue>
#include <stack>
#include "Alloc.hh"

namespace CheckpointHelper
{

class Manager;

extern Manager * helperManager;

class CheckpointItem
{
   friend class InputProcessor;
   friend void init_attrCheckpointHelper__CheckpointItem();
public:
   CheckpointItem();
   virtual ~CheckpointItem();

   virtual void checkpoint() = 0;
   virtual void restart() = 0;
};

#if (defined TRICK_VER) && (TRICK_VER >= 10)
template<typename U, class V, class T>
void chkpt_seq(T & obj)
{
   int numObjs = obj.size();
   if (numObjs)
   {
      CheckpointHelper::alloc<V>(0x0, obj.chkpt, obj.size(), obj.mmstr, "", false, CHECKPOINTHELPER_FILELINE);
      int ii = 0;
      for (U it = obj.begin(), end = obj.end(); it != end; ++it, ++ii)
      {
         (*obj.chkpt)[ii] = *it;
      }
   }
}

template<class V, class T>
void chkpt_unsafevec(T & obj)
{
   int numObjs = obj.size();
   if (numObjs)
   {
      CheckpointHelper::alloc<V>(0x0, obj.chkpt, obj.size(), obj.mmstr, "", false, CHECKPOINTHELPER_FILELINE);
      for (int ii = 0; ii != numObjs; ++ii)
      {
         (*obj.chkpt)[ii] = obj[ii];
      }
   }
}

template<typename U, class T>
void restart_seq(T & obj)
{
   obj.clear();
   if ((*obj.chkpt) != NULL)
   {
      int numObjs = trick_MM->get_size((char *) (*obj.chkpt));
      for (int ii = 0; ii < numObjs; ++ii)
      {
         U it = obj.end();
         obj.insert(it, (*obj.chkpt)[ii]);
      }
   }
}

template<class T>
void restart_unsafevec(T & obj)
{
   obj.clear();
   if ((*obj.chkpt) != 0x0)
   {
      int numObjs = trick_MM->get_size((char *) (*obj.chkpt));
      for (int ii = 0; ii != numObjs; ++ii)
      {
         obj.addElement((*obj.chkpt)[ii]);
      }
   }
}

template<typename U, class V, class T>
void chkpt_queue(T & obj)
{
   U temp_queue(obj);
   int numObjs = temp_queue.size();
   if (numObjs)
   {
      CheckpointHelper::alloc<V>(0x0, obj.chkpt, temp_queue.size(), obj.mmstr, "", false, CHECKPOINTHELPER_FILELINE);
      for (int ii = 0, end = temp_queue.size(); ii != end; ++ii)
      {
         (*obj.chkpt)[ii] = temp_queue.front();
         temp_queue.pop();
      }
   }
}

template<class T>
void restart_queue(T & obj)
{
   while (!obj.empty())
   {
      obj.pop();
   }
   if ((*obj.chkpt) != NULL)
   {
      int numObjs = trick_MM->get_size((char *) (*obj.chkpt));
      for (int ii = 0; ii < numObjs; ++ii)
      {
         obj.push((*obj.chkpt)[ii]);
      }
   }
}

template<typename U, class V, class T>
void chkpt_stack(T & obj)
{
   U temp_stack(obj);
   int numObjs = temp_stack.size();
   if (numObjs)
   {
      CheckpointHelper::alloc<V>(0x0, obj.chkpt, temp_stack.size(), obj.mmstr, "", false, CHECKPOINTHELPER_FILELINE);
      for (int ii = 0, end = temp_stack.size(); ii != end; ++ii)
      {
         (*obj.chkpt)[ii] = temp_stack.top();
         temp_stack.pop();
      }
   }
}

template<class T>
void restart_stack(T & obj)
{
   while (!obj.empty())
   {
      obj.pop();
   }
   if ((*obj.chkpt) != NULL)
   {
      int numObjs = trick_MM->get_size((char *) (*obj.chkpt));
      for (int ii = numObjs - 1; ii != -1; --ii)
      {
         obj.push((*obj.chkpt)[ii]);
      }
   }
}

template<class T>
void chkpt_pair(T & obj)
{
   (*obj.chkptFirst) = obj.first;
   (*obj.chkptSecond) = obj.second;
}

template<class T>
void restart_pair(T & obj)
{
   obj.first = (*obj.chkptFirst);
   obj.second = (*obj.chkptSecond);
}

template<typename U, class V, class X, class T>
void chkpt_map(T &obj)
{
   int numObjs = obj.size();
   if (numObjs)
   {
      CheckpointHelper::alloc<V>(0x0, obj.chkptKeys, obj.size(), obj.mmstrKey, "", false, CHECKPOINTHELPER_FILELINE);
      CheckpointHelper::alloc<X>(0x0, obj.chkptValues, obj.size(), obj.mmstrValue, "", false,
      CHECKPOINTHELPER_FILELINE);
      int ii = 0;
      for (U it = obj.begin(), end = obj.end(); it != end; ++it, ++ii)
      {
         (*obj.chkptKeys)[ii] = it->first;
         (*obj.chkptValues)[ii] = it->second;
      }
   }
}

template<typename U, class T>
void restart_map(T &obj)
{
   obj.clear();
   if ((*obj.chkptKeys) != NULL)
   {
      int numObjs = trick_MM->get_size((char*) (*obj.chkptKeys));
      for (int ii = 0; ii < numObjs; ++ii)
      {
         obj.insert(std::make_pair((*obj.chkptKeys)[ii], (*obj.chkptValues)[ii]));
      }
   }
}
#else
template<typename U, class V, class T>
void chkpt_seq( T & obj )
{
   (void) obj;
}

template<class V, class T>
void chkpt_unsafevec( T & obj )
{
   (void) obj;
}

template<typename U, class T>
void restart_seq( T & obj )
{
   (void) obj;
}

template<class T>
void restart_unsafevec( T & obj )
{
   (void) obj;
}

template<typename U, class V, class T>
void chkpt_queue( T & obj )
{
   (void) obj;
}

template<class T>
void restart_queue( T & obj )
{
   (void) obj;
}

template<typename U, class V, class T>
void chkpt_stack( T & obj )
{
   (void) obj;
}

template<class T>
void restart_stack( T & obj )
{
   (void) obj;
}

template<class T>
void chkpt_pair( T & obj )
{
   (void) obj;
}

template<class T>
void restart_pair( T & obj )
{
   (void) obj;
}

template<typename U, class V, class X, class T>
void chkpt_map( T & obj )
{
   (void) obj;
}

template<typename U, class T>
void restart_map( T &obj )
{
   (void) obj;
}
#endif
}

#endif /* STLHELPER_STL_HH_ */

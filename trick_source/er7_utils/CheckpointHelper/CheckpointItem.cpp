/*******************************************************************************

 PURPOSE:
 (CheckpointItem is the abstract class to be used for automatic registration and
 checkpointing of items. This package provides STL container equivalents deriving
 from this classn.)

 LIBRARY DEPENDENCY:
 ((CheckpointItem.cpp)
 (Manager.cpp))

 PROGRAMMERS:
 (((Thomas Brain) (Metecs) (May 2013) (--)))

 *******************************************************************************/

#include "Manager.hh"
#include "CheckpointItem.hh"

namespace CheckpointHelper
{

CheckpointItem::CheckpointItem()
{
   if (helperManager != NULL)
   {
      helperManager->addItem(*this);
   }
}

CheckpointItem::~CheckpointItem()
{
   if (helperManager != NULL)
   {
      helperManager->removeItem(*this);
   }
}

}

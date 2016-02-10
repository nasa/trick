/*
 * Alloc.cpp
 *
 *  Created on: Aug 24, 2015
 *      Author: tbrain
 */

#include "Alloc.hh"

namespace CheckpointHelper
{
#if (defined TRICK_VER) && (TRICK_VER >= 10)
std::string treatAllocName(const std::string & nameIn)
{
   std::string treatedName(nameIn);

   // Always remove "&"
   std::size_t found = treatedName.find('&');
   if (found != std::string::npos)
   {
      treatedName.erase(0, 1);
   }

   found = treatedName.find('.');
   while (found != std::string::npos)
   {
      treatedName.replace(found, 1, "_");
      found = treatedName.find('.');
   }
   found = treatedName.find('[');
   while (found != std::string::npos)
   {
      treatedName.replace(found, 1, "_");
      found = treatedName.find('[');
   }
   found = treatedName.find(']');
   while (found != std::string::npos)
   {
      treatedName.replace(found, 1, "_");
      found = treatedName.find(']');
   }
   found = treatedName.find(' ');
   while (found != std::string::npos)
   {
      treatedName.replace(found, 1, "_");
      found = treatedName.find(' ');
   }
   found = treatedName.find('+');
   while (found != std::string::npos)
   {
      treatedName.replace(found, 1, "_");
      found = treatedName.find('+');
   }
   found = treatedName.find(':');
   while (found != std::string::npos)
   {
      treatedName.replace(found, 1, "_");
      found = treatedName.find(':');
   }
   found = treatedName.find('(');
   while (found != std::string::npos)
   {
      treatedName.replace(found, 1, "_");
      found = treatedName.find('(');
   }
   found = treatedName.find(')');
   while (found != std::string::npos)
   {
      treatedName.replace(found, 1, "_");
      found = treatedName.find(')');
   }

   // Check for linked-list, we want to prevent very long names
   found = treatedName.rfind("AUTOLIST");
   if (found == std::string::npos)
   {
      found = treatedName.rfind("_0__");
      if (found != std::string::npos)
      {
         size_t newParamNamePos = found + 4;
         std::string newParamName = treatedName.substr(newParamNamePos);
         size_t remChars = treatedName.length() - newParamNamePos;
         if (remChars <= found)
         {
            std::string prevParamName = treatedName.substr(found - remChars, remChars);
            if (prevParamName.compare(newParamName) == 0)
            {
               treatedName = treatedName.substr(0, found);
               treatedName += "_AUTOLIST1";
            }
         }
      }
   }
   else
   {
      size_t afterListPos = treatedName.rfind("_0__");
      std::stringstream iss(treatedName.substr(found + 8, afterListPos));
      int currIdx = 0;
      iss >> currIdx;
      ++currIdx;
      iss.str("");
      iss << currIdx;
      treatedName = treatedName.substr(0, found + 8);
      treatedName += iss.str();
   }

   return treatedName;
}

bool updateAllocName(void * parentPtr, void * ptr, const std::string & name, const std::string & funcName,
      bool printError)
{
   bool success = false;
   if (parentPtr != 0x0)
   {
      ALLOC_INFO * info = trick_MM->get_alloc_info_of(parentPtr);
      if (info != 0x0)
      {
         if (info->name != 0x0)
         {
            std::string pathName = trick_MM->ref_name_from_address(parentPtr);
            pathName = treatAllocName(pathName);
            std::stringstream ss;
            std::string treatedName = treatAllocName(name);
            ss << pathName << "_" << treatedName;
            trick_MM->set_name_at(ptr, ss.str().c_str());
            success = true;
         }
         else
         {
            if (printError)
            {
               if (ptr == 0x0)
               {
                  std::stringstream ss;
                  ss << "Could not name an allocation with name " << name << " because the allocated ptr is NULL. "
                        << "Allocation call invoked from " << funcName << std::endl;
                  message_publish(1, ss.str().c_str());
               }
               else
               {
                  std::stringstream ss;
                  ss << "Could not name the allocation at " << ptr << " because it's parent data structure at "
                        << parentPtr
                        << " has no name registered with Trick MemoryManager. Allocation call invoked from " << funcName
                        << std::endl;
                  message_publish(1, ss.str().c_str());
               }
            }
         }
      }
      else
      {
         if (printError)
         {
            if (ptr == 0x0)
            {
               std::stringstream ss;
               ss << "Could not name an allocation with name " << name << " because the allocated ptr is NULL. "
                     << "Allocation call invoked from " << funcName << std::endl;
               message_publish(1, ss.str().c_str());
            }
            else
            {
               std::stringstream ss;
               ss << "Could not name the allocation at " << ptr << " because it's parent data structure at "
                     << parentPtr << " was not found by Trick MemoryManagerAllocation call invoked from " << funcName
                     << std::endl;
               message_publish(1, ss.str().c_str());
            }
         }
      }
   }
   else
   {
      if (printError)
      {
         if (ptr == 0x0)
         {
            std::stringstream ss;
            ss << "Could not name an allocation with name  " << name
                  << " because the parent and allocated pointers are NULL. " << "Allocation call invoked from "
                  << funcName << std::endl;
            message_publish(1, ss.str().c_str());
         }
         else
         {
            std::stringstream ss;
            ss << "Could not name the allocation at " << ptr << " because it's parent pointer is NULL. "
                  << "Allocation call invoked from " << funcName << std::endl;
            message_publish(1, ss.str().c_str());
         }
      }
   }
   return success;
}
#endif
}

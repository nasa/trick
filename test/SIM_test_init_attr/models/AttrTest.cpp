#include "trick/tc_proto.h"
#include "trick/message_proto.h"
#include "trick/AttributesMap.hh"
#include "AttrTest.hh"

void AttrTest::initialization() {
   // If there is an instance of Outer declared in the header or S_define, then the init_attrOuter() function
   // is automatically called. 
   // If there is no instance of Outer declared in the header or S_define, then the init_attrOuter() function
   // is not automatically called.
   // The init_attr merge request (PR# 2166) fixes this issue so that the init_attrOuter() function
   // is always called, regardless of whether there is an instance of Outer declared in the header/S_define or not.
   // Without PR# 2166 and there is no instance of Outer declared in the header/S_define, the following commented
   // out code is required for the call to trick_bswap_buffer() to work properly. 
/*
   extern void init_attrOuter() ;
   init_attrOuter() ;
*/
}

void AttrTest::scheduled() {
   Outer o;
   unsigned char buffer[sizeof(Outer)];

   message_publish(MSG_NORMAL,"calling byte swap\n");
   trick_bswap_buffer(&buffer, &o, Trick::AttributesMap::attributes_map()->get_attr("Outer"),1);
   return;
}


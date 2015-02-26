/*
 * $Id: message_proto.h 1629 2011-05-19 21:38:40Z dstrauss $
 */

#ifndef _MM_MESSAGE_PROTO_H_
#define _MM_MESSAGE_PROTO_H_

#include "sim_services/Message/include/message_proto.h"

// when you want to use the MemoryManager standalone, use this #define
// otherwise the normal message_publish is in a file that includes the MessagePublisher and Executive
#ifndef TRICK_VER
#define message_publish message_publish_standalone
#endif

#endif


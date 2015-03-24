#include "sim_services/MemoryManager/include/MemoryManager.hh"
#include "sim_services/Message/include/message_proto.h"
#include "sim_services/Message/include/message_type.h"

Trick::CheckPointAgent * Trick::MemoryManager::get_CheckPointAgent() {
    return currentCheckPointAgent ;
}

void Trick::MemoryManager::set_CheckPointAgent(CheckPointAgent* agent) {

    if (agent != NULL) {
        currentCheckPointAgent = agent;
    } else {
        message_publish(MSG_ERROR, "Memory Manager ERROR: Attempt to set CheckPointAgent to NULL.\n") ;
    }
    return;
}

void Trick::MemoryManager::reset_CheckPointAgent() {

    currentCheckPointAgent = defaultCheckPointAgent;
    return;
}


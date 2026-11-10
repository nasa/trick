#include "trick/MemoryManager.hh"
#include "trick/ClassicCheckPointAgent.hh"

/**
 @par Detailed Description:
 */
std::string Trick::MemoryManager::ref_name_from_address( void* address) {

    std::string result;
    bool saved_write_state = checkpoint_write_success;

    Trick::ClassicCheckPointAgent* agent = dynamic_cast<Trick::ClassicCheckPointAgent*>(defaultCheckPointAgent);
    if (agent) {
        result = agent->ref_string_from_ptr( address, NULL, 0);
    } else {
        result.clear();
    }
    // Errors from reference lookups are not checkpoint write errors.
    checkpoint_write_success = saved_write_state;
    return (result);
}

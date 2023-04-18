#ifndef MOCK_MESSAGE_CUSTOM_MANAGER_HH
#define MOCK_MESSAGE_CUSTOM_MANAGER_HH

#include "trick/MessageCustomManager.hh"
#include <gmock/gmock.h>

class MockMessageCustomManager : public Trick::MessageCustomManager {
    public: 
        MOCK_METHOD3(update, void(int level,  std::string header, std::string message));
        MOCK_METHOD3(open_custom_message_file, int(std::string file_name, std::string subscriber_name, int level));
};

#endif
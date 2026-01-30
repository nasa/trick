#ifndef MOCK_MESSAGE_PUBLISHER_HH
#define MOCK_MESSAGE_PUBLISHER_HH

#include "trick/MessagePublisher.hh"
#include <gmock/gmock.h>

class MockMessagePublisher : public Trick::MessagePublisher
{
public:
    MOCK_METHOD2(publish, int(int level, std::string message));
};

#endif

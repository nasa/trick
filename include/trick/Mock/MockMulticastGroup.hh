#ifndef MOCK_MULTICAST_GROUP_HH
#define MOCK_MULTICAST_GROUP_HH

#include "trick/MulticastGroup.hh"

#include <gmock/gmock.h>

class MockMulticastGroup : public Trick::MulticastGroup
{
public:
    MOCK_METHOD1(broadcast, int(std::string));
    MOCK_METHOD2(addAddress, int(std::string, int));
    MOCK_METHOD0(restart, int());
    MOCK_METHOD0(isInitialized, bool());
    MOCK_METHOD0(initialize, int());
    MOCK_METHOD0(disconnect, int());
};

#endif

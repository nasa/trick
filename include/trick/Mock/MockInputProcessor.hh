#ifndef MOCK_INPUT_PROCESSOR_HH
#define MOCK_INPUT_PROCESSOR_HH

#include "trick/InputProcessor.hh"
#include <gmock/gmock.h>


class MockInputProcessor : public Trick::InputProcessor {
    public: 
        MOCK_METHOD1(parse, int(std::string in_string));
};

#endif
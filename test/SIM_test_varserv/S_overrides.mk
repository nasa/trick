TRICK_CFLAGS += -I./models
TRICK_CXXFLAGS += -I./models -I$(GTEST_HOME)/include -std=c++11

all: test_client
clean: clean_test_client

TEST_CLIENT_LIBS += -L${GTEST_HOME}lib64 -L${GTEST_HOME}lib -lgtest -lgtest_main -lpthread

test_client: models/test_client/test_client.cpp
	cd models/test_client; $(TRICK_CXX) test_client.cpp $(TRICK_SYSTEM_LDFLAGS) $(TRICK_CXXFLAGS) -o test_client $(TEST_CLIENT_LIBS) -std=c++11

clean_test_client:
	rm -f models/test_client/test_client

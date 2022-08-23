
TRICK_CFLAGS += -I./models
TRICK_CXXFLAGS += -I./models

all: test_client
clean: clean_test_client

TEST_CLIENT_LIBS += -L${GTEST_HOME}/lib64 -L${GTEST_HOME}/lib -lgtest -lgtest_main -lpthread -L${TRICK_LIB_DIR} -ltrick_var_binary_parser

test_client: models/test_client/test_client.cpp 
	cd models/test_client; $(TRICK_CXX) test_client.cpp -o test_client $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) $(TRICK_TEST_FLAGS) -Wno-write-strings -Wno-sign-compare -I$(TRICK_HOME)/include $(TEST_CLIENT_LIBS)

clean_test_client:
	rm -f models/test_client/test_client
	

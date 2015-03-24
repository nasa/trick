
#include <gtest/gtest.h>

#include "trick_utils/comm/include/tc.h"
#include "trick_utils/comm/include/attributes.h"
#include "trick_utils/comm/include/hs_msg.h"
#include "trick_utils/comm/include/tc_proto.h"
#include "trick_utils/comm/include/trick_byteswap.h"
#include "trick_utils/comm/include/trick_error_hndlr.h"

class TCPendingTest : public testing::Test {

   protected:
      TCPendingTest(){}
      ~TCPendingTest(){}

      TCDevice* device;

      void SetUp(){

         device = (TCDevice *) malloc(sizeof(TCDevice));
         memset( (void *)device,'\0',sizeof(TCDevice) );

         device->socket = 1;
      }

      void TearDown(){
   
         free(device);
      }
};

TEST_F( TCPendingTest, testNullDevice ) {

   int tcpending_status = tc_pending( NULL );

   EXPECT_EQ( tcpending_status, -1 );
} 

/*
TEST_F( TCPendingTest, testInvalidSocket ) {

   device->socket = TRICKCOMM_INVALID_SOCKET;

   int tcpending_status = tc_pending( device );

   EXPECT_EQ( tcpending_status, 0 );
} 
*/

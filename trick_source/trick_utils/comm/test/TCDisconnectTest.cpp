
#include <gtest/gtest.h>

#include "trick_utils/comm/include/tc.h"
#include "trick_utils/comm/include/attributes.h"
#include "trick_utils/comm/include/hs_msg.h"
#include "trick_utils/comm/include/tc_proto.h"
#include "trick_utils/comm/include/trick_byteswap.h"
#include "trick_utils/comm/include/trick_error_hndlr.h"


class TCDisconnectTest : public testing::Test {

   protected:
      TCDisconnectTest(){}
      ~TCDisconnectTest(){}

      TCDevice* device;

      void SetUp(){

         device = (TCDevice *) malloc(sizeof(TCDevice));
         memset( (void *)device,'\0',sizeof(TCDevice) );
      }

      void TearDown(){
   
         free(device);
      }
};

TEST_F( TCDisconnectTest, testNoDevice ) {

   int disconnect_status = tc_disconnect( NULL );

   EXPECT_EQ( disconnect_status, TC_DRIVER_ALREADY_DISCONNECTED );
} 

TEST_F( TCDisconnectTest, testSuccess ) {

   int disconnect_status = tc_disconnect( device );

   EXPECT_EQ( disconnect_status, TC_SUCCESS );
} 

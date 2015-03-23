
#include <gtest/gtest.h>

#include "trick_utils/comm/include/tc.h"
#include "trick_utils/comm/include/attributes.h"
#include "trick_utils/comm/include/hs_msg.h"
#include "trick_utils/comm/include/tc_proto.h"
#include "trick_utils/comm/include/trick_byteswap.h"
#include "trick_utils/comm/include/trick_error_hndlr.h"

class TCSetBlockIOTest : public testing::Test {

   protected:
      TCSetBlockIOTest(){}
      ~TCSetBlockIOTest(){}

      TCDevice* device;

      void SetUp(){

         device = (TCDevice *) malloc(sizeof(TCDevice));
         memset( (void *)device,'\0',sizeof(TCDevice) );
      }

      void TearDown(){
   
         free(device);
      }
};

TEST_F( TCSetBlockIOTest, testNoDevice ) {

   int set_blockio_status = tc_set_blockio_timeout_limit( NULL, 0 );

   EXPECT_EQ( set_blockio_status, -1 );
} 

TEST_F( TCSetBlockIOTest, testSuccess ) {

   int set_blockio_status = tc_set_blockio_timeout_limit( device, 5 );

   EXPECT_EQ( set_blockio_status, 0 );
   EXPECT_EQ( device->blockio_limit, 5 );
} 

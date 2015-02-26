/*
 * $Id: TCBlockioTest.cpp 3370 2013-12-20 17:29:05Z jpenn1 $
 */

#include <gtest/gtest.h>
#include "trick_utils/comm/include/tc.h"
#include "trick_utils/comm/include/attributes.h"
#include "trick_utils/comm/include/hs_msg.h"
#include "trick_utils/comm/include/tc_proto.h"
#include "trick_utils/comm/include/trick_byteswap.h"
#include "trick_utils/comm/include/trick_error_hndlr.h"

class TCBlockioTest : public testing::Test {

   protected:
      TCBlockioTest(){}
      ~TCBlockioTest(){}

      TCDevice* device;

      void SetUp(){

         device = (TCDevice *) malloc(sizeof(TCDevice));
         memset( (void *)device,'\0',sizeof(TCDevice) );
      }

      void TearDown(){
   
         free(device);
      }
};

#if 0
TEST_F( TCBlockioTest, testNullDevice ) {

   int blockio_status = tc_blockio( NULL, TC_COMM_ALL_OR_NOTHING );

   EXPECT_EQ( TC_EWOULDBLOCK, blockio_status );
} 

TEST_F( TCBlockioTest, testTC_COMM_BLOCKIO ) {

   (void) tc_blockio( device, TC_COMM_BLOCKIO );
   EXPECT_EQ( TC_COMM_BLOCKIO, device->blockio_type);
} 

TEST_F( TCBlockioTest, testTC_COMM_NOBLOCKIO ) {

   (void) tc_blockio( device, TC_COMM_NOBLOCKIO );
   EXPECT_EQ( TC_COMM_NOBLOCKIO, device->blockio_type);
} 

TEST_F( TCBlockioTest, testTC_COMM_TIMED_BLOCKIO ) {

   (void) tc_blockio( device, TC_COMM_TIMED_BLOCKIO );
   EXPECT_EQ( TC_COMM_TIMED_BLOCKIO, device->blockio_type );
} 

TEST_F( TCBlockioTest, testTC_COMM_ALL_OR_NOTHING ) {

   (void) tc_blockio( device, TC_COMM_ALL_OR_NOTHING );
   EXPECT_EQ( TC_COMM_ALL_OR_NOTHING, device->blockio_type );
} 
#endif


#include <gtest/gtest.h>

#include "trick_utils/comm/include/tc.h"
#include "trick_utils/comm/include/attributes.h"
#include "trick_utils/comm/include/hs_msg.h"
#include "trick_utils/comm/include/tc_proto.h"
#include "trick_utils/comm/include/trick_byteswap.h"
#include "trick_utils/comm/include/trick_error_hndlr.h"


class TCReadByteSwapTest : public testing::Test {

   protected:
      TCReadByteSwapTest(){}
      ~TCReadByteSwapTest(){}

      TCDevice* device;

      void SetUp(){

         device = (TCDevice *) malloc(sizeof(TCDevice));
         memset( (void *)device,'\0',sizeof(TCDevice) );
      }

      void TearDown(){
   
         free(device);
      }
};

/* Listed requirements are under requirement Trick-076 (3.1.2.9.7) */

#include <iostream>
#include <sys/types.h>
#include <signal.h>
#include <map>

#include "gtest/gtest.h"
#include "trick/bitfield_proto.h"
//#include "trick/RequirementScribe.hh"

namespace Trick {

typedef struct {
    unsigned int var1 :14;
    unsigned int var2 :5;
    unsigned int var3 :12;
    unsigned int var4 :1;
} uintBits;

typedef struct {
    int var1 :7;
    int var2 :16;
    int var3 :3;
    int var4 :6;
} intBits;

typedef struct {
    unsigned char var1 :3;
    unsigned char var2 :2;
    unsigned char var3 :2;
    unsigned char var4 :1;
} ucharBits;

typedef struct {
    char var1 :2;
    char var2 :2;
    char var3 :2;
    char var4 :2;
} charBits;

typedef struct {
    unsigned short var1 :4;
    unsigned short var2 :2;
    unsigned short var3 :9;
    unsigned short var4 :1;
} ushortBits;

typedef struct {
    short var1 :8;
    short var2 :1;
    short var3 :2;
    short var4 :5;
} shortBits;

/* this struct should be the size of the largest member
 *  * (in this case, unsigned int) */
typedef struct {
    unsigned char var1 :3;
    short var2 :8;
    int var3 :9;
    unsigned int var4 :12;
} mixBits;

class BitfieldsTest : public ::testing::Test {

	protected:
		//Trick::RequirementScribe req;

		BitfieldsTest() {}
		~BitfieldsTest() {}
		virtual void SetUp() {}
		virtual void TearDown() {}

		template <class T, class S>
		int bitTest(int bitSizes[], bool sign) {
    		int start = sizeof(T) * 8;

    		int bitVals[4] = {0, 0, 0, 0};
    		int num;
    		T test = 0;
    		union {
        		T total;
        		S b;
    		}bitRep;

    		if (sign) {
        		bitVals[0] = (1 << (bitSizes[0] - 1)) - 1;
        		bitVals[1] = -(1 << (bitSizes[1] - 1));
        		bitVals[2] = 0;
        		bitVals[3] = (1 << (bitSizes[3] - 1)) + (1 << bitSizes[3]/2);;
    		} else {
       			bitVals[0] = (1 << bitSizes[0]) + (1 << bitSizes[0]/2);
        		bitVals[1] = 0;
        		bitVals[2] = (1 << (bitSizes[2]/2)) - 1;
        		bitVals[3] = (1 << bitSizes[3]) - 1;
    		}

    		for (unsigned int i = 0; i < 4; i++) {

        		start -= bitSizes[i];
        		test = insert_bitfield_any((T) test, bitVals[i], sizeof(T), start, bitSizes[i]);
        		//std::cout << "Test: " << (int)test << std::endl;
        		if (sign) {
        			num = extract_bitfield_any((T) test, sizeof(T), start, bitSizes[i]);
        		} else {
        			num = extract_unsigned_bitfield_any((T) test, sizeof(T), 0, (start+bitSizes[i]));
        		}
        		if (bitVals[i] > num) {
        			/* bit value inserted is too large for bit member, ensure correct behavior */
        			EXPECT_EQ(num, bitVals[i] - (1 << bitSizes[i]));
        		} else if (bitVals[i] < num) {
        			EXPECT_EQ(num, -2*(1 << bitSizes[i]) + bitVals[i]);
        		} else {
        			EXPECT_EQ(num, bitVals[i]);
        		}
        	}

			bitRep.b.var1 = bitVals[0];
        	bitRep.b.var2 = bitVals[1];
        	bitRep.b.var3 = bitVals[2];
        	bitRep.b.var4 = bitVals[3];

			EXPECT_EQ(test, bitRep.total);

        	//printf("Decimal: %d %d %d %d\n", bitVals[0], bitVals[1], bitVals[2], bitVals[3]);
        	return 0;
		}


};

//Trick::Requirements the;
//the.add_parent_number("3.1.2.9.7");
//::testing::Test::RecordProperty("parent", "3.1.2.9.7");


TEST_F(BitfieldsTest, UnsignedInt) {
	//req.add_requirement("3119954850") ;

	int bitSizes[4] = {14, 5, 12, 1};

	bitTest <unsigned int, uintBits> (bitSizes, false);
}

TEST_F(BitfieldsTest, UnsignedChar) {
	//req.add_requirement("3119954850") ;

	int bitSizes[4] = {3, 2, 2, 1};

	bitTest <unsigned char, ucharBits> (bitSizes, false);
}

TEST_F(BitfieldsTest, UnsignedShort) {
	//req.add_requirement("3119954850") ;

	int bitSizes[4] = {4, 2, 9, 1};

	bitTest <unsigned short, ushortBits> (bitSizes, false);
}
TEST_F(BitfieldsTest, Int) {
	//req.add_requirement("1151502390");

	int bitSizes[4] = {7, 16, 3, 6};

	bitTest <unsigned int, intBits> (bitSizes, true);
}

TEST_F(BitfieldsTest, Char) {
	//req.add_requirement("1151502390") ;

	int bitSizes[4] = {2, 2, 2, 2};

	bitTest <unsigned char, charBits> (bitSizes, true);
}

TEST_F(BitfieldsTest, Short) {
	//req.add_requirement("1151502390") ;

	int bitSizes[4] = {8, 1, 2, 5};

	bitTest <unsigned short, shortBits> (bitSizes, true);
}

TEST_F(BitfieldsTest, Mix) {
	//req.add_requirement("1151502390") ;

	int bitSizes[4] = {3, 8, 9, 12};

	/* Expect size of mixed bitfield to be unsigned int (largest member) */
	if (sizeof(mixBits) == sizeof(unsigned int)) {
		bitTest <unsigned int, mixBits> (bitSizes, true);
	} else {
		EXPECT_TRUE(false);
	}
}

}

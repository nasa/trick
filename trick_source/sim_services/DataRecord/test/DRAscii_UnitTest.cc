/*
PURPOSE: (Testing)
*/

#include <gtest/gtest.h>
#include <gtest/gtest_prod.h>

#include "trick/DRAscii.hh"
#include "trick/MemoryManager.hh"
#include "DR_user_defined_types.hh"

#include <string>
#include <cstring>
#include <math.h>

namespace Trick {

        class DRAsciiTest : public ::testing::Test {

                protected:
                        Trick::MemoryManager *memmgr;
                        Trick::DRAscii *dr;
                        DRAsciiTest() {
                                try {
                                        memmgr = new Trick::MemoryManager;
                                } catch (std::logic_error e) {
                                        memmgr = NULL;
                                }
                                dr = new Trick::DRAscii("Testing");
                        }
                        ~DRAsciiTest() {
                                delete memmgr;
                        }
                        void SetUp() {}
                        void TearDown() {}

                        /**
                         * @brief       Generates a DataRecordBuffer object from a given REF2
                         * @param ref2  A pointer to the REF2 instance that the DataRecordBuffer will be built around
                         * @param drb   A pointer to the DataRecordBuffer that will take the output of the function
                         * 
                         * @see         include/trick/DataRecordGroup.hh::DataRecordBuffer()
                         * @see         include/trick/reference.h::REF2 
                         */
                        DataRecordBuffer* generateDataRecordBuffer(REF2 *ref2) {
                                DataRecordBuffer* drb = new Trick::DataRecordBuffer;
                                drb->name = std::string(ref2->reference) ;
                                drb->ref_searched = true ;
                                drb->ref = ref2 ;
                                drb->last_value = (char *)calloc(1 , drb->ref->attr->size) ;
                                drb->buffer = (char *)(ref2->address);
                                return drb;
                        }
        };

        // Test Cases

        TEST_F(DRAsciiTest, GroupName_OnCreation) {
                // BENCHMARK is the expected output or value checked against
                const std::string BENCHMARK = "Testing";

                delete dr;
                dr = new DRAscii(BENCHMARK);
                EXPECT_EQ(BENCHMARK, dr->get_group_name());
        }

        // Not sure if these tests are necessary...
        // TEST_F(DRAsciiTest, FloatFormat_OnCreation) {
        //         EXPECT_EQ("%20.8g", dr->ascii_float_format);
        // }

        // TEST_F(DRAsciiTest, DoubleFormat_OnCreation) {
        //         EXPECT_EQ("%20.16g", dr->ascii_double_format);
        // }

        TEST_F(DRAsciiTest, FloatFormat_Setting) {
                // BENCHMARK is the expected output or value checked against
                const std::string BENCHMARK = "%1.2g";

                dr->set_ascii_float_format(BENCHMARK);
                EXPECT_EQ(BENCHMARK, dr->ascii_float_format);
        }

        TEST_F(DRAsciiTest, DoubleFormat_Setting) {
                // BENCHMARK is the expected output or value checked against
                const std::string BENCHMARK = "%1.2g";

                dr->set_ascii_double_format(BENCHMARK);
                EXPECT_EQ(BENCHMARK, dr->ascii_double_format);
        }

        TEST_F(DRAsciiTest, CharToASCII) {
                // BENCHMARK is the expected output or value checked against
                const char BENCHMARK = 'T';

                // Variable Definition
                Trick::DataRecordBuffer *drb;
                char *cPtr;
                REF2 *ref2;

                // REF2 and DataRecordBuffer Set Up
                std::string varName = "_c";
                cPtr = (char*) memmgr->declare_var(("char " + varName).c_str(), 10);
                ref2 = memmgr->ref_attributes(varName.c_str());
                *cPtr = BENCHMARK;
                drb = generateDataRecordBuffer(ref2);

                // Asserts Checking that the DataRecordBuffer was set up correctly
                ASSERT_EQ(drb->name, varName);
                ASSERT_EQ(drb->ref->attr->type, TRICK_CHARACTER);
                ASSERT_EQ(((char *)drb->ref->address)[0],BENCHMARK);
                
                // Output Generation
                char *buf = (char*)malloc(10);
                dr->copy_data_ascii_item(drb,0,buf);

                // Test of Output
                EXPECT_EQ(buf[0],BENCHMARK);
                free(buf);
        }

        TEST_F(DRAsciiTest, UnsignedCharToASCII) {
                // BENCHMARK is the expected output or value checked against
                const unsigned char BENCHMARK = 200;

                // Variable Definition
                Trick::DataRecordBuffer *drb;
                unsigned char *cPtr;
                REF2 *ref2;

                // REF2 and DataRecordBuffer Set Up
                std::string varName = "_uc";
                cPtr = (unsigned char*) memmgr->declare_var(("unsigned char " + varName).c_str(), 10);
                ref2 = memmgr->ref_attributes(varName.c_str());
                *cPtr = BENCHMARK;
                drb = generateDataRecordBuffer(ref2);

                // Asserts Checking that the DataRecordBuffer was set up correctly
                ASSERT_EQ(drb->name, varName);
                ASSERT_EQ(drb->ref->attr->type, TRICK_UNSIGNED_CHARACTER);
                ASSERT_EQ((unsigned char)((char *)drb->ref->address)[0],BENCHMARK);
                
                // Output Generation
                char *buf = (char*)malloc(10);
                dr->copy_data_ascii_item(drb,0,buf);

                // Test of Output
                EXPECT_EQ(std::atoi(buf),BENCHMARK);
                free(buf);
        }

        TEST_F(DRAsciiTest, BooleanToASCII) {
                // Variable Definition
                Trick::DataRecordBuffer *drb;
                bool *bPtr;
                REF2 *ref2;

                // REF2 and DataRecordBuffer Set Up
                std::string varName = "_b";
                bPtr = (bool*) memmgr->declare_var(("bool " + varName).c_str(), 10);
                ref2 = memmgr->ref_attributes(varName.c_str());
                *bPtr = true;
                drb = generateDataRecordBuffer(ref2);

                // Asserts Checking that the DataRecordBuffer was set up correctly
                ASSERT_EQ(drb->name, varName);
                ASSERT_EQ(drb->ref->attr->type, TRICK_BOOLEAN);
                ASSERT_TRUE((bool *)drb->ref->address);
                
                // Output Generation
                char *buf = (char*)malloc(10);
                dr->copy_data_ascii_item(drb,0,buf);

                // Test of Output
                EXPECT_EQ(std::atoi(buf),1);

                // Testing Output if boolean is false
                *bPtr = false;
                dr->copy_data_ascii_item(drb,0,buf);
                EXPECT_EQ(std::atoi(buf),0);
                free(buf);
        }

        TEST_F(DRAsciiTest, StringToASCII) {
                // BENCHMARK is the expected output or value checked against
                const std::string BENCHMARK = "Hello World";

                // Variable Definition
                Trick::DataRecordBuffer *drb;
                std::string *sPtr;
                REF2 *ref2;

                // REF2 and DataRecordBuffer Set Up
                std::string varName = "_str";
                sPtr = (std::string*) memmgr->declare_var(("std::string " + varName).c_str(), 10);
                ref2 = memmgr->ref_attributes(varName.c_str());
                *sPtr = BENCHMARK;

                drb = generateDataRecordBuffer(ref2);

                // Asserts Checking that the DataRecordBuffer was set up correctly
                ASSERT_EQ(drb->name, varName);
                ASSERT_EQ(drb->ref->attr->type, TRICK_STRING);
                ASSERT_EQ(*((std::string*)drb->ref->address), BENCHMARK);
                
                // Output Generation
                char *buf = (char*)malloc(10);
                dr->copy_data_ascii_item(drb,0,buf);

                // Test of Output
                EXPECT_EQ(std::strcmp(buf,BENCHMARK.c_str()),0);
                free(buf);
        }

        TEST_F(DRAsciiTest, ShortToASCII) {
                // BENCHMARK is the expected output or value checked against
                const short BENCHMARK = -12345;

                // Variable Definition
                Trick::DataRecordBuffer *drb;
                short *stPtr;
                REF2 *ref2;

                // REF2 and DataRecordBuffer Set Up
                std::string varName = "_shrt";
                stPtr = (short*) memmgr->declare_var(("short " + varName).c_str(), 10);
                ref2 = memmgr->ref_attributes(varName.c_str());
                *stPtr = BENCHMARK;
                drb = generateDataRecordBuffer(ref2);

                // Asserts Checking that the DataRecordBuffer was set up correctly
                ASSERT_EQ(drb->name, varName);
                ASSERT_EQ(drb->ref->attr->type, TRICK_SHORT);
                ASSERT_EQ(*((short*)drb->ref->address),BENCHMARK);
                
                // Output Generation
                char *buf = (char*)malloc(10);
                dr->copy_data_ascii_item(drb,0,buf);

                // Test of Output
                EXPECT_EQ(std::atoi(buf), BENCHMARK);
                free(buf);
        }

        TEST_F(DRAsciiTest, UnsignedShortToASCII) {
                // BENCHMARK is the expected output or value checked against
                const unsigned short BENCHMARK = 12345;

                // Variable Definition
                Trick::DataRecordBuffer *drb;
                unsigned short *uShPtr;
                REF2 *ref2;

                // REF2 and DataRecordBuffer Set Up
                std::string varName = "_uShrt";
                uShPtr = (unsigned short*) memmgr->declare_var(("unsigned short " + varName).c_str(), 10);
                ref2 = memmgr->ref_attributes(varName.c_str());
                *uShPtr = BENCHMARK;
                drb = generateDataRecordBuffer(ref2);

                // Asserts Checking that the DataRecordBuffer was set up correctly
                ASSERT_EQ(drb->name, varName);
                ASSERT_EQ(drb->ref->attr->type, TRICK_UNSIGNED_SHORT);
                ASSERT_EQ(*((unsigned short*)drb->ref->address),BENCHMARK);
                
                // Output Generation
                char *buf = (char*)malloc(10);
                dr->copy_data_ascii_item(drb,0,buf);

                // Test of Output
                EXPECT_EQ(std::atoi(buf), BENCHMARK);
                free(buf);
        }

        TEST_F(DRAsciiTest, EnumeratedToASCII) {
                // BENCHMARK is the expected output or value checked against
                const FRUIT BENCHMARK = APPLE;

                // Variable Definition
                Trick::DataRecordBuffer *drb;
                FRUIT *enumPtr;
                REF2 *ref2;

                // REF2 and DataRecordBuffer Set Up
                std::string varName = "_enum";
                enumPtr = (FRUIT*) memmgr->declare_var(("FRUIT " + varName).c_str(), 10);
                ref2 = memmgr->ref_attributes(varName.c_str());
                *enumPtr = BENCHMARK;
                drb = generateDataRecordBuffer(ref2);

                // Asserts Checking that the DataRecordBuffer was set up correctly
                ASSERT_EQ(drb->name, varName);
                ASSERT_EQ(drb->ref->attr->type, TRICK_ENUMERATED);
                ASSERT_EQ(*((FRUIT*)drb->ref->address),BENCHMARK);
                
                // Output Generation
                char *buf = (char*)malloc(10);
                dr->copy_data_ascii_item(drb,0,buf);

                // Test of Output
                EXPECT_EQ(std::atoi(buf), BENCHMARK);
                free(buf);
        }

        TEST_F(DRAsciiTest, IntegerToASCII) {
                // BENCHMARK is the expected output or value checked against
                const int BENCHMARK = -123456789;

                // Variable Definition
                Trick::DataRecordBuffer *drb;
                int *intPtr;
                REF2 *ref2;

                // REF2 and DataRecordBuffer Set Up
                std::string varName = "_inte";
                intPtr = (int*) memmgr->declare_var(("int " + varName).c_str(), 10);
                ref2 = memmgr->ref_attributes(varName.c_str());
                *intPtr = BENCHMARK;
                drb = generateDataRecordBuffer(ref2);

                // Asserts Checking that the DataRecordBuffer was set up correctly
                ASSERT_EQ(drb->name, varName);
                ASSERT_EQ(drb->ref->attr->type, TRICK_INTEGER);
                ASSERT_EQ(*((int*)drb->ref->address),BENCHMARK);
                
                // Output Generation
                char *buf = (char*)malloc(10);
                dr->copy_data_ascii_item(drb,0,buf);

                // Test of Output
                EXPECT_EQ(std::atoi(buf), BENCHMARK);
                free(buf);
        }

        TEST_F(DRAsciiTest, UnsignedIntegerToASCII) {
                // BENCHMARK is the expected output or value checked against
                const unsigned int BENCHMARK = 123456789;

                // Variable Definition
                Trick::DataRecordBuffer *drb;
                unsigned int *uIntPtr;
                REF2 *ref2;

                // REF2 and DataRecordBuffer Set Up
                std::string varName = "_uInte";
                uIntPtr = (unsigned int*) memmgr->declare_var(("unsigned int " + varName).c_str(), 10);
                ref2 = memmgr->ref_attributes(varName.c_str());
                *uIntPtr = BENCHMARK;
                drb = generateDataRecordBuffer(ref2);

                // Asserts Checking that the DataRecordBuffer was set up correctly
                ASSERT_EQ(drb->name, varName);
                ASSERT_EQ(drb->ref->attr->type, TRICK_UNSIGNED_INTEGER);
                ASSERT_EQ(*((unsigned int*)drb->ref->address),BENCHMARK);
                
                // Output Generation
                char *buf = (char*)malloc(10);
                dr->copy_data_ascii_item(drb,0,buf);

                // Test of Output
                EXPECT_EQ(std::atoi(buf), BENCHMARK);
                free(buf);
        }

        TEST_F(DRAsciiTest, LongToASCII) {
                // BENCHMARK is the expected output or value checked against
                const long BENCHMARK = -123456789;

                // Variable Definition
                Trick::DataRecordBuffer *drb;
                long *longPtr;
                REF2 *ref2;

                // REF2 and DataRecordBuffer Set Up
                std::string varName = "_lng";
                longPtr = (long*) memmgr->declare_var(("long " + varName).c_str(), 10);
                ref2 = memmgr->ref_attributes(varName.c_str());
                *longPtr = BENCHMARK;
                drb = generateDataRecordBuffer(ref2);

                // Asserts Checking that the DataRecordBuffer was set up correctly
                ASSERT_EQ(drb->name, varName);
                ASSERT_EQ(drb->ref->attr->type, TRICK_LONG);
                ASSERT_EQ(*((long*)drb->ref->address),BENCHMARK);
                
                // Output Generation
                char *buf = (char*)malloc(10);
                dr->copy_data_ascii_item(drb,0,buf);

                // Test of Output
                EXPECT_EQ(std::atol(buf), BENCHMARK);
                free(buf);
        }

        TEST_F(DRAsciiTest, UnsignedLongToASCII) {
                // BENCHMARK is the expected output or value checked against
                const unsigned long BENCHMARK = 345678901;

                // Variable Definition
                Trick::DataRecordBuffer *drb;
                unsigned long *uLngPtr;
                REF2 *ref2;

                // REF2 and DataRecordBuffer Set Up
                std::string varName = "_uLong";
                uLngPtr = (unsigned long*) memmgr->declare_var(("unsigned long " + varName).c_str(), 10);
                ref2 = memmgr->ref_attributes(varName.c_str());
                *uLngPtr = BENCHMARK;
                drb = generateDataRecordBuffer(ref2);

                // Asserts Checking that the DataRecordBuffer was set up correctly
                ASSERT_EQ(drb->name, varName);
                ASSERT_EQ(drb->ref->attr->type, TRICK_UNSIGNED_LONG);
                ASSERT_EQ(*((unsigned long*)drb->ref->address),BENCHMARK);
                
                // Output Generation
                char *buf = (char*)malloc(10);
                dr->copy_data_ascii_item(drb,0,buf);

                // Test of Output
                EXPECT_EQ(std::atol(buf), BENCHMARK);
                free(buf);
        }

        TEST_F(DRAsciiTest, FloatToASCII) {
                // BENCHMARK is the expected output or value checked against
                const float BENCHMARK = 123.456f;

                // Variable Definition
                Trick::DataRecordBuffer *drb;
                float *floatPtr;
                REF2 *ref2;

                // REF2 and DataRecordBuffer Set Up
                std::string varName = "_flt";
                floatPtr = (float*) memmgr->declare_var(("float " + varName).c_str(), 10);
                ref2 = memmgr->ref_attributes(varName.c_str());
                *floatPtr = BENCHMARK;
                drb = generateDataRecordBuffer(ref2);

                // Asserts Checking that the DataRecordBuffer was set up correctly
                ASSERT_EQ(drb->name, varName);
                ASSERT_EQ(drb->ref->attr->type, TRICK_FLOAT);
                ASSERT_EQ(*((float*)drb->ref->address),BENCHMARK);
                
                // Output Generation
                char *buf = (char*)malloc(10);
                dr->copy_data_ascii_item(drb,0,buf);

                // Test of Output
                EXPECT_FLOAT_EQ(std::atof(buf),BENCHMARK);
                free(buf);
        }

        TEST_F(DRAsciiTest, DoubleToASCII) {
                // BENCHMARK is the expected output or value checked against
                const double BENCHMARK = 123.456f;

                // Variable Definition
                Trick::DataRecordBuffer *drb;
                double *doublePtr;
                REF2 *ref2;

                // REF2 and DataRecordBuffer Set Up
                std::string varName = "_flt";
                doublePtr = (double*) memmgr->declare_var(("double " + varName).c_str(), 10);
                ref2 = memmgr->ref_attributes(varName.c_str());
                *doublePtr = BENCHMARK;
                drb = generateDataRecordBuffer(ref2);

                // Asserts Checking that the DataRecordBuffer was set up correctly
                ASSERT_EQ(drb->name, varName);
                ASSERT_EQ(drb->ref->attr->type, TRICK_DOUBLE);
                ASSERT_EQ(*((double*)drb->ref->address),BENCHMARK);
                
                // Output Generation
                char *buf = (char*)malloc(10);
                dr->copy_data_ascii_item(drb,0,buf);
                
                EXPECT_DOUBLE_EQ(std::stod(buf), BENCHMARK);
                free(buf);
        }

        TEST_F(DRAsciiTest, BitfieldToASCII) {

        }

        TEST_F(DRAsciiTest, UnsignedBitfieldToASCII) {

        }

        TEST_F(DRAsciiTest, LongLongToASCII) {
                // BENCHMARK is the expected output or value checked against
                const long long BENCHMARK = -1234567890;

                // Variable Definition
                Trick::DataRecordBuffer *drb;
                long long *longPtr;
                REF2 *ref2;

                // REF2 and DataRecordBuffer Set Up
                std::string varName = "_lng";
                longPtr = (long long*) memmgr->declare_var(("long long " + varName).c_str(), 10);
                ref2 = memmgr->ref_attributes(varName.c_str());
                *longPtr = BENCHMARK;
                drb = generateDataRecordBuffer(ref2);

                // Asserts Checking that the DataRecordBuffer was set up correctly
                ASSERT_EQ(drb->name, varName);
                ASSERT_EQ(drb->ref->attr->type, TRICK_LONG_LONG);
                ASSERT_EQ(*((long long*)drb->ref->address),BENCHMARK);
                
                // Output Generation
                char *buf = (char*)malloc(10);
                dr->copy_data_ascii_item(drb,0,buf);

                // Test of Output
                EXPECT_EQ(std::atoi(buf), BENCHMARK);
                free(buf);
        }

        TEST_F(DRAsciiTest, UnsignedLongLongToASCII) {
                // BENCHMARK is the expected output or value checked against
                const unsigned long long BENCHMARK = 1234567890;

                // Variable Definition
                Trick::DataRecordBuffer *drb;
                unsigned long long *uLngPtr;
                REF2 *ref2;

                // REF2 and DataRecordBuffer Set Up
                std::string varName = "_uLong";
                uLngPtr = (unsigned long long*) memmgr->declare_var(("unsigned long long " + varName).c_str(), 10);
                ref2 = memmgr->ref_attributes(varName.c_str());
                *uLngPtr = BENCHMARK;
                drb = generateDataRecordBuffer(ref2);

                // Asserts Checking that the DataRecordBuffer was set up correctly
                ASSERT_EQ(drb->name, varName);
                ASSERT_EQ(drb->ref->attr->type, TRICK_UNSIGNED_LONG_LONG);
                ASSERT_EQ(*((unsigned long long*)drb->ref->address),BENCHMARK);
                
                // Output Generation
                char *buf = (char*)malloc(10);
                dr->copy_data_ascii_item(drb,0,buf);

                // Test of Output
                EXPECT_EQ(std::atoi(buf), BENCHMARK);
                free(buf);
        }



        /* TEST TARGETS
        *   Constructor: Creation of a New Group w/ the given name
        *   Destructor: N/A
        *   Setting Float Format: Check that the assignment went through correctly
        *   Double Format: Check that the assignment went through correctly
        *   Delimiter: Check that the assignment went through correctly
        *   Setting Precision: Check both true and false params 
        *   Converting data value to ASCII
        *   Initialization
        *   Header
        *   Data Writing
        *   Shutdown
        */

}
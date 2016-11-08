/******************************TRICK HEADER*************************************
PURPOSE:                     ( Test variable server )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( scheduled )
LIBRARY DEPENDENCY:          ( VS.o )
PROGRAMMERS:                 ( (Lindsay Landry) (L3) (9-12-2013) )
*******************************************************************************/

#include <sys/resource.h>

#include "../include/VS.hh"
#include "sim_services/VariableServer/include/variable_server_proto.h"
#include "sim_services/VariableServer/include/VariableServer.hh"
#include "sim_services/UnitTest/include/trick_tests.h"

int VSTest::strcmp_IgnoringWhiteSpace(const char* s1, const char* s2) {
    int i1 = 0;
    int i2 = 0;

    while (1) {
        while ( !isgraph( s1[i1] ) && s1[i1] != '\0') { i1++; }
        while ( !isgraph( s2[i2] ) && s2[i2] != '\0') { i2++; }
        if ( s1[i1] == '\0' && s2[i2] == '\0') { return 0; }
        if ( s1[i1] != s2[i2]) {
            if (s1[i1] < s2[i2]) {
                return -1;
            } else {
                return 1;
            }
        }
        i1++; i2++;
    }
}


// Expect cycle time of 0.01 s
int VSTest::testCycle() {
    char msg[256];
    char suite[] = "VariableServerTest";
/*
    int num, num2;
    double cpu_t, cpu_st;
    struct rusage cpu_usg;

    sprintf(msg,"trick.var_add(\"vsx.vst.l\")\n");
    vs_write(msg);

    // currently bad programming, could get stuck in this loop
    while (true) {
        getrusage(RUSAGE_SELF, &cpu_usg);
        cpu_st = ((double) cpu_usg.ru_utime.tv_sec) + ((double) cpu_usg.ru_utime.tv_usec/1000000.0);

        num = tc_pending(&bob);
        //first read.
        if (num) {
            break;
        }
    }

    while (true) {
        getrusage(RUSAGE_SELF, &cpu_usg);
        cpu_t = ((double) cpu_usg.ru_utime.tv_sec) + ((double) cpu_usg.ru_utime.tv_usec/1000000.0);

        num2 = tc_pending(&bob);

        if (num2 == 2*num) {
            break;
        }
    }

    //std::cout << cpu_t - cpu_st << std::endl;

    sprintf(msg,"trick.var_clear()\n");
    vs_write(msg);
*/
}


int VSTest::testAddRemove() {
    char msg[256];
    char suite[] = "VariableServerTest";
    int result;

    // NO UNITS
    sprintf(msg,"trick.var_add(\"vsx.vst.c\")\n");
    vs_write(msg);
    vs_read();
    std::cout << got_read << std::endl;
    result = strcmp_IgnoringWhiteSpace("0  -1234", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableAddNoUnits")
    trick_test_add_parent( suite , "VariableAddNoUnits" , "3700107028");

    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  -1234", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableAddCyclic")
    trick_test_add_parent( suite , "VariableAddCyclic" , "3700107028");

    // INVALID UNIT CHANGE
    sprintf(msg,"trick.var_units(\"vsx.vst.c\",\"g\")\n");
    vs_write(msg);
    std::cerr << "The purpose of this test is to cause an error. Error messages are expected." << std::endl;
    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  -1234", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableInvalidUnits")
    trick_test_add_parent( suite , "VariableInvalidUnits" , "");

    // ADD UNITS
    sprintf(msg,"trick.var_add(\"vsx.vst.e\",\"M\")\n");
    vs_write(msg);
    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  -1234  -123456 {M}", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableAddUnits")
    trick_test_add_parent( suite , "VariableAddUnits" , "3700107028");

    // CHANGE UNITS
    sprintf(msg,"trick.var_units(\"vsx.vst.e\",\"ft\")\n");
    vs_write(msg);
    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  -1234  -405039 {ft}", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableChangeUnits")
    trick_test_add_parent( suite , "VariableChangeUnits" , "2710769246");

    // REMOVE SINGLE VARIABLE
    sprintf(msg,"trick.var_remove(\"vsx.vst.e\")\n");
    vs_write(msg);
    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  -1234", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableRemove")
    trick_test_add_parent( suite , "VariableRemove" , "806750761");

    // CLEAR VARIABLE SERVER
    sprintf(msg,"trick.var_clear()\n");
    vs_write(msg);
    vs_read();
    TRICK_EXPECT_EQ(strcmp(got_read, ""), 0, suite, "VariableClear")
    trick_test_add_parent( suite , "VariableClear" , "835578702");

    return(0);
}

int VSTest::testExists() {
    char msg[256];
    char suite[] = "VariableServerTest";
    int result;

    // VARIABLE EXISTS
    sprintf(msg,"trick.var_exists(\"vsx.vst.e\")\n");
    vs_write(msg);
    vs_read();
    result = strcmp_IgnoringWhiteSpace("1  1",got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableExists")
    trick_test_add_parent( suite , "VariableExists" , "3587464751");

    // VARIABLE DOES NOT EXIST
    sprintf(msg,"trick.var_exists(\"vsx.vst.z\")\n");
    vs_write(msg);
    vs_read();
    result = strcmp_IgnoringWhiteSpace("1  0",got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableNotExists")
    trick_test_add_parent( suite , "VariableNotExists" , "3587464751");

    sprintf(msg,"trick.var_clear()\n");
    vs_write(msg);

    return(0);
}

int VSTest::testPause() {
    char msg[256];
    char suite[] = "VariableServerTest";
    int result;

    sprintf(msg,"trick.var_add(\"vsx.vst.f\")\n");
    vs_write(msg);
    sprintf(msg,"trick.var_add(\"vsx.vst.i\")\n");
    vs_write(msg);

    // PAUSE
    sprintf(msg,"trick.var_pause()\n");
    vs_write(msg);
    vs_read();
    vs_read();
    TRICK_EXPECT_EQ(strcmp(got_read, ""), 0, suite, "VariablePause")
    trick_test_add_parent( suite , "VariablePause" , "964174074");

    // SEND
    sprintf(msg,"trick.var_send()\n");
    vs_write(msg);
    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  123456  1234.5677", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableSend")
    trick_test_add_parent( suite , "VariableSend" , "67211805");

    vs_read();
    TRICK_EXPECT_EQ(strcmp(got_read, ""), 0, suite, "VariableSendNoCyclic")
    trick_test_add_parent( suite , "VariableSendNoCyclic" , "67211805");

    // UNPAUSE
    sprintf(msg,"trick.var_unpause()\n");
    vs_write(msg);
    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  123456  1234.5677", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableUnpause")
    trick_test_add_parent( suite , "VariableUnpause" , "964174074");

    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  123456  1234.5677", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableUnpauseCyclic")
    trick_test_add_parent( suite , "VariableUnpauseCyclic" , "964174074");

    sprintf(msg,"trick.var_clear()\n");
    vs_write(msg);

    return(0);
}

int VSTest::testOther() {
    char msg[256];
    char suite[] = "VariableServerTest";

    sprintf(msg,"trick.var_binary()\n");
    vs_write(msg);
    sprintf(msg,"trick.var_add(\"vsx.vst.l\")\n");
    vs_write(msg);
    vs_read();

    return(0);
}

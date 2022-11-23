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

int VSTest::testUnits() {
    char msg[256];
    char suite[] = "VariableServerTest";
    int result;

    // INVALID UNIT CHANGE
    snprintf(msg, sizeof(msg), "trick.var_add(\"vsx.vst.c\")\ntrick.var_units(\"vsx.vst.c\",\"g\")\n");
    vs_write(msg);
    std::cerr << "The purpose of this test is to cause an error. Error messages are expected." << std::endl;
    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  -1234", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableInvalidUnits")

    // ADD UNITS
    snprintf(msg, sizeof(msg), "trick.var_add(\"vsx.vst.e\",\"m\")\n");
    vs_write(msg);
    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  -1234  -123456 {m}", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableAddUnits")

    // CHANGE UNITS
    snprintf(msg, sizeof(msg), "trick.var_units(\"vsx.vst.e\",\"ft\")\n");
    vs_write(msg);
    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  -1234  -405039 {ft}", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableChangeUnits")

    // CLEAR VARIABLE SERVER
    snprintf(msg, sizeof(msg), "trick.var_clear()\n");
    vs_write(msg);
}

int VSTest::testAddRemove() {
    char msg[256];
    char suite[] = "VariableServerTest";
    int result;

    // NO UNITS
    snprintf(msg, sizeof(msg), "trick.var_add(\"vsx.vst.c\")\n");
    vs_write(msg);
    vs_read();
    std::cout << got_read << std::endl;
    result = strcmp_IgnoringWhiteSpace("0  -1234", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableAddNoUnits")

    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  -1234", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableAddCyclic")

    // REMOVE SINGLE VARIABLE
    snprintf(msg, sizeof(msg), "trick.var_remove(\"vsx.vst.e\")\n");
    vs_write(msg);
    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  -1234", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableRemove")

    // CLEAR VARIABLE SERVER
    snprintf(msg, sizeof(msg), "trick.var_clear()\n");
    vs_write(msg);
    vs_read();
    TRICK_EXPECT_EQ(strcmp(got_read, ""), 0, suite, "VariableClear")

    return(0);
}

int VSTest::testSendOnce() {
    char msg[256];
    char suite[] = "VariableServerTest";
    int result;

    // SEND ONCE
    snprintf(msg, sizeof(msg), "trick.var_send_once(\"vsx.vst.e\")\n");
    vs_write(msg);
    vs_read();
    result = strcmp_IgnoringWhiteSpace("5  -123456", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableSendOnce")
    trick_test_add_parent( suite , "VariableSendOnce" , "");

    // SEND ONCE LIST
    snprintf(msg, sizeof(msg), "trick.var_send_once(\"vsx.vst.n[0], vsx.vst.n[1], vsx.vst.n[2],\", 3)\n");
    vs_write(msg);
    vs_read();
    result = strcmp_IgnoringWhiteSpace("5   0   1   2", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableSendOnceList")

    // SEND ONCE LIST - WRONG INDICES
    snprintf(msg, sizeof(msg), "trick.var_send_once(\"vsx.vst.n[0], vsx.vst.n[1], vsx.vst.n[2],\", 4)\n");
    vs_write(msg);
    std::cerr << "The purpose of this test is to cause an error. Error messages are expected." << std::endl;
    vs_read();
    result = strcmp_IgnoringWhiteSpace("", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableSendOnceListError")
}

int VSTest::testExists() {
    char msg[256];
    char suite[] = "VariableServerTest";
    int result;

    // VARIABLE EXISTS
    snprintf(msg, sizeof(msg), "trick.var_exists(\"vsx.vst.e\")\n");
    vs_write(msg);
    vs_read();
    result = strcmp_IgnoringWhiteSpace("1  1",got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableExists")
    trick_test_add_parent( suite , "VariableExists" , "3587464751");

    // VARIABLE DOES NOT EXIST
    snprintf(msg, sizeof(msg), "trick.var_exists(\"vsx.vst.z\")\n");
    vs_write(msg);
    vs_read();
    std::cout << "Check variable doesn't exist: " << got_read << std::endl;
    result = strcmp_IgnoringWhiteSpace("1  0",got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableNotExists")

    snprintf(msg, sizeof(msg), "trick.var_clear()\n");
    vs_write(msg);

    return(0);
}

int VSTest::testPause() {
    char msg[256];
    char suite[] = "VariableServerTest";
    int result;

    snprintf(msg, sizeof(msg), "trick.var_add(\"vsx.vst.f\")\n");
    vs_write(msg);
    snprintf(msg, sizeof(msg), "trick.var_add(\"vsx.vst.i\")\n");
    vs_write(msg);

    // PAUSE
    snprintf(msg, sizeof(msg), "trick.var_pause()\n");
    vs_write(msg);
    vs_read();
    vs_read();
    TRICK_EXPECT_EQ(strcmp(got_read, ""), 0, suite, "VariablePause")
    trick_test_add_parent( suite , "VariablePause" , "964174074");

    // SEND
    snprintf(msg, sizeof(msg), "trick.var_send()\n");
    vs_write(msg);
    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  123456  1234.5677", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableSend")
    trick_test_add_parent( suite , "VariableSend" , "67211805");

    vs_read();
    TRICK_EXPECT_EQ(strcmp(got_read, ""), 0, suite, "VariableSendNoCyclic")
    trick_test_add_parent( suite , "VariableSendNoCyclic" , "67211805");

    // UNPAUSE
    snprintf(msg, sizeof(msg), "trick.var_unpause()\n");
    vs_write(msg);
    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  123456  1234.5677", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableUnpause")
    trick_test_add_parent( suite , "VariableUnpause" , "964174074");

    vs_read();
    result = strcmp_IgnoringWhiteSpace("0  123456  1234.5677", got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableUnpauseCyclic")
    trick_test_add_parent( suite , "VariableUnpauseCyclic" , "964174074");

    snprintf(msg, sizeof(msg), "trick.var_clear()\n");
    vs_write(msg);

    return(0);
}

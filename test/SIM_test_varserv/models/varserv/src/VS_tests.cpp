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

int VSTest::strcmp_IgnoringWhiteSpace(std::string s1_str, const char* s2) {
    const char * s1 = s1_str.c_str();
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
    std::string expected;

    // INVALID UNIT CHANGE
    sprintf(msg,"trick.var_add(\"vsx.vst.c\")\ntrick.var_units(\"vsx.vst.c\",\"g\")\n");
    vs_write(msg);
    std::cerr << "The purpose of this test is to cause an error. Error messages are expected." << std::endl;
    vs_read();
    expected = "0  -1234";
    result = strcmp_IgnoringWhiteSpace(expected, got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableInvalidUnits")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;

    // ADD UNITS
    sprintf(msg,"trick.var_add(\"vsx.vst.e\",\"m\")\n");
    vs_write(msg);
    vs_read();
    expected = "0  -1234  -123456 {m}";
    result = strcmp_IgnoringWhiteSpace(expected, got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableAddUnits")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;


    // CHANGE UNITS
    sprintf(msg,"trick.var_units(\"vsx.vst.e\",\"ft\")\n");
    vs_write(msg);
    vs_read();
    expected = "0  -1234  -405039 {ft}";
    result = strcmp_IgnoringWhiteSpace(expected, got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableChangeUnits")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;


    // CLEAR VARIABLE SERVER
    sprintf(msg,"trick.var_clear()\n");
    vs_write(msg);
}

int VSTest::testAddRemove() {
    char msg[256];
    char suite[] = "VariableServerTest";
    int result;

    // NO UNITS
    sprintf(msg,"trick.var_add(\"vsx.vst.c\")\n");
    vs_write(msg);
    vs_read();
    std::string expected = "0  -1234";
    result = strcmp_IgnoringWhiteSpace(expected, got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableAddNoUnits")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;


    vs_read();
    expected = "0  -1234";
    result = strcmp_IgnoringWhiteSpace(expected, got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableAddCyclic")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;


    // REMOVE SINGLE VARIABLE
    sprintf(msg,"trick.var_remove(\"vsx.vst.e\")\n");
    vs_write(msg);
    vs_read();
    expected = "0  -1234";
    result = strcmp_IgnoringWhiteSpace(expected, got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableRemove")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;


    // CLEAR VARIABLE SERVER
    sprintf(msg,"trick.var_clear()\n");
    vs_write(msg);
    vs_read();
    expected = "";
    result = strcmp_IgnoringWhiteSpace(expected, got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableClear")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;


    return(0);
}

int VSTest::testSendOnce() {
    char msg[256];
    char suite[] = "VariableServerTest";
    int result;
    std::string expected;

    // SEND ONCE
    sprintf(msg,"trick.var_send_once(\"vsx.vst.e\")\n");
    vs_write(msg);
    vs_read();

    expected = "5  -123456";
    result = strcmp_IgnoringWhiteSpace(expected, got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableSendOnce")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;


    // SEND ONCE LIST
    sprintf(msg,"trick.var_send_once(\"vsx.vst.n[0], vsx.vst.n[1], vsx.vst.n[2],\", 3)\n");
    vs_write(msg);
    vs_read();
    expected = "5   0   1   2";
    result = strcmp_IgnoringWhiteSpace(expected, got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableSendOnceList")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;


    // SEND ONCE LIST - WRONG INDICES
    sprintf(msg,"trick.var_send_once(\"vsx.vst.n[0], vsx.vst.n[1], vsx.vst.n[2],\", 4)\n");
    vs_write(msg);
    std::cerr << "The purpose of this test is to cause an error. Error messages are expected." << std::endl;
    vs_read();
    expected = "";
    result = strcmp_IgnoringWhiteSpace(expected, got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableSendOnceListError")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;

}

int VSTest::testExists() {
    char msg[256];
    char suite[] = "VariableServerTest";
    int result;
    std::string expected;

    // VARIABLE EXISTS
    sprintf(msg,"trick.var_exists(\"vsx.vst.e\")\n");
    vs_write(msg);
    vs_read();
    expected = "1  1";
    result = strcmp_IgnoringWhiteSpace(expected,got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableExists")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;


    // VARIABLE DOES NOT EXIST
    sprintf(msg,"trick.var_exists(\"vsx.vst.z\")\n");
    vs_write(msg);
    vs_read();
    std::cout << "Check variable doesn't exist: " << got_read << std::endl;
    expected = "1  0";
    result = strcmp_IgnoringWhiteSpace(expected,got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableNotExists")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;


    sprintf(msg,"trick.var_clear()\n");
    vs_write(msg);

    return(0);
}

int VSTest::testPause() {
    char msg[256];
    char suite[] = "VariableServerTest";
    int result;
    std::string expected;

    sprintf(msg,"trick.var_add(\"vsx.vst.f\")\n");
    vs_write(msg);
    sprintf(msg,"trick.var_add(\"vsx.vst.i\")\n");
    vs_write(msg);

    // PAUSE
    sprintf(msg,"trick.var_pause()\n");
    vs_write(msg);
    vs_read();
    vs_read();
    expected = "";
    result = strcmp(got_read, expected.c_str());
    TRICK_EXPECT_EQ(result, 0, suite, "VariablePause")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;

    // SEND
    sprintf(msg,"trick.var_send()\n");
    vs_write(msg);
    vs_read();
    expected = "0  123456  1234.5677";
    result = strcmp_IgnoringWhiteSpace(expected, got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableSend")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;

    vs_read();
    expected = "";
    result = strcmp(got_read, expected.c_str());
    TRICK_EXPECT_EQ(result, 0, suite, "VariableSendNoCyclic")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;

    // UNPAUSE
    sprintf(msg,"trick.var_unpause()\n");
    vs_write(msg);
    vs_read();
    expected = "0  123456  1234.5677";
    result = strcmp_IgnoringWhiteSpace(expected, got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableUnpause")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;

    vs_read();
    expected = "0  123456  1234.5677";
    result = strcmp_IgnoringWhiteSpace(expected, got_read);
    TRICK_EXPECT_EQ(result, 0, suite, "VariableUnpauseCyclic")
    std::cout << "Actual: " << got_read << "\tExpected: " << expected << std::endl;

    sprintf(msg,"trick.var_clear()\n");
    vs_write(msg);

    return(0);
}

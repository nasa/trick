#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "units_conv.h"

int n_failures = 0;

void test_convert_spec(const char *old_units_spec) {

     char new_units_spec[100];
     new_units_spec[0] = 0;
     if ( convert_units_spec (old_units_spec, new_units_spec) == 0 ) {
             printf (" PASS: %s => %s\n\n",old_units_spec, new_units_spec);
     } else {
             printf (" FAIL: %s %s\n\n",old_units_spec, new_units_spec);
             n_failures ++;
     }
}

int main (int argc, char * argv[] ) {

     UnitsConvFn_t Newton_to_Poundal_fn1;
     UnitsConvFn_t Newton_to_Poundal_fn2;
     Units_t *Bogus;
     Units_t *Newton;
     Units_t *Poundal;
     double poundals;

     printf("================================================================================.\n");
     printf("Test Suite for the \"C\" Language Interface to the Trick Unit Conversion Package.\n");
     printf("================================================================================.\n\n");

     printf("Test 1\n");
     printf("Attempt to create a Units_t object with a bogus units specifier.\n");
     printf("Note that we expect to generate an error condition.\n");

     if ( ( Bogus = new_units("foo") ) == NULL ) {
        printf("PASS\n\n");
     }else {
        printf("FAIL\n\n"); n_failures++;
     }


     printf("Test 2\n");
     printf("Create a conversion function object (UnitsConvFn_t) directly from Newtons (\"kg*m/s2\")\n");
     printf("to Poundals (\"lbm*ft/s2\") using conv_fn_s().\n");

     if (conv_fn_s("kg*m/s2","lbm*ft/s2", &Newton_to_Poundal_fn1) == CONV_OK) {
        printf("PASS\n\n");
     }else {
        printf("FAIL\n\n"); n_failures++;
     }


     printf("Test 3\n");
     printf("Create Units_t objects from units specifiers: \"kg*m/s2\" and \"lbm*ft/s2\".\n");
     Newton  = new_units("kg*m/s2");
     Poundal = new_units("lbm*ft/s2");
     if ((Newton != NULL) && (Poundal != NULL)) {
        printf("PASS\n\n");
     }else {
        printf("FAIL\n\n"); n_failures++;
     }

     printf("Test 4\n");
     printf("Create a conversion function (Newtons -> Poundals) using the Newton Units_t object,\n");
     printf("the Poundals Units_t object, and conv_fn_u().\n");

     if (conv_fn_u(Newton, Poundal, &Newton_to_Poundal_fn2) == CONV_OK) {
        printf("PASS\n\n");
     }else {
        printf("FAIL\n\n"); n_failures++;
     }

     printf("Test 5\n");
     printf("Compare the conversion function objects created by conv_fn_s() and conv_fn_u().\n");
     printf("We expect them to be identical.\n");
     if (memcmp(&Newton_to_Poundal_fn1, &Newton_to_Poundal_fn2, sizeof(UnitsConvFn_t)) == 0) {
        printf("PASS\n\n");
     }else {
        printf("FAIL\n\n"); n_failures++;
     }

     printf("Test 6\n");
     printf("We also expect them to be correct, i.e., we expect: 1 Newton = 7.23301 Poundals.\n");

     poundals = convert_units(1.0, &Newton_to_Poundal_fn1 );

     if ( fabs(poundals - 7.23301) < 0.00001 ) {
        printf("PASS\n\n");
     }else {
        printf("FAIL\n\n"); n_failures++;
     }

     printf("Test conversion of unit specifications from Pre-07 to 07.\n");
     printf("Test 7\n"); test_convert_spec("ft");
     printf("Test 8\n"); test_convert_spec("insl2");
     printf("Test 9\n"); test_convert_spec("yddday");
     printf("Test 10\n"); test_convert_spec("kgM/s2");
     printf("Test 11\n"); test_convert_spec("kg*M/s2");
     printf("Test 12\n"); test_convert_spec("M2N3/s2");
     printf("Test 13\n"); test_convert_spec("M2*N3/s2");
     printf("Test 14\n"); test_convert_spec("M2/N3s2");



     printf("=========================================================================.\n");
     printf(" END OF \"C\" TEST SUITE\n");
     printf(" Number of test failures = %d\n\n", n_failures);
     printf(" NOTE:\n");
     printf(" Be sure to also run \"unit_cpp_test\", which tests the \"C++\" interface\n");
     printf(" and is a much more extensive test of the Units Conversion Package.\n");
     printf("=========================================================================.\n\n");

     return CONV_OK;
}

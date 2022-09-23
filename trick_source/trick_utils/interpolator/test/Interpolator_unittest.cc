
#include <gtest/gtest.h>
#include <iostream>
#include "trick/Interpolator.hh"
//#include "trick/RequirementScribe.hh"

#define EXCEPTABLE_ERROR 0.2

TEST(Interpolator_unittest, OneDimension) {
   //Trick::RequirementScribe req;
   //req.add_requirement("3369678360");

   bool exception_thrown;
   double result;

   double BpA[] = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0}; 

   double* break_point_arrays[1] = { BpA };
   unsigned int break_point_array_sizes[1] = { 11 };
   double table[] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};

   Trick::Interpolator* my_interpolator = new Trick::Interpolator( table, break_point_arrays, break_point_array_sizes, 1); 

   try {
       exception_thrown = false;
       result = my_interpolator->eval(-1.0);
   } catch (std::logic_error e) {
       exception_thrown = true;
   }
   EXPECT_TRUE(exception_thrown);

   try {
       exception_thrown = false;
       result = my_interpolator->eval(0.0);
   } catch (std::logic_error e) {
       exception_thrown = true;
   } 
   EXPECT_FALSE(exception_thrown);
   EXPECT_NEAR(result, 0.0, EXCEPTABLE_ERROR);

   try {
       exception_thrown = false;
       result = my_interpolator->eval(0.1);
   } catch (std::logic_error e) {
       exception_thrown = true;
   } 
   EXPECT_FALSE(exception_thrown);
   EXPECT_NEAR(result, 0.5, EXCEPTABLE_ERROR);


   try {
       exception_thrown = false;
   result = my_interpolator->eval(0.2);
   } catch (std::logic_error e) {
       exception_thrown = true;
   } 
   EXPECT_FALSE(exception_thrown);
   EXPECT_NEAR(result, 1.0, EXCEPTABLE_ERROR);

   try {
       exception_thrown = false;
   result = my_interpolator->eval(0.3);
   } catch (std::logic_error e) {
       exception_thrown = true;
   } 
   EXPECT_FALSE(exception_thrown);
   EXPECT_NEAR(result, 1.5, EXCEPTABLE_ERROR);

   try {
       exception_thrown = false;
   result = my_interpolator->eval(2.0);
   } catch (std::logic_error e) {
       exception_thrown = true;
   } 
   EXPECT_FALSE(exception_thrown);
   EXPECT_NEAR(result, 10.0, EXCEPTABLE_ERROR);

   try {
       exception_thrown = false;
       result = my_interpolator->eval(3.0);
   } catch (std::logic_error e) {
       exception_thrown = true;
   } 
   EXPECT_TRUE(exception_thrown);

}

TEST(Interpolator_unittest, TwoDimension) {
   // Below is a Body Mass Index(BMI) table. BMI is a function of height and weight.
   // It is defined as:
   //
   //                    BMI = mass(kg) / (height(m))^2 
   //
   // Reference: http://en.wikipedia.org/wiki/Body_mass_index 
   //

   //Trick::RequirementScribe req;
   //req.add_requirement("715834524");


   double weight_lb[] = { 88.2, 110.25, 132.3, 154.35, 176.4, 198.45, 220.5, 242.55, 264.65, 286.65, 308.6, 330.75, 352.7};
   double height_inches[] = { 59.0, 63.0, 67.0, 71.0, 75.0, 78.0 };

   double* bmi_break_point_arrays[2] = {weight_lb, height_inches};
   unsigned int bmi_break_point_array_sizes[2] = {13, 6};
   double bmi_table[] = {
                /*  1.5m    1.6m    1.7m    1.8m    1.9m     2.0m */
       /*  40kg */ 17.78,  15.62,  13.84,  12.35,  11.08,   10.00,
       /*  50kg */ 22.22,  19.53,  17.30,  15.43,  13.85,   12.50,
       /*  60kg */ 26.67,  23.44,  20.76,  18.52,  16.62,   15.00,
       /*  70kg */ 31.11,  27.34,  24.22,  21.60,  19.39,   17.50,
       /*  80kg */ 35.56,  31.25,  27.68,  24.69,  22.16,   20.00,
       /*  90kg */ 40.00,  35.16,  31.14,  27.78,  24.93,   22.50,
       /* 100kg */ 44.44,  39.06,  34.60,  30.86,  27.70,   25.00,
       /* 110kg */ 48.89,  42.97,  38.06,  33.95,  30.47,   27.50,
       /* 120kg */ 53.33,  46.87,  41.52,  37.04,  33.24,   30.00,
       /* 130kg */ 57.78,  50.78,  44.98,  40.12,  36.01,   32.50,
       /* 140kg */ 62.22,  54.69,  48.44,  43.21,  38.78,   35.00,
       /* 150kg */ 66.67,  58.59,  51.90,  46.30,  41.55,   37.50,
       /* 160kg */ 71.11,  62.50,  55.36,  49.38,  44.32,   40.00
   };
   
   Trick::Interpolator* bmi_interpolator = new Trick::Interpolator( bmi_table, bmi_break_point_arrays, bmi_break_point_array_sizes, 2); 
   double bmi;

   bmi = bmi_interpolator->eval(175.0, 72.0);
   EXPECT_NEAR(bmi, 23.7, EXCEPTABLE_ERROR);

   bmi = bmi_interpolator->eval(185.0, 72.0);
   EXPECT_NEAR(bmi, 25.1, EXCEPTABLE_ERROR);

   bmi = bmi_interpolator->eval(201.0, 72.0);
   EXPECT_NEAR(bmi, 27.4, EXCEPTABLE_ERROR);

   bmi = bmi_interpolator->eval(220.0, 72.0);
   EXPECT_NEAR(bmi, 29.9, EXCEPTABLE_ERROR);

   bmi = bmi_interpolator->eval(185.0, 68.0);
   EXPECT_NEAR(bmi, 28.1, EXCEPTABLE_ERROR);

}

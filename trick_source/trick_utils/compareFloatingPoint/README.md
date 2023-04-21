
# Compare Floating Point Numbers

The following functions compare floating-point numbers to determine whether they are within a specified tolerance of each other.

These functions are designed to never generate ```FP_SUBNORMAL``` numbers, that could result in a floating point underflow exception, even if the OS doesn't handle floating point underflows by setting their values to zero.

## Header

```#include "trick/compareFloatingPoint.hh" ```

## ```Trick::dbl_is_near```
```c
bool Trick::dbl_is_near( double A, double B, double tolerance);
```

This function compares the values of ```double A``` and ```double B``` to determine whether they are within tolerance of each other. If they are, then the function returns ```true```, otherwise it returns ```false```.

The design of ```Trick::dbl_is_near``` requires that the minimum tolerance be ```DBL_MIN/DBL_EPSILON,``` which is approximately ```1.00208e-292```. That is, any two arguments whose difference is less than or equal to ```1.00208e-292``` are considered to be within tolerance, regardless of the specified tolerance.

Before thinking that doubles should be compared to a tolerance smaller than ```1.00208e-292```, please consider that the ratio of the Planck length to the size of the observable universe is approximately ```1.8e-62```. Also consider that our minimum tolerance is ```5.4e-231``` times smaller than that. So, we think that'll probably be good enough in most cases.

## ```Trick::flt_is_near```

```c
bool Trick::flt_is_near( float A, float B, float tolerance);
```

This function compares the values of ```float A``` and ```float B``` to determine whether they are within tolerance of each other. If they are, then the function returns ```true```, otherwise it returns ```false```.

The minimum tolerance for ```Trick::flt_is_near``` is ```FLT_MIN/FLT_EPSILON```, which is approximately ```9.86076e-32```.

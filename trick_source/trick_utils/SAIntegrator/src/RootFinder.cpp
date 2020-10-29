#include "Rootfinder.hh"
#include <math.h>
#include <float.h>
#include <iostream>

void RootFinder::init () {
    f_error_tol = 0.00000000001;
    iterations = 0;
    prev_f_error = DBL_MAX;
    slope_constraint = Unconstrained;
    lower_set = false;
    upper_set = false;
}
RootFinder::RootFinder () {
    init();
}
RootFinder::RootFinder (double tolerance, SlopeConstraint constraint) {
    init();
    f_error_tol = tolerance;
    slope_constraint = constraint;
}
// Given the values of the independent variable, and the function,
// estimate the distance of the independent variable from functions root.
double RootFinder::find_roots( double x, double f_error ) {
    if (  ( fabs(f_error) < f_error_tol) ||
          (( iterations > 0) && (fabs(prev_f_error - f_error) < f_error_tol))
       ) {
        if (slope_constraint == Unconstrained) {
            return (0.0);
        } else if (slope_constraint == Positive && lower_set) {
            return (0.0);
        } else if (slope_constraint == Negative && upper_set) {
            return (0.0);
        }
    }
    if (f_error < 0.0) {
        f_lower = f_error;
        x_lower = x;
        lower_set = true;
    } else if (f_error > 0.0) {
        f_upper = f_error;
        x_upper = x;
        upper_set = true;
    }
    iterations++;
    if (upper_set && lower_set) {
        double x_correction_estimate;
        if (fabs(f_error) < f_error_tol)
            x_correction_estimate = 0.0;
        else {
            x_correction_estimate = -f_error/((f_upper-f_lower)/(x_upper-x_lower));
            if (iterations > 20)
                x_correction_estimate = 0.0;
        }
        /* Check for Positive or Negative constraint */
        switch (slope_constraint) {
            case Unconstrained:
                prev_f_error = f_error;
                return (x_correction_estimate);
            case Positive:
                if (f_slope == Positive) {
                    prev_f_error = f_error;
                    return (x_correction_estimate);
                } else {
                    lower_set = false;
                    f_slope = Positive;
                }
                break;
            case Negative:
                if (f_slope == Negative) {
                    prev_f_error = f_error;
                    return (x_correction_estimate);
                } else {
                    upper_set = false;
                    f_slope = Negative;
                }
                break;
        }
        f_slope = Unconstrained;
    } else if (lower_set == 1) {
        f_slope = Positive;
    } else if (upper_set == 1) {
        f_slope = Negative;
    }
    iterations = 0;
    return (DBL_MAX);
}

#ifndef ROOTFINDER_HH
#define ROOTFINDER_HH
#include <iostream>

typedef enum {
    Negative = -1,
    Unconstrained,
    Positive
} SlopeConstraint;

class RootFinder {
    public:
        void init();
        void init( double tolerance, SlopeConstraint constraint);
        RootFinder();
        RootFinder (double tolerance, SlopeConstraint constraint);
        double find_roots( double x, double f_error );
        friend std::ostream& operator<<(std::ostream& os, const RootFinder& rf);
    private:
        double f_upper;
        double x_upper;
        bool upper_set;
        double f_lower;
        double x_lower;
        bool lower_set;
        double prev_f_error;
        double f_error_tol;
        int iterations;
        SlopeConstraint slope_constraint;
        SlopeConstraint f_slope;
};
#endif /* ROOTFINDER_HH */

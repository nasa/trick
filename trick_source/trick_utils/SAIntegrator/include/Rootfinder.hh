
typedef enum {
    Negative = -1,
    Unconstrained,
    Positive
} SlopeConstraint;

class RootFinder {
    public:
        void init();
        RootFinder();
        RootFinder (double tolerance, SlopeConstraint constraint);
        double find_roots( double x, double f_error );
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

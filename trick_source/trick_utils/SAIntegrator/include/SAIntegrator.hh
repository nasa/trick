#ifndef SAINTEGRATOR_HH
#define SAINTEGRATOR_HH
#include "Rootfinder.hh"
#include <iostream>

namespace SA {

    class Integrator {
    protected:
        double X_in;       // Independent Variable In
        double X_out;      // Independent Variable Out
        double default_h;  // Default step-size
        void*  user_data;  // User data
        void advanceIndyVar();
    public:
        Integrator();
        Integrator(double h, void* udata);
        virtual ~Integrator() {}
        virtual void load();
        virtual void unload();
        virtual void step();
        void integrate();
        virtual double undo_integrate();
        double getIndyVar();
        void setIndyVar(double v);
        friend std::ostream& operator<<(std::ostream& os, const SA::Integrator& I);
    };

    std::ostream& operator<<(std::ostream& os, const Integrator& I);

    typedef void (*DerivsFunc)( double x, double state[], double derivs[], void* udata);

    class FirstOrderODEIntegrator : public Integrator {
    protected:
        unsigned int state_size; // Size of the state vector.
        double*  inState;        // State vector prior to integration step.
        double*  outState;       // State vector result of integration step.
        double** inVars;         // Array of pointers to the variables whose values comprise the input state vector.
        double** outVars;        // Array of pointers to the variables to which the output state vector values are to be disseminated.
        DerivsFunc derivs_func;  // Pointer to the function that calculates the derivatives to be integrated.
    public:
        FirstOrderODEIntegrator();
        FirstOrderODEIntegrator( double h, unsigned int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        FirstOrderODEIntegrator(const FirstOrderODEIntegrator& other);
        FirstOrderODEIntegrator& operator=( const FirstOrderODEIntegrator& other);
        virtual ~FirstOrderODEIntegrator();
        void load();                               // Overrides Integrator::load(), Final
        void unload();                             // Overrides Integrator::unload(), Final
        virtual void step();                       // Overrides Integrator::step(). Virtual
        virtual double undo_integrate();           // Overrides Integrator::undo_integrate(), Virtual
        void load_from_outState();                 // New, Final
        double** set_in_vars( double* in_vars[]);  // New, Final
        double** set_out_vars( double* out_vars[]);// New, Final

        friend std::ostream& operator<<(std::ostream& os, const SA::FirstOrderODEIntegrator& I);
    };
    std::ostream& operator<<(std::ostream& os, const FirstOrderODEIntegrator& I);

    typedef double (*RootErrorFunc)( double x, double state[], RootFinder* root_finder, void* udata);

    class FirstOrderODEVariableStepIntegrator : public FirstOrderODEIntegrator {
    private:
        RootFinder* root_finder;
        RootErrorFunc root_error_func;
    protected:
        double last_h;
        void advanceIndyVar( double h);
    public:
        FirstOrderODEVariableStepIntegrator();
        FirstOrderODEVariableStepIntegrator( double h, unsigned int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        FirstOrderODEVariableStepIntegrator( const FirstOrderODEVariableStepIntegrator& other);
        FirstOrderODEVariableStepIntegrator& operator=( const FirstOrderODEVariableStepIntegrator& rhs);
        ~FirstOrderODEVariableStepIntegrator();
        void step();                                                        // Overrides FirstOrderODEIntegrator::step(), Final
        double undo_integrate();                                            // Overrides FirstOrderODEIntegrator::undo_integrate(), Final
        virtual void variable_step( double h);                              // New, Should be overridden in derived classes, Virtual
        void add_Rootfinder( double tolerance, SlopeConstraint constraint, RootErrorFunc rfunc);
        friend std::ostream& operator<<(std::ostream& os, const SA::FirstOrderODEVariableStepIntegrator& I);
    private:
        void find_roots(double h, unsigned int depth);
    };
    std::ostream& operator<<(std::ostream& os, const FirstOrderODEVariableStepIntegrator& I);

    class EulerIntegrator : public FirstOrderODEVariableStepIntegrator {
        public:
        EulerIntegrator();
        EulerIntegrator( double h, unsigned int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        EulerIntegrator( double h, unsigned int N, double* in_out_vars[], DerivsFunc derivs_func, void* udata);
        EulerIntegrator(const EulerIntegrator& other);
        EulerIntegrator& operator=( const EulerIntegrator& rhs);
        ~EulerIntegrator();
        void variable_step( double h);
        friend std::ostream& operator<<(std::ostream& os, const SA::EulerIntegrator& I);
    };
    std::ostream& operator<<(std::ostream& os, const EulerIntegrator& I);

    class HeunsMethod : public FirstOrderODEVariableStepIntegrator {
        public:
        HeunsMethod();
        HeunsMethod( double h, unsigned int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        HeunsMethod( double h, unsigned int N, double* in_out_vars[], DerivsFunc derivs_func, void* udata);
        HeunsMethod(const HeunsMethod& other);
        HeunsMethod& operator=( const HeunsMethod& rhs);
        ~HeunsMethod();
        void variable_step( double h);
        friend std::ostream& operator<<(std::ostream& os, const SA::HeunsMethod& I);
    };
    std::ostream& operator<<(std::ostream& os, const HeunsMethod& I);

    class RK2Integrator : public FirstOrderODEVariableStepIntegrator {
        public:
        RK2Integrator();
        RK2Integrator( double h, unsigned int N, double* in_vars[], double* out_vars[], DerivsFunc derivs_func, void* udata);
        RK2Integrator( double h, unsigned int N, double* in_out_vars[], DerivsFunc derivs_func, void* udata);
        RK2Integrator(const RK2Integrator& other);
        RK2Integrator& operator=( const RK2Integrator& rhs);
        ~RK2Integrator();
        void variable_step( double h);
        friend std::ostream& operator<<(std::ostream& os, const SA::RK2Integrator& I);
    };
    std::ostream& operator<<(std::ostream& os, const RK2Integrator& I);

    class RK4Integrator : public FirstOrderODEVariableStepIntegrator {
        public:
        RK4Integrator();
        RK4Integrator( double h, unsigned int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        RK4Integrator( double h, unsigned int N, double* in_out_vars[], DerivsFunc derivs_func, void* udata);
        RK4Integrator(const RK4Integrator& other);
        RK4Integrator& operator=( const RK4Integrator& rhs);
        ~RK4Integrator();
        void variable_step( double h);
        friend std::ostream& operator<<(std::ostream& os, const SA::RK4Integrator& I);
    };
    std::ostream& operator<<(std::ostream& os, const RK4Integrator& I);

    class RK3_8Integrator : public FirstOrderODEVariableStepIntegrator {
        public:
        RK3_8Integrator();
        RK3_8Integrator( double h, unsigned int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        RK3_8Integrator( double h, unsigned int N, double* in_out_vars[], DerivsFunc derivs_func, void* udata);
        RK3_8Integrator(const RK3_8Integrator& other);
        RK3_8Integrator& operator=( const RK3_8Integrator& rhs);
        ~RK3_8Integrator();
        void variable_step( double h);

        friend std::ostream& operator<<(std::ostream& os, const SA::RK3_8Integrator& I);
    };
    std::ostream& operator<<(std::ostream& os, const RK3_8Integrator& I);

    class RKF45Integrator : public FirstOrderODEIntegrator {
        protected:
        double epsilon;
        double next_h; // the next value of h necessary to maintain accuracy.
        double last_h;
        // default_h will represent the maximum value of h.
        void advanceIndyVar( double h);
        public:
        RKF45Integrator();
        RKF45Integrator( double epsilon, double h, unsigned int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        RKF45Integrator( double epsilon, double h, unsigned int N, double* in_out_vars[], DerivsFunc derivs_func, void* udata);
        RKF45Integrator(const RKF45Integrator& other);
        RKF45Integrator& operator=( const RKF45Integrator& rhs);
        ~RKF45Integrator();
        void step();
        // Returns the next suggested step-size.
        double adaptive_step( double h);
        double getLastStepSize();

        friend std::ostream& operator<<(std::ostream& os, const SA::RKF45Integrator& I);
    };
    std::ostream& operator<<(std::ostream& os, const RKF45Integrator& I);


    typedef void (*Derivs2Func)( double t, double x[], double v[], double derivs[], void* udata);

    class EulerCromerIntegrator : public Integrator {
    protected:
        unsigned int nDimensions;
        double** pos_p;
        double** vel_p;
        double* pos_in;
        double* vel_in;
        double* pos_out;
        double* vel_out;
        double* g_out;
        double last_h;
        Derivs2Func gderivs;

    public:
        EulerCromerIntegrator();
        EulerCromerIntegrator(double dt, unsigned int N, double* xp[], double* vp[], Derivs2Func g, void* udata);
        EulerCromerIntegrator(const EulerCromerIntegrator& other);
        EulerCromerIntegrator& operator=( const EulerCromerIntegrator& rhs);
        ~EulerCromerIntegrator();
        void advanceIndyVar(double h);
        void step(double dt);
        void step();
        void load();
        void unload();
        double undo_integrate();
        friend std::ostream& operator<<(std::ostream& os, const SA::EulerCromerIntegrator& I);
    };
    std::ostream& operator<<(std::ostream& os, const EulerCromerIntegrator& I);

    // AdamsBashforthMoulton Self Priming
    class ABMIntegrator : public FirstOrderODEIntegrator {
    protected:
        double **    deriv_history;
        double *     composite_deriv;
        unsigned int bufferSize;
        unsigned int algorithmHistorySize;
        unsigned int hix;
        unsigned int currentHistorySize;
    public:
        ABMIntegrator(unsigned int history_size, double h, unsigned int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ABMIntegrator(unsigned int history_size, double h, unsigned int N, double* in_out_vars[], DerivsFunc derivs_func, void* udata);
        ABMIntegrator(const ABMIntegrator& other);
        ABMIntegrator& operator=( const ABMIntegrator& rhs);
        ~ABMIntegrator();
        void step();
        double undo_integrate();
        friend std::ostream& operator<<(std::ostream& os, const ABMIntegrator& I);
    };
    std::ostream& operator<<(std::ostream& os, const ABMIntegrator& I);

    // AdamsBashforthMoulton 2 with RK2 Priming
    class ABM2Integrator : public FirstOrderODEIntegrator {
    protected:
        double **    deriv_history;
        double *     composite_deriv;
        unsigned int bufferSize;
        unsigned int hix;
        unsigned int currentHistorySize;
        RK2Integrator* priming_integrator;
    public:
        ABM2Integrator( double h, unsigned int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ABM2Integrator( double h, unsigned int N, double* in_out_vars[], DerivsFunc derivs_func, void* udata);
        ABM2Integrator(const ABM2Integrator& other);
        ABM2Integrator& operator=( const ABM2Integrator& rhs);
        ~ABM2Integrator();
        void step();
        double undo_integrate();
        friend std::ostream& operator<<(std::ostream& os, const ABM2Integrator& I);
    };
    std::ostream& operator<<(std::ostream& os, const ABM2Integrator& I);

    // AdamsBashforthMoulton 4 with RK4 Priming
    class ABM4Integrator : public FirstOrderODEIntegrator {
    protected:
        double **    deriv_history;
        double *     composite_deriv;
        unsigned int bufferSize;
        unsigned int hix;
        unsigned int currentHistorySize;
        RK4Integrator* priming_integrator;
    public:
        ABM4Integrator( double h, unsigned int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ABM4Integrator( double h, unsigned int N, double* in_out_vars[], DerivsFunc derivs_func, void* udata);
        ABM4Integrator(const ABM4Integrator& other);
        ABM4Integrator& operator=( const ABM4Integrator& rhs);
        ~ABM4Integrator();
        void step();
        double undo_integrate();
        friend std::ostream& operator<<(std::ostream& os, const ABM4Integrator& I);
    };
    std::ostream& operator<<(std::ostream& os, const ABM4Integrator& I);
}
#endif /* SAINTEGRATOR_HH */

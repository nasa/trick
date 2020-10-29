#include "Rootfinder.hh"
namespace SA {

    class Integrator {
    protected:
        double X_in;  // Independent Variable In
        double X_out; // Independent Variable Out
        double default_h;  // Default step-size
        void*  user_data;  // User data
    public:
        Integrator(double h, void* udata): X_in(0.0), X_out(0.0), default_h(h), user_data(udata) {};
        virtual ~Integrator() {}
        virtual void step();
        virtual void load();
        virtual void unload();
        double getIndyVar();
        void setIndyVar(double v);
    };

    typedef void (*DerivsFunc)( double x, double state[], double derivs[], void* udata);

    class FirstOrderODEIntegrator : public Integrator {
    protected:
        unsigned int state_size; // size of the state vector.
        double*  inState;
        double*  outState;
        double** inVars;
        double** outVars;
        DerivsFunc derivs_func;
        double last_h; // What was the last step-size? For undo_step().

    public:
        FirstOrderODEIntegrator( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        virtual ~FirstOrderODEIntegrator();
        virtual double undo_step();
        void load();
        void unload();
        void load_from_outState();
        double** set_in_vars( double* in_vars[]);
        double** set_out_vars( double* out_vars[]);
    };

    typedef double (*RootErrorFunc)( double x, double state[], RootFinder* root_finder, void* udata);

    class FirstOrderODEVariableStepIntegrator : public FirstOrderODEIntegrator {
        RootFinder* root_finder;
        RootErrorFunc root_error_func;
    public:
        FirstOrderODEVariableStepIntegrator( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ~FirstOrderODEVariableStepIntegrator();
        void add_Rootfinder( RootFinder* root_finder, RootErrorFunc rfunc);
        // virtual void variable_step( double h)=0;
        virtual void variable_step( double h);
        void step();
    protected:
        void advanceIndyVar( double h );
    private:
        void find_roots(double h, unsigned int depth);
    };

    class EulerIntegrator : public FirstOrderODEVariableStepIntegrator {
        public:
        double *derivs;
        EulerIntegrator( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ~EulerIntegrator();
        void variable_step( double h);
    };

    class HeunsMethod : public FirstOrderODEVariableStepIntegrator {
        public:
        double *wstate;
        double *derivs[2];
        HeunsMethod( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ~HeunsMethod();
        void variable_step( double h);
    };

    class RK2Integrator : public FirstOrderODEVariableStepIntegrator {
        public:
        double *wstate;
        double *derivs[2];
        RK2Integrator( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc derivs_func, void* udata);
        ~RK2Integrator();
        void variable_step( double h);
    };

    class RK4Integrator : public FirstOrderODEVariableStepIntegrator {
        public:
        double *wstate[3];
        double *derivs[4];
        RK4Integrator( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ~RK4Integrator();
        void variable_step( double h);
    };

    class RK3_8Integrator : public FirstOrderODEVariableStepIntegrator {
        public:
        double *wstate[4];
        double *derivs[4];
        RK3_8Integrator( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ~RK3_8Integrator();
        void variable_step( double h);
    };

    class EulerCromerIntegrator : public Integrator {
    protected:
        unsigned int state_size;
        double** x_p;
        double** v_p;
        double* x_in;
        double* v_in;
        double* x_out;
        double* v_out;
        double* g_out;
        double* f_out;
        DerivsFunc gderivs;
        DerivsFunc fderivs;

    public:
        EulerCromerIntegrator(double dt, int N, double* xp[], double* vp[], DerivsFunc g, DerivsFunc f , void* udata);
        ~EulerCromerIntegrator();
        void step( double dt);
        void step();
        void load();
        void unload();
    };

    // class ABMIntegrator : public FirstOrderODEIntegrator {
    // protected:
    //     double **    deriv_history;
    //     double *     composite_deriv;
    //     unsigned int bufferSize;
    //     unsigned int algorithmHistorySize;
    //     unsigned int hix;
    //     unsigned int currentHistorySize;
    //
    // public:
    //     ABMIntegrator(int history_size, double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
    //     ~ABMIntegrator();
    //     void step();
    //     void undo_step();
    // };

    // AdamsBashforthMoulton 2
    class ABM2Integrator : public FirstOrderODEIntegrator {
    protected:
        double **    deriv_history;
        double *     composite_deriv;
        unsigned int bufferSize;
        unsigned int hix;
        unsigned int currentHistorySize;
        FirstOrderODEVariableStepIntegrator* priming_integrator;

    public:
        ABM2Integrator( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ~ABM2Integrator();
        void step();
        double undo_step();
    };

    // AdamsBashforthMoulton 4
    class ABM4Integrator : public FirstOrderODEIntegrator {
    protected:
        double **    deriv_history;
        double *     composite_deriv;
        unsigned int bufferSize;
        unsigned int hix;
        unsigned int currentHistorySize;
        FirstOrderODEVariableStepIntegrator* priming_integrator;

    public:
        ABM4Integrator( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ~ABM4Integrator();
        void step();
        double undo_step();
    };
}

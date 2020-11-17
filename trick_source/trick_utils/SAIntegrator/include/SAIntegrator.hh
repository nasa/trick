#include "Rootfinder.hh"
namespace SA {

    class Integrator {
    protected:
        double X_in;       // Independent Variable In
        double X_out;      // Independent Variable Out
        double default_h;  // Default step-size
        void*  user_data;  // User data
        void advanceIndyVar();
    public:
        Integrator(double h, void* udata): X_in(0.0), X_out(0.0), default_h(h), user_data(udata) {};
        virtual ~Integrator() {}
        virtual void load();
        virtual void unload();
        virtual void step();
        void integrate();
        virtual double undo_integrate();
        double getIndyVar();
        void setIndyVar(double v);
    };

    typedef void (*DerivsFunc)( double x, double state[], double derivs[], void* udata);

    class FirstOrderODEIntegrator : public Integrator {
    protected:
        unsigned int state_size; // Size of the state vector.
        double*  inState;        // State vector prior to integration step.
        double*  outState;       // State vector result of integration step.
        double** inVars;         // Array of pointers to the variables whose values comprise the input state vector.
        double** outVars;        // Array of pointers to the variables to which the output state vector values are to be disseminated.
        DerivsFunc derivs_func;  // Pointer to the function that calculates the derivatives to be integrated.
     // void advanceIndyVar();   // Inherited from Integrator::advanceIndyVar()
    public:
        FirstOrderODEIntegrator( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        virtual ~FirstOrderODEIntegrator();
        void load();                               // Overrides Integrator::load(), Final
        void unload();                             // Overrides Integrator::unload(), Final
        virtual void step();                       // Overrides Integrator::step(). Virtual
        // integrate();                               Inherited from Integrator::integrate(), Final
        virtual double undo_integrate();           // Overrides Integrator::undo_integrate(), Virtual
        void load_from_outState();                 // New, Final
        double** set_in_vars( double* in_vars[]);  // New, Final
        double** set_out_vars( double* out_vars[]);// New, Final
        // double getIndyVar();
        // void setIndyVar(double v);
    };

    typedef double (*RootErrorFunc)( double x, double state[], RootFinder* root_finder, void* udata);

    class FirstOrderODEVariableStepIntegrator : public FirstOrderODEIntegrator {
    private:
        RootFinder* root_finder;
        RootErrorFunc root_error_func;
    protected:
        double last_h;
        // void advanceIndyVar();                                           // Inherited from Integrator::advanceIndyVar(), Final
        void advanceIndyVar( double h);
    public:
        FirstOrderODEVariableStepIntegrator( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ~FirstOrderODEVariableStepIntegrator();
        // load();                                                             Inherited from FirstOrderODEIntegrator, Final
        // unload();                                                           Inherited from FirstOrderODEIntegrator, Final
        void step();                                                        // Overrides FirstOrderODEIntegrator::step(), Final
        // integrate();                                                     // Inherited from Integrator
        double undo_integrate();                                            // Overrides FirstOrderODEIntegrator::undo_integrate(), Final
        // load_from_outState();                                            // Inherited from FirstOrderODEIntegrator, Final
        // set_in_vars();                                                   // Inherited from FirstOrderODEIntegrator, Final
        // set_out_vars();                                                  // Inherited from FirstOrderODEIntegrator, Final
        virtual void variable_step( double h);                              // New, Should be overridden in derived classes, Virtual
        void add_Rootfinder( RootFinder* root_finder, RootErrorFunc rfunc); // New, Final
        // double getIndyVar();                                             // Inherited from Integrator
        // void setIndyVar(double v);                                       // Inherited from Integrator
    private:
        void find_roots(double h, unsigned int depth);
    };

    class EulerIntegrator : public FirstOrderODEVariableStepIntegrator {
        public:
        double *derivs;
        EulerIntegrator( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ~EulerIntegrator();
        // load();                                                              Inherited from FirstOrderODEIntegrator
        // unload();                                                            Inherited from FirstOrderODEIntegrator
        // void step();                                                         Inherited from FirstOrderODEVariableStepIntegrator
        // void integrate()                                                     Inherited from FirstOrderODEVariableStepIntegrator
        // double undo_integrate();                                             Inherited from FirstOrderODEVariableStepIntegrator
        // load_from_outState();                                                Inherited from FirstOrderODEIntegrator
        // set_in_vars();                                                       Inherited from FirstOrderODEIntegrator
        // set_out_vars();                                                      Inherited from FirstOrderODEIntegrator
        void variable_step( double h);                                       // Overrides FirstOrderODEVariableStepIntegrator::variable_step()
        // void integrate(double h)                                             Inherited from FirstOrderODEVariableStepIntegrator
        // void add_Rootfinder( RootFinder* root_finder, RootErrorFunc rfunc);  Inherited from FirstOrderODEVariableStepIntegrator
        // double getIndyVar();                                                 Inherited from Integrator
        // void setIndyVar(double v);                                           Inherited from Integrator
    };

    class HeunsMethod : public FirstOrderODEVariableStepIntegrator {
        public:
        double *wstate;
        double *derivs[2];
        HeunsMethod( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ~HeunsMethod();
        void variable_step( double h); // Overrides FirstOrderODEVariableStepIntegrator::variable_step()
    };

    class RK2Integrator : public FirstOrderODEVariableStepIntegrator {
        public:
        double *wstate;
        double *derivs[2];
        RK2Integrator( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc derivs_func, void* udata);
        ~RK2Integrator();
        void variable_step( double h); // Overrides FirstOrderODEVariableStepIntegrator::variable_step()
    };

    class RK4Integrator : public FirstOrderODEVariableStepIntegrator {
        public:
        double *wstate[3];
        double *derivs[4];
        RK4Integrator( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ~RK4Integrator();
        void variable_step( double h); // Overrides FirstOrderODEVariableStepIntegrator::variable_step()
    };

    class RK3_8Integrator : public FirstOrderODEVariableStepIntegrator {
        public:
        double *wstate[4];
        double *derivs[4];
        RK3_8Integrator( double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ~RK3_8Integrator();
        void variable_step( double h); // Overrides FirstOrderODEVariableStepIntegrator::variable_step()
    };

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
        double undo_integrate();
    };

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
        ABMIntegrator(int history_size, double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata);
        ~ABMIntegrator();
        void step();
        double undo_integrate();
    };

    // AdamsBashforthMoulton 2 with RK2 Priming
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
        double undo_integrate();
    };

    // AdamsBashforthMoulton 4 with RK4 Priming
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
        double undo_integrate();
    };
}

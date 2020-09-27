
namespace SA {
    
    class Integrator {
    protected:
        double time;
        double default_dt;
        void * udata;
    public:
        Integrator(double dt, void* user_data): time(0.0), default_dt(dt), udata(user_data) {};
        virtual ~Integrator() {}
        virtual void step() { time += default_dt; }; // Integrate over implicit (default) time step (default: default_dt)
        virtual void load() = 0;
        virtual void unload() = 0;
        double getTime() {return time;}
    };

    typedef void (*derivsFunc)( double t, double state[], double derivs[], void* user_data);

    class FirstOrderODEIntegrator : public Integrator {
    protected:
        unsigned int state_size; // size of the state vector.
        double* istate;
        double* ostate;
        double** orig_vars;
        double** dest_vars;
        derivsFunc derivs_func;

        bool reset;
        double last_dt; // What was the last step-size? For undo_step().

    public:
        FirstOrderODEIntegrator( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data);
        ~FirstOrderODEIntegrator();
        // void init(double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func);
        virtual void undo_step();
        void load();
        void unload();
    };

    class FirstOrderODEVariableStepIntegrator : public FirstOrderODEIntegrator {

    public:
        FirstOrderODEVariableStepIntegrator( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data);
        using Integrator::step;
        virtual void step( double dt );
    };

    class EulerIntegrator : public FirstOrderODEVariableStepIntegrator {
        public:
        double *derivs;
        EulerIntegrator( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc derivs_func, void* user_data);
        ~EulerIntegrator();
        void step( double dt);
        void step();
    };

    class HeunsMethod : public FirstOrderODEVariableStepIntegrator {
        public:
        double *wstate;
        double *derivs[2];

        HeunsMethod( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc derivs_func, void* user_data);
        ~HeunsMethod();
        void step( double dt);
        void step();
    };

    class RK2Integrator : public FirstOrderODEVariableStepIntegrator {
        public:
        double *wstate;
        double *derivs[2];

        RK2Integrator( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc derivs_func, void* user_data);
        ~RK2Integrator();
        void step( double dt);
        void step();
    };

    class RK4Integrator : public FirstOrderODEVariableStepIntegrator {
        public:
        double *wstate[3];
        double *derivs[4];

        RK4Integrator( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc derivs_func, void* user_data);
        ~RK4Integrator();
        void step( double dt);
        void step();

    };

    class RK3_8Integrator : public FirstOrderODEVariableStepIntegrator {
        public:
        double *wstate[4];
        double *derivs[4];

        RK3_8Integrator( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc derivs_func, void* user_data);
        ~RK3_8Integrator();
        void step( double dt);
        void step();

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
        derivsFunc gderivs;
        derivsFunc fderivs;

    public:
        EulerCromerIntegrator(double dt, int N, double* xp[], double* vp[], derivsFunc g, derivsFunc f , void* user_data);
        ~EulerCromerIntegrator();
        void step( double dt);
        void step();
        void load();
        void unload();
    };

    // AdamsBashforthMoulton 4
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
    //     ABMIntegrator(int history_size, double dt, int N, double* in_vars[], double* out_vars[], derivsFunc derivs_func, void* user_data);
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
        ABM2Integrator( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc derivs_func, void* user_data);
        ~ABM2Integrator();
        void step();
        void undo_step();
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
        ABM4Integrator( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc derivs_func, void* user_data);
        ~ABM4Integrator();
        void step();
        void undo_step();
    };
}

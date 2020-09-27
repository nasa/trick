
#include <stdlib.h>
#include <iostream>
#include "SAIntegrator.hh"

// ------------------------------------------------------------

SA::FirstOrderODEIntegrator::FirstOrderODEIntegrator(double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
: Integrator(dt, user_data) {
    state_size = N;
    orig_vars = in_vars;
    dest_vars = out_vars;
    istate = new double[state_size];
    ostate = new double[state_size];
    derivs_func = func;
    reset=false;
}
SA::FirstOrderODEIntegrator::~FirstOrderODEIntegrator() {
    delete istate;
    delete ostate;
}
void SA::FirstOrderODEIntegrator::load() {
    reset = false;
    for (int i=0 ; i<state_size; i++ ) {
        istate[i] = *(orig_vars[i]);
    }
}
void SA::FirstOrderODEIntegrator::unload() {
    for (int i=0 ; i<state_size; i++ ) {
        *(dest_vars[i]) = ostate[i];
    }
}
void SA::FirstOrderODEIntegrator::undo_step() {
    if (!reset) {                           // If we've not already reset the integrator, then reset it.
        for (int i=0 ; i<state_size; i++ ) {
            *(orig_vars[i]) = istate[i]; // Copy istate values back to the state variables from whence they came.
        }
        time -= last_dt;                    // Undo the last time-step.
    }
    reset = true;
}

// ------------------------------------------------------------

SA::FirstOrderODEVariableStepIntegrator::FirstOrderODEVariableStepIntegrator( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
: FirstOrderODEIntegrator(dt, N, in_vars, out_vars, func, user_data) {}

void SA::FirstOrderODEVariableStepIntegrator::step( double dt ) {
     last_dt = dt; time += dt;
}

// ------------------------------------------------------------

SA::EulerIntegrator::EulerIntegrator(double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
    : FirstOrderODEVariableStepIntegrator(dt, N, in_vars, out_vars, func, user_data)
{
    derivs = new double[N];
}
SA::EulerIntegrator::~EulerIntegrator() {
    delete derivs;
}
void SA::EulerIntegrator::step( double dt) {
    (*derivs_func)( time, istate, derivs, udata);
    for (unsigned int i=0; i<state_size; i++) {
        ostate[i] = istate[i] + derivs[i] * dt;
    }
    SA::FirstOrderODEVariableStepIntegrator::step(dt);
}
void SA::EulerIntegrator::step() {
    step(default_dt);
}

// ------------------------------------------------------------

SA::HeunsMethod::HeunsMethod( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
    : FirstOrderODEVariableStepIntegrator(dt, N, in_vars, out_vars, func, user_data)
{
    wstate = new double[N];
    for (unsigned int i = 0; i < 2 ; i++) {
        derivs[i] = new double[N];
    }
}
SA::HeunsMethod::~HeunsMethod() {
    delete wstate;
    for (unsigned int i = 0; i < 2 ; i++) {
        delete derivs[i];
    }
}
void SA::HeunsMethod::step( double dt) {
    (*derivs_func)( time, istate, derivs[0], udata);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[i] = istate[i] + dt * derivs[0][i];
    }
    (*derivs_func)( time, wstate, derivs[1], udata);
    for (unsigned int i=0; i<state_size; i++) {
        ostate[i] = istate[i] + (dt/2) * ( derivs[0][i] +  derivs[1][i] );
    }
    SA::FirstOrderODEVariableStepIntegrator::step(dt);
}
void SA::HeunsMethod::step() {
    step(default_dt);
}

// ------------------------------------------------------------

SA::RK2Integrator::RK2Integrator( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
    : FirstOrderODEVariableStepIntegrator(dt, N, in_vars, out_vars, func, user_data)
{
    wstate = new double[N];
    for (unsigned int i = 0; i < 2 ; i++) {
        derivs[i] = new double[N];
    }
}
SA::RK2Integrator::~RK2Integrator() {
    delete (wstate);
    for (unsigned int i = 0; i < 2 ; i++) {
        delete derivs[i];
    }
}
void SA::RK2Integrator::step( double dt) {
    (*derivs_func)( time, istate, derivs[0], udata);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[i] = istate[i] + 0.5 * dt * derivs[0][i];
    }
    (*derivs_func)( time + 0.5 * dt , wstate, derivs[1], udata);
    for (unsigned int i=0; i<state_size; i++) {
        ostate[i] = istate[i] + dt * derivs[1][i];
    }
    SA::FirstOrderODEVariableStepIntegrator::step(dt);
}
void SA::RK2Integrator::step() {
    step(default_dt);
}

// ------------------------------------------------------------

SA::RK4Integrator::RK4Integrator( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
    : FirstOrderODEVariableStepIntegrator(dt, N, in_vars, out_vars, func, user_data)
{
    for (unsigned int i = 0; i < 3 ; i++) {
        wstate[i] = new double[N];
    }
    for (unsigned int i = 0; i < 4 ; i++) {
        derivs[i] = new double[N];
    }
}
SA::RK4Integrator::~RK4Integrator() {
    for (unsigned int i = 0; i < 3 ; i++) {
        delete (wstate[i]);
    }
    for (unsigned int i = 0; i < 4 ; i++) {
        delete (derivs[i]);
    }
}
void SA::RK4Integrator::step( double dt) {
    (*derivs_func)( time, istate, derivs[0], udata);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[0][i] = istate[i] + 0.5 * derivs[0][i] * dt;
    }
    (*derivs_func)( time + 0.5 * dt , wstate[0], derivs[1], udata);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[1][i] = istate[i] + 0.5 * derivs[1][i] * dt;
    }
    (*derivs_func)( time + 0.5 * dt , wstate[1], derivs[2], udata);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[2][i] = istate[i] + derivs[2][i] * dt;
    }
    (*derivs_func)( time + dt , wstate[2], derivs[3], udata);
    for (unsigned int i=0; i<state_size; i++) {
        ostate[i] = istate[i] + ((1/6.0)* derivs[0][i] +
                                 (1/3.0)* derivs[1][i] +
                                 (1/3.0)* derivs[2][i] +
                                 (1/6.0)* derivs[3][i]) * dt;
    }
    SA::FirstOrderODEVariableStepIntegrator::step(dt);
}
void SA::RK4Integrator::step() {
    step(default_dt);
}

// ------------------------------------------------------------

SA::RK3_8Integrator::RK3_8Integrator( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
    : FirstOrderODEVariableStepIntegrator(dt, N, in_vars, out_vars ,func, user_data)
{
    for (unsigned int i = 0; i < 3 ; i++) {
        wstate[i] = new double[N];
    }
    for (unsigned int i = 0; i < 4 ; i++) {
        derivs[i] = new double[N];
    }
}
SA::RK3_8Integrator::~RK3_8Integrator() {
    for (unsigned int i = 0; i < 3 ; i++) {
        delete (wstate[i]);
    }
    for (unsigned int i = 0; i < 4 ; i++) {
        delete (derivs[i]);
    }
}
void SA::RK3_8Integrator::step( double dt) {
    (*derivs_func)( time, istate, derivs[0], udata);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[0][i] = istate[i] + dt * (1/3.0) * derivs[0][i];
    }
    (*derivs_func)( time + (1/3.0) * dt , wstate[0], derivs[1], udata);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[1][i] = istate[i] + dt * ((-1/3.0) * derivs[0][i] + derivs[1][i]);
    }
    (*derivs_func)( time + (2/3.0) * dt , wstate[1], derivs[2], udata);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[2][i] = istate[i] + dt * (derivs[0][i] - derivs[1][i] + derivs[2][i]);
    }
    (*derivs_func)( time + dt , wstate[2], derivs[3], udata);
    for (unsigned int i=0; i<state_size; i++) {
        ostate[i] = istate[i] + ((1/8.0)* derivs[0][i] +
                                 (3/8.0)* derivs[1][i] +
                                 (3/8.0)* derivs[2][i] +
                                 (1/8.0)* derivs[3][i]) * dt;
    }
    SA::FirstOrderODEVariableStepIntegrator::step(dt);
}
void SA::RK3_8Integrator::step() {
    step(default_dt);
}

// ------------------------------------------------------------

SA::EulerCromerIntegrator::EulerCromerIntegrator(double dt, int N, double* xp[], double* vp[], derivsFunc gfunc, derivsFunc ffunc, void* user_data)
: Integrator(dt, user_data) {
    state_size  = N;
    x_p   = new double*[N];
    v_p   = new double*[N];
    x_in  = new double[N];
    v_in  = new double[N];
    g_out = new double[N];
    f_out = new double[N];
    x_out = new double[N];
    v_out = new double[N];
    for (int i=0 ; i<N; i++ ) {
        x_p[i] = xp[i];
        v_p[i] = vp[i];
    }
    gderivs = gfunc;
    fderivs = ffunc;
}
SA::EulerCromerIntegrator::~EulerCromerIntegrator() {
    delete x_p;
    delete v_p;
    delete x_in;
    delete v_in;
    delete g_out;
    delete f_out;
    delete x_out;
    delete v_out;
}
void SA::EulerCromerIntegrator::step( double dt) {
    (*gderivs)( time, x_in, g_out, udata);
    for (int i=0 ; i<state_size; i++ ) {
        v_out[i] = v_in[i] + g_out[i] * dt;
    }
    (*fderivs)( time, v_in, f_out, udata);
    for (int i=0 ; i<state_size; i++ ) {
        x_out[i] = x_in[i] + f_out[i] * dt;
    }
    time += dt;
}
void SA::EulerCromerIntegrator::step() {
    step(default_dt);
}
void SA::EulerCromerIntegrator::load() {
    for (int i=0 ; i<state_size; i++ ) {
        x_in[i] = *(x_p[i]);
        v_in[i] = *(v_p[i]);
    }
}
void SA::EulerCromerIntegrator::unload(){
    for (int i=0 ; i<state_size; i++ ) {
        *(x_p[i]) = x_out[i];
        *(v_p[i]) = v_out[i];
    }
}

// // Adams-Bashforth Coefficients
// static const double ABCoeffs[5][5] = {
//   {1.0,           0.0,           0.0,          0.0,           0.0},
//   {(3/2.0),      (-1/2.0),       0.0,          0.0,           0.0},
//   {(23/12.0),    (-16/12.0),    (5/12.0),      0.0,           0.0},
//   {(55/24.0),    (-59/24.0),    (37/24.0),    (-9/24.0),      0.0},
//   {(1901/720.0), (-2774/720.0), (2616/720.0), (-1274/720.0), (251/720.0)}
// };
//
// // Adams-Moulton Coefficients
// static const double AMCoeffs[5][5] = {
//   {1.0,          0.0,         0.0,          0.0,         0.0},
//   {(1/2.0),     (1/2.0),      0.0,          0.0,         0.0},
//   {(5/12.0),    (8/12.0),    (-1/12.0),     0.0,         0.0},
//   {(9/24.0),    (19/24.0),   (-5/24.0),    (1/24.0),     0.0},
//   {(251/720.0), (646/720.0), (-264/720.0), (106/720.0), (-19/720.0)}
// };
// SA::ABMIntegrator::ABMIntegrator ( int history_size, double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
//     : FirstOrderODEIntegrator(dt, N, in_vars, out_vars ,func, user_data) {
//
//     algorithmHistorySize=history_size; // The amount of history that we intend to use in this ABM integrator.
//     bufferSize=algorithmHistorySize+1; // The size of the buffer needs to be one more than the history so that an integration step can be reset.
//     hix=0;
//     currentHistorySize=0;            // How much derivative history is currently stored int the history buffer. Initially there will be none until we've primed the integrator.
//     deriv_history = new double*[bufferSize];
//     for (unsigned int i=0; i<bufferSize ; i++) {
//         deriv_history[i] = new double[state_size];
//     }
//     composite_deriv = new double[state_size];
// }
// SA::ABMIntegrator::~ABMIntegrator() {
//     for (int i=0; i<bufferSize ; i++) {
//         delete (deriv_history[i]);
//     }
//     delete(deriv_history);
//     delete(composite_deriv);
// }
// void SA::ABMIntegrator::step() {
//
//     hix = (hix+1)%bufferSize;                          // Move history index forward
//     (*derivs_func)( time, istate, deriv_history[hix], udata); // Calculated and store the deriv for current, corrected state.
//     // Increase the size of the stored history, up to the limit specified by the user.
//     currentHistorySize = (currentHistorySize < algorithmHistorySize) ? currentHistorySize+1 : algorithmHistorySize;
//     // (Predictor) Predict the next state using the Adams-Bashforth explicit method.
//     for (int i=0; i<state_size; i++) {
//         composite_deriv[i] = 0.0;
//         for (int n=0,j=hix; n<currentHistorySize ; n++,j=(j+bufferSize-1)%bufferSize) {
//             composite_deriv[i] += ABCoeffs[currentHistorySize-1][n] * deriv_history[j][i];
//         }
//         ostate[i] = istate[i] + default_dt * composite_deriv[i];
//     }
//     // Move history index forward, so we can temporarily store the derivative of the predicted next state.
//     // We do not increase the currentHistorySize.
//     hix = (hix+1)%bufferSize;
//     (*derivs_func)( time, ostate, deriv_history[hix], udata); // Calc deriv for predicted next state.
//     // (Corrector) Refine the next state using the Adams-Moulton implicit method. This is the corrected next state.
//     for (int i=0; i<state_size; i++) {
//         composite_deriv[i] = 0.0;
//         for (int n=0,j=hix; n<currentHistorySize ; n++,j=(j+bufferSize-1)%bufferSize) {
//             composite_deriv[i] += AMCoeffs[currentHistorySize-1][n] * deriv_history[j][i];
//         }
//         ostate[i] = istate[i] + default_dt * composite_deriv[i];
//     }
//     // Move history index backward, so we over-write the predicted state with the corrected state on our next step().
//     hix = (hix+bufferSize-1)%bufferSize;
//     SA::Integrator::step();
// }
//
// void SA::ABMIntegrator::undo_step() {
//     hix = (hix+bufferSize-1)%bufferSize;
//     FirstOrderODEIntegrator::undo_step();
// }

// =======================================================================
static const double AB2Coeffs[2] = {(3/2.0), (-1/2.0)};
static const double AM2Coeffs[2] = {(1/2.0),  (1/2.0)};

SA::ABM2Integrator::ABM2Integrator ( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
    : FirstOrderODEIntegrator(dt, N, in_vars, out_vars ,func, user_data) {
    // The amount of history that we intend to use in this ABM integrator.
    bufferSize=2+1; // The size of the buffer needs to be one more than the history so that an integration step can be reset.
    hix=0;
    currentHistorySize=0;            // How much derivative history is currently stored int the history buffer. Initially there will be none until we've primed the integrator.
    deriv_history = new double*[bufferSize];
    for (unsigned int i=0; i<bufferSize ; i++) {
        deriv_history[i] = new double[state_size];
    }
    composite_deriv = new double[state_size];

    // NEW: Create a priming integrator.
    double** priming_integrator_in_vars = new double*[state_size];
    for (unsigned int i=0; i<N ; i++) {
        priming_integrator_in_vars[i] = &(istate[i]);
    }
    double** priming_integrator_out_vars = new double*[state_size];
    for (unsigned int i=0; i<N ; i++) {
        priming_integrator_out_vars[i] = &(ostate[i]);
    }
    priming_integrator = new SA::RK2Integrator(dt, N, priming_integrator_in_vars, priming_integrator_out_vars, func, user_data);
}
SA::ABM2Integrator::~ABM2Integrator() {
    for (int i=0; i<bufferSize ; i++) {
        delete (deriv_history[i]);
    }
    delete(deriv_history);
    delete(composite_deriv);
    delete priming_integrator;
}
void SA::ABM2Integrator::step() {

    hix = (hix+1)%bufferSize;                          // Move history index forward
    (*derivs_func)( time, istate, deriv_history[hix], udata); // Calculated and store the deriv for current, corrected state.
    // Increase the size of the stored history, up to the limit specified by the user.
    currentHistorySize = (currentHistorySize < 2) ? currentHistorySize+1 : 2;

    if ( currentHistorySize < 2 ) {
        priming_integrator->load();
        priming_integrator->step();
        priming_integrator->unload();
    } else {
        // (Predictor) Predict the next state using the Adams-Bashforth explicit method.
        for (int i=0; i<state_size; i++) {
            composite_deriv[i] = 0.0;
            for (int n=0,j=hix; n<2 ; n++,j=(j+bufferSize-1)%bufferSize) {
                composite_deriv[i] += AB2Coeffs[n] * deriv_history[j][i];
            }
            ostate[i] = istate[i] + default_dt * composite_deriv[i];
        }
        // Move history index forward, so we can temporarily store the derivative of the predicted next state.
        // We do not increase the currentHistorySize.
        hix = (hix+1)%bufferSize;
        (*derivs_func)( time, ostate, deriv_history[hix], udata); // Calc deriv for predicted next state.
        // (Corrector) Refine the next state using the Adams-Moulton implicit method. This is the corrected next state.
        for (int i=0; i<state_size; i++) {
            composite_deriv[i] = 0.0;
            for (int n=0,j=hix; n<2 ; n++,j=(j+bufferSize-1)%bufferSize) {
                composite_deriv[i] += AM2Coeffs[n] * deriv_history[j][i];
            }
            ostate[i] = istate[i] + default_dt * composite_deriv[i];
        }
        // Move history index backward, so we over-write the predicted state with the corrected state on our next step().
        hix = (hix+bufferSize-1)%bufferSize;
    }
    SA::Integrator::step();
}
void SA::ABM2Integrator::undo_step() {
    hix = (hix+bufferSize-1)%bufferSize;
    FirstOrderODEIntegrator::undo_step();
}

// =======================================================================
static const double AB4Coeffs[4] = {(55/24.0), (-59/24.0), (37/24.0), (-9/24.0)};
static const double AM4Coeffs[4] = {(9/24.0), (19/24.0), (-5/24.0), (1/24.0)};

SA::ABM4Integrator::ABM4Integrator ( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
    : FirstOrderODEIntegrator(dt, N, in_vars, out_vars ,func, user_data) {

    // The amount of history that we intend to use in this ABM integrator.
    bufferSize=4+1; // The size of the buffer needs to be one more than the history so that an integration step can be reset.
    hix=0;
    currentHistorySize=0;            // How much derivative history is currently stored int the history buffer. Initially there will be none until we've primed the integrator.
    deriv_history = new double*[bufferSize];
    for (unsigned int i=0; i<bufferSize ; i++) {
        deriv_history[i] = new double[state_size];
    }
    composite_deriv = new double[state_size];

    // NEW: Create a priming integrator.
    double** priming_integrator_in_vars = new double*[state_size];
    for (unsigned int i=0; i<N ; i++) {
        priming_integrator_in_vars[i] = &(istate[i]);
    }
    double** priming_integrator_out_vars = new double*[state_size];
    for (unsigned int i=0; i<N ; i++) {
        priming_integrator_out_vars[i] = &(ostate[i]);
    }
    priming_integrator = new SA::RK4Integrator(dt, N, priming_integrator_in_vars, priming_integrator_out_vars, func, user_data);
}

SA::ABM4Integrator::~ABM4Integrator() {
    for (int i=0; i<bufferSize ; i++) {
        delete (deriv_history[i]);
    }
    delete(deriv_history);
    delete(composite_deriv);
    delete priming_integrator;
}

void SA::ABM4Integrator::step() {

    hix = (hix+1)%bufferSize;                          // Move history index forward
    (*derivs_func)( time, istate, deriv_history[hix], udata); // Calculated and store the deriv for current, corrected state.
    // Increase the size of the stored history, up to the limit specified by the user.
    currentHistorySize = (currentHistorySize < 4) ? currentHistorySize+1 : 4;
    if ( currentHistorySize < 4 ) {
        priming_integrator->load();
        priming_integrator->step();
        priming_integrator->unload();
    } else {
        // (Predictor) Predict the next state using the Adams-Bashforth explicit method.
        for (int i=0; i<state_size; i++) {
            composite_deriv[i] = 0.0;
            for (int n=0,j=hix; n<4 ; n++,j=(j+bufferSize-1)%bufferSize) {
                composite_deriv[i] += AB4Coeffs[n] * deriv_history[j][i];
            }
            ostate[i] = istate[i] + default_dt * composite_deriv[i];
        }
        // Move history index forward, so we can temporarily store the derivative of the predicted next state.
        // We do not increase the currentHistorySize.
        hix = (hix+1)%bufferSize;
        (*derivs_func)( time, ostate, deriv_history[hix], udata); // Calc deriv for predicted next state.
        // (Corrector) Refine the next state using the Adams-Moulton implicit method. This is the corrected next state.
        for (int i=0; i<state_size; i++) {
            composite_deriv[i] = 0.0;
            for (int n=0,j=hix; n<4 ; n++,j=(j+bufferSize-1)%bufferSize) {
                composite_deriv[i] += AM4Coeffs[n] * deriv_history[j][i];
            }
            ostate[i] = istate[i] + default_dt * composite_deriv[i];
        }
        // Move history index backward, so we over-write the predicted state with the corrected state on our next step().
        hix = (hix+bufferSize-1)%bufferSize;
    }
    SA::Integrator::step();
}
void SA::ABM4Integrator::undo_step() {
    hix = (hix+bufferSize-1)%bufferSize;
    FirstOrderODEIntegrator::undo_step();
}


#include <stdlib.h>
#include <iostream>
#include "SAIntegrator.hh"

void SA::Integrator::step() { X_out = X_in + default_h; }
void SA::Integrator::load() { X_in = X_out; }
void SA::Integrator::unload() {}
double SA::Integrator::getIndyVar() { return X_out; }
void SA::Integrator::setIndyVar(double v) { X_out = v; }

// ------------------------------------------------------------
SA::FirstOrderODEIntegrator::FirstOrderODEIntegrator(
    double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata)
: Integrator(h, udata) {
    state_size = N;
    inVars = in_vars;
    outVars = out_vars;
    inState = new double[state_size];
    outState = new double[state_size];
    derivs_func = dfunc;
}
SA::FirstOrderODEIntegrator::~FirstOrderODEIntegrator() {
    delete inState;
    delete outState;
}
void SA::FirstOrderODEIntegrator::load() {
    if (inVars != NULL) {
        for (unsigned int i=0 ; i<state_size; i++ ) {
            inState[i] = *(inVars[i]);
        }
        Integrator::load();
    } else {
        std::cerr << "Error: SA::FirstOrderODEIntegrator::load(). inVars is not set." << std::endl;
    }
}
void SA::FirstOrderODEIntegrator::unload() {
    if (outVars != NULL) {
        for (unsigned int i=0 ; i<state_size; i++ ) {
            *(outVars[i]) = outState[i];
        }
    } else {
        std::cerr << "Error: SA::FirstOrderODEIntegrator::load(). outVars is not set." << std::endl;
    }
}
void SA::FirstOrderODEIntegrator::load_from_outState(){
    for (unsigned int i=0 ; i<state_size; i++ ) {
        inState[i] = outState[i];
    }
    Integrator::load();
}
double** SA::FirstOrderODEIntegrator::set_in_vars( double* in_vars[]){
    double **ret = inVars;
    inVars = in_vars;
    return (ret);
}
double** SA::FirstOrderODEIntegrator::set_out_vars( double* out_vars[]){
    double **ret = outVars;
    outVars = out_vars;
    return (ret);
}
double SA::FirstOrderODEIntegrator::undo_step() {
        for (unsigned int i=0 ; i<state_size; i++ ) {
            *(inVars[i]) = inState[i];
        }
        X_out = X_in;
    return (last_h);
}
// ------------------------------------------------------------
SA::FirstOrderODEVariableStepIntegrator::FirstOrderODEVariableStepIntegrator(
    double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata)
: FirstOrderODEIntegrator(h, N, in_vars, out_vars, dfunc, udata) {
    root_finder = (RootFinder*) NULL;
    root_error_func = (RootErrorFunc)NULL;
}
SA::FirstOrderODEVariableStepIntegrator::~FirstOrderODEVariableStepIntegrator() {
    // User is responsible for deleting the root_finder.
}
void SA::FirstOrderODEVariableStepIntegrator::add_Rootfinder(
    RootFinder* rootFinder, RootErrorFunc rfunc) {
    root_finder = rootFinder;
    root_error_func = rfunc;
}
void SA::FirstOrderODEVariableStepIntegrator::variable_step( double h) {
    last_h = h; X_out = X_in + h;
}
void SA::FirstOrderODEVariableStepIntegrator::find_roots(double h, unsigned int depth) {
    double new_h;
    double h_correction = (*root_error_func)( getIndyVar(), outState, root_finder, user_data );
    if (h_correction < 0.0) {
        while (h_correction != 0.0) {
            new_h = undo_step() + h_correction;
            variable_step(new_h);
            h_correction = (*root_error_func)( getIndyVar(), outState, root_finder, user_data );
        }
        load_from_outState();
        variable_step(h-new_h);
        if (depth > 0) {
            find_roots(h-new_h, depth-1);
        }
    }
}
void SA::FirstOrderODEVariableStepIntegrator::step() {
    variable_step( default_h );
    if ((root_finder != NULL) && (root_error_func != NULL)) {
        find_roots( default_h, 5 );
    }
}
// ------------------------------------------------------------
SA::EulerIntegrator::EulerIntegrator(
    double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata)
    : FirstOrderODEVariableStepIntegrator(h, N, in_vars, out_vars, dfunc, udata) {
    derivs = new double[N];
}
SA::EulerIntegrator::~EulerIntegrator() {
    delete derivs;
}
void SA::EulerIntegrator::variable_step( double h) {
    (*derivs_func)( X_in, inState, derivs, user_data);
    for (unsigned int i=0; i<state_size; i++) {
        outState[i] = inState[i] + derivs[i] * h;
    }
    SA::FirstOrderODEVariableStepIntegrator::variable_step(h);
}
// ------------------------------------------------------------
SA::HeunsMethod::HeunsMethod(
    double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata)
    : FirstOrderODEVariableStepIntegrator(h, N, in_vars, out_vars, dfunc, udata) {
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
void SA::HeunsMethod::variable_step( double h) {
    (*derivs_func)( X_in, inState, derivs[0], user_data);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[i] = inState[i] + h * derivs[0][i];
    }
    (*derivs_func)( X_in, wstate, derivs[1], user_data);
    for (unsigned int i=0; i<state_size; i++) {
        outState[i] = inState[i] + (h/2) * ( derivs[0][i] +  derivs[1][i] );
    }
    SA::FirstOrderODEVariableStepIntegrator::variable_step(h);
}
// ------------------------------------------------------------
SA::RK2Integrator::RK2Integrator(
    double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata)
    : FirstOrderODEVariableStepIntegrator(h, N, in_vars, out_vars, dfunc, udata) {
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
void SA::RK2Integrator::variable_step( double h) {
    (*derivs_func)( X_in, inState, derivs[0], user_data);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[i] = inState[i] + 0.5 * h * derivs[0][i];
    }
    (*derivs_func)( X_in + 0.5 * h , wstate, derivs[1], user_data);
    for (unsigned int i=0; i<state_size; i++) {
        outState[i] = inState[i] + h * derivs[1][i];
    }
    SA::FirstOrderODEVariableStepIntegrator::variable_step(h);
}
// ------------------------------------------------------------
SA::RK4Integrator::RK4Integrator(
    double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata)
    : FirstOrderODEVariableStepIntegrator(h, N, in_vars, out_vars, dfunc, udata) {
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
void SA::RK4Integrator::variable_step( double h) {
    (*derivs_func)( X_in, inState, derivs[0], user_data);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[0][i] = inState[i] + 0.5 * derivs[0][i] * h;
    }
    (*derivs_func)( X_in + 0.5 * h , wstate[0], derivs[1], user_data);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[1][i] = inState[i] + 0.5 * derivs[1][i] * h;
    }
    (*derivs_func)( X_in + 0.5 * h , wstate[1], derivs[2], user_data);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[2][i] = inState[i] + derivs[2][i] * h;
    }
    (*derivs_func)( X_in + h , wstate[2], derivs[3], user_data);
    for (unsigned int i=0; i<state_size; i++) {
        outState[i] = inState[i] + ((1/6.0)* derivs[0][i] +
                                 (1/3.0)* derivs[1][i] +
                                 (1/3.0)* derivs[2][i] +
                                 (1/6.0)* derivs[3][i]) * h;
    }
    SA::FirstOrderODEVariableStepIntegrator::variable_step(h);
}
// ------------------------------------------------------------
SA::RK3_8Integrator::RK3_8Integrator(
    double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata)
    : FirstOrderODEVariableStepIntegrator(h, N, in_vars, out_vars ,dfunc, udata) {
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
void SA::RK3_8Integrator::variable_step( double h) {
    (*derivs_func)( X_in, inState, derivs[0], user_data);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[0][i] = inState[i] + h * (1/3.0) * derivs[0][i];
    }
    (*derivs_func)( X_in + (1/3.0) * h , wstate[0], derivs[1], user_data);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[1][i] = inState[i] + h * ((-1/3.0) * derivs[0][i] + derivs[1][i]);
    }
    (*derivs_func)( X_in + (2/3.0) * h , wstate[1], derivs[2], user_data);
    for (unsigned int i=0; i<state_size; i++) {
        wstate[2][i] = inState[i] + h * (derivs[0][i] - derivs[1][i] + derivs[2][i]);
    }
    (*derivs_func)( X_in + h , wstate[2], derivs[3], user_data);
    for (unsigned int i=0; i<state_size; i++) {
        outState[i] = inState[i] + ((1/8.0)* derivs[0][i] +
                                 (3/8.0)* derivs[1][i] +
                                 (3/8.0)* derivs[2][i] +
                                 (1/8.0)* derivs[3][i]) * h;
    }
    SA::FirstOrderODEVariableStepIntegrator::variable_step(h);
}
// ------------------------------------------------------------
SA::EulerCromerIntegrator::EulerCromerIntegrator(
    double dt, int N, double* xp[], double* vp[], DerivsFunc gfunc, DerivsFunc ffunc, void* udata)
    :Integrator(dt, udata) {
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
    (*gderivs)( X_in, x_in, g_out, user_data);
    for (int i=0 ; i<state_size; i++ ) {
        v_out[i] = v_in[i] + g_out[i] * dt;
    }
    (*fderivs)( X_in, v_in, f_out, user_data);
    for (int i=0 ; i<state_size; i++ ) {
        x_out[i] = x_in[i] + f_out[i] * dt;
    }
    X_out = X_in + dt;
}
void SA::EulerCromerIntegrator::step() {
    step(default_h);
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
// SA::ABMIntegrator::ABMIntegrator ( int history_size, double h, int N, double* in_vars[], double* out_vars[], DerivsFunc func, void* udata)
//     : FirstOrderODEIntegrator(h, N, in_vars, out_vars ,func, udata) {
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
//     // (*derivs_func)( indyVar, inState, deriv_history[hix], user_data);
//     (*derivs_func)( X_in, inState, deriv_history[hix], user_data); // Calculated and store the deriv for current, corrected state.
//     // Increase the size of the stored history, up to the limit specified by the user.
//     currentHistorySize = (currentHistorySize < algorithmHistorySize) ? currentHistorySize+1 : algorithmHistorySize;
//     // (Predictor) Predict the next state using the Adams-Bashforth explicit method.
//     for (int i=0; i<state_size; i++) {
//         composite_deriv[i] = 0.0;
//         for (int n=0,j=hix; n<currentHistorySize ; n++,j=(j+bufferSize-1)%bufferSize) {
//             composite_deriv[i] += ABCoeffs[currentHistorySize-1][n] * deriv_history[j][i];
//         }
//         outState[i] = inState[i] + default_h * composite_deriv[i];
//     }
//     // Move history index forward, so we can temporarily store the derivative of the predicted next state.
//     // We do not increase the currentHistorySize.
//     hix = (hix+1)%bufferSize;
//     (*derivs_func)( X_out, outState, deriv_history[hix], user_data); // Calc deriv for predicted next state.
//     // (Corrector) Refine the next state using the Adams-Moulton implicit method. This is the corrected next state.
//     for (int i=0; i<state_size; i++) {
//         composite_deriv[i] = 0.0;
//         for (int n=0,j=hix; n<currentHistorySize ; n++,j=(j+bufferSize-1)%bufferSize) {
//             composite_deriv[i] += AMCoeffs[currentHistorySize-1][n] * deriv_history[j][i];
//         }
//         outState[i] = inState[i] + default_h * composite_deriv[i];
//     }
//     // Move history index backward, so we over-write the predicted state with the corrected state on our next step().
//     hix = (hix+bufferSize-1)%bufferSize;
//     SA::Integrator::step();
// }
//
// double SA::ABMIntegrator::undo_step() {
//     hix = (hix+bufferSize-1)%bufferSize;
//     return (FirstOrderODEIntegrator::undo_step());
// }

// =======================================================================
static const double AB2Coeffs[2] = {(3/2.0), (-1/2.0)};
static const double AM2Coeffs[2] = {(1/2.0),  (1/2.0)};

SA::ABM2Integrator::ABM2Integrator (
    double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata)
    : FirstOrderODEIntegrator(h, N, in_vars, out_vars, dfunc, udata) {
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
        priming_integrator_in_vars[i] = &(inState[i]);
    }
    double** priming_integrator_out_vars = new double*[state_size];
    for (unsigned int i=0; i<N ; i++) {
        priming_integrator_out_vars[i] = &(outState[i]);
    }
    priming_integrator = new SA::RK2Integrator(h, N, priming_integrator_in_vars, priming_integrator_out_vars, dfunc, udata);
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
    (*derivs_func)( X_in, inState, deriv_history[hix], user_data); // Calculated and store the deriv for current, corrected state.
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
            outState[i] = inState[i] + default_h * composite_deriv[i];
        }
        // Move history index forward, so we can temporarily store the derivative of the predicted next state.
        // We do not increase the currentHistorySize.
        hix = (hix+1)%bufferSize;
        (*derivs_func)( X_in, outState, deriv_history[hix], user_data); // Calc deriv for predicted next state.
        // (Corrector) Refine the next state using the Adams-Moulton implicit method. This is the corrected next state.
        for (int i=0; i<state_size; i++) {
            composite_deriv[i] = 0.0;
            for (int n=0,j=hix; n<2 ; n++,j=(j+bufferSize-1)%bufferSize) {
                composite_deriv[i] += AM2Coeffs[n] * deriv_history[j][i];
            }
            outState[i] = inState[i] + default_h * composite_deriv[i];
        }
        // Move history index backward, so we over-write the predicted state with the corrected state on our next step().
        hix = (hix+bufferSize-1)%bufferSize;
    }
    SA::Integrator::step();
}
double SA::ABM2Integrator::undo_step() {
    hix = (hix+bufferSize-1)%bufferSize;
    return ( FirstOrderODEIntegrator::undo_step());
}
// =======================================================================
static const double AB4Coeffs[4] = {(55/24.0), (-59/24.0), (37/24.0), (-9/24.0)};
static const double AM4Coeffs[4] = {(9/24.0), (19/24.0), (-5/24.0), (1/24.0)};

SA::ABM4Integrator::ABM4Integrator (
    double h, int N, double* in_vars[], double* out_vars[], DerivsFunc dfunc, void* udata)
    : FirstOrderODEIntegrator(h, N, in_vars, out_vars, dfunc, udata) {

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
        priming_integrator_in_vars[i] = &(inState[i]);
    }
    double** priming_integrator_out_vars = new double*[state_size];
    for (unsigned int i=0; i<N ; i++) {
        priming_integrator_out_vars[i] = &(outState[i]);
    }
    priming_integrator = new SA::RK4Integrator(h, N, priming_integrator_in_vars, priming_integrator_out_vars, dfunc, udata);
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
    (*derivs_func)( X_in, inState, deriv_history[hix], user_data); // Calculated and store the deriv for current, corrected state.
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
            outState[i] = inState[i] + default_h * composite_deriv[i];
        }
        // Move history index forward, so we can temporarily store the derivative of the predicted next state.
        // We do not increase the currentHistorySize.
        hix = (hix+1)%bufferSize;
        (*derivs_func)( X_out, outState, deriv_history[hix], user_data); // Calc deriv for predicted next state.
        // (Corrector) Refine the next state using the Adams-Moulton implicit method. This is the corrected next state.
        for (int i=0; i<state_size; i++) {
            composite_deriv[i] = 0.0;
            for (int n=0,j=hix; n<4 ; n++,j=(j+bufferSize-1)%bufferSize) {
                composite_deriv[i] += AM4Coeffs[n] * deriv_history[j][i];
            }
            outState[i] = inState[i] + default_h * composite_deriv[i];
        }
        // Move history index backward, so we over-write the predicted state with the corrected state on our next step().
        hix = (hix+bufferSize-1)%bufferSize;
    }
    SA::Integrator::step();
}
double SA::ABM4Integrator::undo_step() {
    hix = (hix+bufferSize-1)%bufferSize;
    return (FirstOrderODEIntegrator::undo_step());
}

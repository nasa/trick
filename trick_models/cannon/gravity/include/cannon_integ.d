#define NUM_STEP 12
#define NUM_VARIABLES 4

INTEGRATOR.state = alloc(NUM_VARIABLES) ;
INTEGRATOR.deriv = alloc(NUM_STEP) ;
INTEGRATOR.state_ws = alloc(NUM_STEP) ;

for (int kk = 0 ; kk < NUM_STEP ; kk++ ) {
    INTEGRATOR.deriv[kk] = alloc(NUM_VARIABLES) ;
    INTEGRATOR.state_ws[kk] = alloc(NUM_VARIABLES) ;
}

INTEGRATOR.stored_data = alloc(8) ;
for (int kk = 0 ; kk < 8 ; kk++ ) {
    INTEGRATOR.stored_data[kk] = alloc(NUM_VARIABLES) ;
}

INTEGRATOR.option = Runge_Kutta_2 ;
INTEGRATOR.init = True ;
INTEGRATOR.first_step_deriv = Yes ;
INTEGRATOR.num_state = NUM_VARIABLES ;

#undef NUM_STEP
#undef NUM_VARIABLES

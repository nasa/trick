/* 
 * PURPOSE:
 * ( )
*/
#ifndef INTEG_ALGORITHMS_HH
#define INTEG_ALGORITHMS_HH

#ifndef SWIG

#if USE_ER7_UTILS_INTEGRATORS
  #include "../er7_algorithms/ABM_Integrator.hh"
  #include "../er7_algorithms/Euler_Cromer_Integrator.hh"
  #include "../er7_algorithms/Euler_Integrator.hh"
  #include "../er7_algorithms/MM4_Integrator.hh"
  #include "../er7_algorithms/NL2_Integrator.hh"
  #include "../er7_algorithms/RK2_Integrator.hh"
  #include "../er7_algorithms/RK4_Integrator.hh"
  #include "../er7_algorithms/RKF45_Integrator.hh"
  #include "../er7_algorithms/RKF78_Integrator.hh"
  #include "../er7_algorithms/RKG4_Integrator.hh"
#else
  #include "../trick_algorithms/ABM_Integrator.hh"
  #include "../trick_algorithms/Euler_Cromer_Integrator.hh"
  #include "../trick_algorithms/Euler_Integrator.hh"
  #include "../trick_algorithms/MM4_Integrator.hh"
  #include "../trick_algorithms/NL2_Integrator.hh"
  #include "../trick_algorithms/RK2_Integrator.hh"
  #include "../trick_algorithms/RK4_Integrator.hh"
  #include "../trick_algorithms/RKF45_Integrator.hh"
  #include "../trick_algorithms/RKF78_Integrator.hh"
  #include "../trick_algorithms/RKG4_Integrator.hh"
#endif

#endif

#endif

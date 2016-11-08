#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "regula_falsi.h"

int main( int argc, char*argv[]) {

  int root = 0;
  double t;
  double t_test;
  double t_next;
  double tgo;
  double dt = 0.2;
  REGULA_FALSI rf;

  reset_regula_falsi(t, &rf);
  rf.error_tol = 1.0e-15;
  rf.mode = Any;

  for (t=0.0; t<15.0; t+=dt) {

      t_next = t + dt;
      t_test = t;

      rf.error = sin( t_test);
      tgo = regula_falsi(t_test, &rf);
      while ( ( (t_test + tgo) < t_next) && !root) {
          t_test += tgo;
          rf.error = sin( t_test);
          tgo = regula_falsi(t_test, &rf);

          if (fabs(tgo) < rf.error_tol) {
              printf("ROOT @ %18.14g\n", t_test);
              root = 1;
              reset_regula_falsi(t_test, &rf);
          }
      }
      root = 0;
  }

  exit(0);

}

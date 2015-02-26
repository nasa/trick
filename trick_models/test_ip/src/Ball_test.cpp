
#include "test_ip/include/Ball_test.hh"

double Ball_alex::sdaa[3][3] ;
const double Ball_alex::scd = 1.2345 ;
const double Ball_alex::csd = 6.7890 ;
const double Ball_alex::sdc = 9.8765 ;

Ball_alex::Ball_alex() : cir(i), icr(i) {

    unsigned int ii ;

    ep = NULL ;
    epp = NULL ;

    for ( ii = 0 ; ii < 4 ; ii++ ) {
        eap[ii] = NULL ;
    }

}


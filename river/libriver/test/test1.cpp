/*
 * Sample program for using Trick's dp lib
 *
 * The program spits out the timeîand position of the ball L1 data
 *
 * To build, rebuild Log without Doug stuff:
 *     % setenv VR_CFLAGS "-UUSE_DOUG_H"
 *     % cd dataproducts/Log
 *     % make clean
 *     % make 
 *     % cd - ; # puts you back in this dir
 *     % make ; # builds test1 
*/

#include <string.h>

#include "../data_products/Log/DataRiverFactory.hh"

int main(int argc, char* argv[])
{

        DataRiver* tr ;
        char param[256] ;
        double time ;
        double* vals ;
        double* tstamps ;

        strcpy(param, "ball.state.out.position[0]");

        //tr = get_data_river(
                //"/users/vetter/sims/SIM_ball_L2/RUN_04/log_Ball1.000") ; 
        tr = get_data_river(
                "/users/vetter/sims/SIM_ball_L2/RUN_test/log_Ball1.trk") ; 
        //tr = get_data_river(
                //"/users/vetter/sims/SIM_ball_L2/RUN_test/log_testcsv.csv") ; 

        time = 264.775 ;

        printf("At time=%lf, idx=%d\n", time, tr->getIndexAtTime(&time));
        printf("fname=%s\n", tr->getFileName());
        printf("unit=%s for %s\n", tr->getUnit(param), param);
        printf("timeunit=%s\n", tr->getTimeUnit());
        printf("numpoints=%d\n", tr->getNumPoints());
        printf("First ten time/val pts for pos[0]\n");
        vals = tr->getVals(param);
        tstamps = tr->getTimeStamps();
        for ( int ii = 0 ; ii < 10 ; ii++ ) {
                printf("   %lf\t\t%lf\n", tstamps[ii], vals[ii]);
        }

        delete tr ;

        return 0 ;
}

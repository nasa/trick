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
#include <cstring>

#include "../DataRiverFactory.hh"
#include "../../libtvs/transformation.hh"

int main(int argc, char* argv[])
{

        DataRiver* tr ;
        char param[256] ;
        double time ;
        double* vals ;
        double* tstamps ;

        if (argc == 0) {
            cerr << "Usage: testriver input_file" << endl;
            exit(1);
        }
        vector<Transformation> transforms;
        //strcpy(param, "graphics.orbital.element[0].out.ecef_to_gmf.location[0]");
        strcpy(param,"sys.exec.out.time");
        tr = get_data_river(argv[1], transforms) ; 
        //tr = get_data_river("RUN_abort1a_las_active_doug.trk", transforms) ; 
        //tr = get_data_river("RUN_abort1a_las_active_doug.csv", transforms) ; 

        time = 4.775 ;

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

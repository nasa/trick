/*********************************** TRICK HEADER **************************
PURPOSE:          ( Monte Master Shutdown Job )
***************************************************************************/

#include "sim_services/UnitTest/include/trick_tests.h"
#include "sim_services/MonteCarlo/include/montecarlo_c_intf.h"

int cannon_master_shutdown()
{
    int answer;

    answer = mc_get_num_results();
    printf("answer is %d\n", answer);
    if (answer == 8) {
        add_test_result("Sim_Monte Simulation", "MonteCarlo Analysis", "");
    } else {
        const char str[20];
        sprintf(str, "Actual runs is %d should be 8\n", answer);
        add_test_result("Sim_Monte Simulation", "MonteCarlo Analysis", str);
    }
    trick_test_add_parent("Sim_Monte Simulation", "MonteCarlo Analysis", "1452306647");    
    call_write_output();
    return ( 0 ); 
}

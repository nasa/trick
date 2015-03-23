#ifndef SATGRAPHICSCOMM_HH
#define SATGRAPHICSCOMM_HH
/********************************* TRICK HEADER *******************************
PURPOSE:
      ()
      LIBRARY DEPENDENCY:
            ((../src/SatGraphicsComm.cpp))
PROGRAMMERS:
      (((John M. Penn) () () () ))
*******************************************************************************/
#include "trick_utils/comm/include/tc_proto.h"
#include "Satellite.hh"

class SatGraphicsComm {
    public:

    SatGraphicsComm();

    int connect();
    int send_packet(Satellite* sat);

    TCDevice connection;
    char* syscmd;
};
#endif

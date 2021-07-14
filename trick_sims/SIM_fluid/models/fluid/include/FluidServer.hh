/********************************* TRICK HEADER *******************************
PURPOSE: (Create a TCP server to transmit particle data to client)
*******************************************************************************/
#ifndef _fluidserver_hh_
#define _fluidserver_hh_
#include "Fluid.hh"

class FluidServer {
    private:
        int clientfd;
    public:
        void initConnection();
        void sendParticleData(Fluid* fluid);
};

#endif
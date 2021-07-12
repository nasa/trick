#include "Fluid.hh"

class FluidServer {
    public:
        FluidServer();
        int initConnection();
        void sendParticleData(int clientfd, Fluid* fluid);
}
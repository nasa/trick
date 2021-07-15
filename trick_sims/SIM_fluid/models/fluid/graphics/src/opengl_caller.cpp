#include <thread>
#include "../../include/Fluid.hh"

extern int openGLMain(Fluid* fluid);

void openGLCaller(Fluid* fluid) {
    {
        std::thread openGLThread(openGLMain, fluid);
        openGLThread.detach();
    }
}
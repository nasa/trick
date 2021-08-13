#include <thread>
#include "../../include/Fluid.hh"
#include "opengl.h"

void openGLCaller(Fluid* fluid) {
    {
        std::thread openGLThread(openGLMain, fluid);
        openGLThread.detach();
    }
}

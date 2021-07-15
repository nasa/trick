#include <thread>

extern int openGLMain();

void openGLCaller() {
    {
        std::thread openGLThread(openGLMain);
        openGLThread.detach();
    }
}
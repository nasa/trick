#include <thread>

extern int openGLMain(int argc, char* argv[]);

void openGLCaller(int argc, char* argv[]) {
    {
        std::thread openGLThread(openGLMain, argc, argv);
        openGLThread.detach();
    }
}
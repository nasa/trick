#include <thread>

extern int openGLMain(int argc, char* argv[]);

void openGLCaller() {
    /*int argc = 2;
    char* argv[] = {"./sim", "../100_sphere.obj"};
    {
        std::thread openGLThread(openGLMain, argc, argv);
        openGLThread.detach();
    }*/
    int x = 0;
    for (int i = 0; i < 10000000000; i++) {
        x+= i;
    }
}
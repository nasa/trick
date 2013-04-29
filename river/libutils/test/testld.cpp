#ifdef WIN32
#include "../port_dl.h"
#else
#include <dlfcn.h>
#endif
#include <iostream>
using std::cerr;
using std::endl;

int main( int narg, char** args ) 
{
    void* libptr = dlopen("../../../../../lib_Win32/goober.dll", RTLD_LAZY);
    cerr << "Open non-existent dll fails - ";
    if (libptr) {
        cerr << "FAIL" << endl;
        return -1;
    }
    cerr << "PASS" << endl;

#if defined(WIN32)
    libptr = dlopen("../../../../../lib_Win32/tk85.dll", RTLD_LAZY);
#else if defined(__LINUX)
    libptr = dlopen("../../../../../lib_Linux_FC3/libtk8.5.so", RTLD_LAZY);
#endif
    cerr << "Open Tk library - ";
    if (!libptr) {
        cerr << "FAIL" << endl;
        return -1;
    }
    cerr << "PASS" << endl;

    void* symbol = dlsym(libptr, "baloney");
    cerr << "Resolve non-existent symbol fails - ";
    if (symbol) {
        cerr << "FAIL" << endl;
        return -1;
    }
    cerr << "PASS" << endl;

    symbol = dlsym(libptr, "TkBindFree");
    cerr << "Resolved existing symbol - ";
    if (!symbol) {
        cerr << "FAIL" << endl;
        return -1;
    }
    cerr << "PASS" << endl;

    dlclose(libptr);
    return 0 ;
}

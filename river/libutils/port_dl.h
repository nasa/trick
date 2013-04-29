
#if defined(__APPLE__) || defined(WIN32)

#ifdef __APPLE__
#include <mach-o/dyld.h>
#define RTLD_LAZY 0
#else
#include <windows.h>
// These constants are unused on Windows but need to be defined for compilation
#define RTLD_LAZY 0
#define RTLD_NOW 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
#ifdef WIN32
void * dlopen(const char *filename, int mode) ;
void dlclose(void *handle) ;
void *dlsym(void *handle, const char *funcname) ;
char *dlerror(void) ;
#endif

#ifdef __cplusplus
}
#endif

#endif


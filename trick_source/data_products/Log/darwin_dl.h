
#ifdef __APPLE__

#include <mach-o/dyld.h>

#define RTLD_LAZY 0

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
void * dlopen(const char *filename, int mode) ;
void dlclose(void *handle) ;
void *dlsym(void *handle, char *funcname) ;
char *dlerror(void) ;

#ifdef __cplusplus
}
#endif

#endif


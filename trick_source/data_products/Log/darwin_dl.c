/*
 *  This file is a wrapper for the dlopen, dlclose, dlsym and dlerror
 *  routines for Darwin
 */

#ifdef __APPLE__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "darwin_dl.h"

char dyn_buf[1024];
char noerror[] = "noerror";
static char *dyn_error = NULL;

void *dlopen(const char *filename, int mode)
{
        NSObjectFileImage image;
        dyn_error = NULL;

        if (NSCreateObjectFileImageFromFile(filename, &image) !=
            NSObjectFileImageSuccess) {
                dyn_error = "Could not open file";
                return NULL;
        }
        return NSLinkModule(image, filename, NSLINKMODULE_OPTION_PRIVATE);
}

void dlclose(void *handle)
{
        dyn_error = NULL;
        NSUnLinkModule(handle, FALSE);
        return;
}

void *dlsym(void *handle, char *funcname)
{
        NSSymbol symbol;
        int cb = strlen(funcname) + 2;
        char *symname = (char *) alloca(cb);
        sprintf(symname, "_%s", funcname);

        dyn_error = NULL;

        symbol = NSLookupSymbolInModule(handle, symname);
        if (symbol) {
                return (void *) NSAddressOfSymbol(symbol);
        } else {
                sprintf(dyn_buf, "Symbol [%s] not found", symname);
                dyn_error = dyn_buf;
                return (void *) NULL;
        }
}

char *dlerror(void)
{
        return dyn_error;
}
#endif

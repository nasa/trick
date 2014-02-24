/*
 *  This file is a wrapper for the dlopen, dlclose, dlsym and dlerror
 *  routines for Windows
 */

#ifdef WIN32

#include <stdio.h>
#include <windows.h>
#include "port_dl.h"

static char noerror[] = "noerror";
static char *dyn_error = NULL;

void *dlopen(const char *filename, int mode)
{
        HINSTANCE hinstLib = LoadLibrary(TEXT(filename));
        dyn_error = NULL;
	    if (hinstLib == NULL) {
            dyn_error = "Could not open file";
            return NULL;
		}

        return (void*) hinstLib;
}

void dlclose(void *handle)
{
        dyn_error = NULL;
        FreeLibrary((HINSTANCE) handle);
        return;
}

void *dlsym(void *handle, const char *funcname)
{
	void* symbol = (void*)GetProcAddress((HINSTANCE)handle, funcname);
	if (symbol == NULL) {
		dyn_error = "ERROR: unable to find DLL function";
		return (void*) NULL;
	} else {
		dyn_error = NULL;
		return (void*) symbol;
	}
}

char *dlerror(void)
{
        return dyn_error;
}
#endif

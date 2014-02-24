#ifndef OPTS_H
#define OPTS_H

/* C header for Options.h C++ class */

#ifdef USE_DOUG_H
#include "doug.h"
#endif
#include "option_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque handle thing */
struct Opts;
typedef struct Opts Opts;

Opts* opts_create();
void opts_destroy(Opts* o);
int opts_parse(Opts* o, int argc, char **argv, const char *plugin_name);
#ifdef USE_DOUG_H
int  opts_tcl_cmd(Opts*o, void *data, Tcl_Interp *interp,
                   int objc, Tcl_Obj *objv[]);
#endif

#define OPTS_DECLARE_CBs(T) \
typedef void(*ptr_presetCB_##T)(T* valptr,const char* strval,bool* ok);\
typedef void(func_presetCB_##T)(T* valptr,const char* strval,bool* ok);\
typedef void(*ptr_postsetCB_##T)(T* valptr,T postsetval,bool* ok);\
typedef void(func_postsetCB_##T)(T* valptr,T postsetval,bool* ok);

// Function defs for Options.add(type)
#define FOREACH_C_TYPE(T) OPTS_DECLARE_CBs(T)
#include "option_types.h"

#define OPTS_DECLARE_ADD(A,T,V) \
void opts_add_##A(Opts*o, T* var, const char* opt, V val,\
                   const char* info,\
                   ptr_presetCB_##T  presetCB,\
                   ptr_postsetCB_##T postsetCB)

OPTS_DECLARE_ADD(int,int,int);
OPTS_DECLARE_ADD(uint,uint,int);
OPTS_DECLARE_ADD(double,double,double);
#ifdef USE_DOUG_H
OPTS_DECLARE_ADD(node,CDougNode,const char*);
#endif
OPTS_DECLARE_ADD(host,CDougHost,const char*);
OPTS_DECLARE_ADD(bool,CDougBool,int);
OPTS_DECLARE_ADD(string,CDougString,const char*);

void opts_add_color(Opts*o, CDougColor* var, const char* opt,
                     int red, int green, int blue,
                    const char *info,
                     ptr_presetCB_CDougColor  presetCB,
                     ptr_postsetCB_CDougColor postsetCB);

#ifdef __cplusplus
}
#endif

#endif // OPTS_H

#include "options.h"
#include "opts.h"

#include <stdio.h>
#include <stdlib.h>

/* C functions for Options C++ class */

struct Opts : public Options {};

extern "C" {

#define IF_NULL_RETURN(X)\
    if ( X == NULL || X == 0 ) {\
        return;\
    }

#define OPTS_ADD(A,T,V)\
OPTS_DECLARE_ADD(A,T,V)\
{\
    if ( o == NULL || o == 0 ) {\
        return;\
    } \
\
    try {\
        o->add(var,opt,T(val),info,presetCB,postsetCB);\
    } catch (...) {}\
}

Opts* opts_create()
{
    Opts* o = NULL;
    try {
        o = new Opts();
    } catch (...) {}
    return o;
}

void opts_destroy(Opts* o)
{
    IF_NULL_RETURN(o);
    delete o;
}

int opts_parse(Opts* o, int argc, char **argv, const char *plugin_name)
{
    if ( o == NULL || o == 0 ) {
        return -1;
    }

    bool ok;
    try {
        o->parse(argc,argv,plugin_name,&ok);
    } catch (...) {}

    if ( ! ok ) {
        return -1;
    } else {
        return 0;
    }
}

#ifdef USE_DOUG_H
int opts_tcl_cmd(Opts*o, void *data, Tcl_Interp *interp,
                   int objc, Tcl_Obj *objv[])
{
    if ( o == NULL || o == 0 ) return TCL_ERROR;

    int ret = TCL_OK;
    try {
        ret = o->tcl_cmd(data,interp,objc,objv);
    } catch (...) {}
    return ret;
}
#endif

OPTS_ADD(int,int,int)
OPTS_ADD(double,double,double)
#ifdef USE_DOUG_H
OPTS_ADD(node,CDougNode,const char*)
#endif
OPTS_ADD(host,CDougHost,const char*)

// The "int val" is on purpose because C will not croak on compiling
// opts_add_uint(o,var,"-size",-4,NULL,NULL)
// However, if somebody uses a giant uint... this will die
// Dunno...
void opts_add_uint(Opts*o, uint* var, const char* opt, int val,
                   const char* info,
                    ptr_presetCB_uint  presetCB,
                    ptr_postsetCB_uint postsetCB)
{
    if ( val < 0 ) {
        // Hard exit on this!
        fprintf(stderr, "\n\n");
        fprintf(stderr, "opts.cpp [error]: opts_add_uint() called with "
                "a default value of %d.  The value should be greater than 0 "
                "since it is unsigned.  Now it could be that val is "
                "large and I'm shooting you in the foot :(\n", val);
        fprintf(stderr, "\n\n");
        exit(-1);
    }

    IF_NULL_RETURN(o);
    try {
        o->add(var,opt,(uint)val,info,presetCB,postsetCB);
    } catch (...) {}
}

void opts_add_color(Opts*o, CDougColor* var, const char* opt,
                    int red, int green, int blue,
                    const char* info,
                    ptr_presetCB_CDougColor  presetCB,
                    ptr_postsetCB_CDougColor postsetCB)
{
    IF_NULL_RETURN(o);
    try {
        o->add(var,opt,CDougColor(red,green,blue),info,presetCB,postsetCB);
    } catch (...) {}
}

void opts_add_bool(Opts*o, CDougBool* var, const char* opt, int val,
                    const char* info,
                    ptr_presetCB_CDougBool  presetCB,
                    ptr_postsetCB_CDougBool postsetCB)
{
    if ( val != 0 && val != 1 ) {
        // Hard exit on this!
        fprintf(stderr, "\n\n");
        fprintf(stderr, "opts.cpp [error]: opts_add_bool() called with "
                "a default value of %d.  The value should be either 0 or 1 "
                "since it is a boolean.\n", val);
        fprintf(stderr, "\n\n");
        exit(-1);
    }

    IF_NULL_RETURN(o);
    try {
        bool b = val ? 1 : 0 ;
        o->add(var,opt,CDougBool(b),info,presetCB,postsetCB);
    } catch (...) {}
}

} // end extern "C"

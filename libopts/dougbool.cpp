#include "dougbool.h"

extern "C" {

CDougBool* dougbool_create()
{
    CDougBool* c = NULL;
    try {
        bool b = false;
        c = new CDougBool(b);
    } catch (...) {}
    return c;
}

void dougbool_destroy(CDougBool* c)
{
    if ( c == 0 || c == NULL ) return;

    delete c;

    c = 0 ;
}

}

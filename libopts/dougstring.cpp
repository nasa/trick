#include "dougstring.h"


#include <stdio.h>

DougString::DougString() : _str()
{
}

DougString::DougString(const string &str) :
    _str(str)
{
}

DougString::DougString(const char *str) :
    _str(str)
{
}

void DougString::set(const string &str)
{
    _str = str;
}

string DougString::get() const
{
    return _str;
}

bool DougString::isNull() const
{
    bool ret = false;
    if ( _str.size() == 0 ) {
        ret = true;
    }
    return ret;
}


//
// C interface for DougString Class
//

extern "C" {

CDougString* dougstring_create()
{
    CDougString* c = NULL;
    try {
        c = new CDougString();
    } catch (...) {}
    return c;
}

void dougstring_destroy(CDougString* c)
{
    if ( c == 0 || c == NULL ) return;

    delete c;
    c = 0 ;
}

}


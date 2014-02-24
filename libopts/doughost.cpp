#include "doughost.h"

#include <stdio.h>

DougHost::DougHost() :
    _address()
{
}

DougHost::DougHost(const string &address) :
    _address(address)
{
}

void DougHost::setAddress(const string &address)
{
    _address = address;
}

string DougHost::toString() const
{
    return _address;
}

bool DougHost::isNull() const
{
    bool ret = false;
    if ( _address.length() == 0 ) {
        ret = true;
    }
    return ret;
}


//
// C Interface for DougHost class
//
extern "C" {

CDougHost* doughost_create()
{
    CDougHost* c = NULL;
    try {
        c = new CDougHost();
    } catch (...) {}
    return c;
}

void doughost_destroy(CDougHost* c)
{
    if ( c == 0 || c == NULL ) return;

    delete c;
    c = 0 ;
}

}

#ifndef DOUGHOST_H
#define DOUGHOST_H

#ifdef __cplusplus
//
// For now this is just a fake facade illusion of QHostAddress
//
// TODO: probably should enforce name syntax, see:
//     http://tools.ietf.org/html/rfc1123#page-13
//     http://tools.ietf.org/html/rfc952 (rfc1123 allows 1st char to be digit)

#include <string>
using namespace std;

class DougHost
{
public:
    DougHost();
    DougHost(const string& address);
    void setAddress(const string &address);
    string toString() const;
    bool isNull() const;

private:
    string _address;
};
#endif

/*
 * C interface for DougHost class
 */

#ifdef __cplusplus
#include "doughost.h"
struct CDougHost : public DougHost {
     CDougHost() : DougHost() {}
     CDougHost(const string& host_name) : DougHost(host_name) {}
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque handle thing */
struct CDougHost;
typedef struct CDougHost CDougHost;

CDougHost* doughost_create();
void doughost_destroy(CDougHost* c);

#ifdef __cplusplus
}
#endif
#endif // DOUGHOST_H

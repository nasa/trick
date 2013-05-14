#ifndef DOUGSTRING_H
#define DOUGSTRING_H

#ifdef __cplusplus
//
// For now this is just a fake facade illusion of QHostAddress
//
// TODO: probably should enforce name syntax, see:
//     http://tools.ietf.org/html/rfc1123#page-13
//     http://tools.ietf.org/html/rfc952 (rfc1123 allows 1st char to be digit)

#include <string>
using namespace std;

class DougString
{
public:
    DougString();
    DougString(const string& str);
    DougString(const char* str);
    void set(const string &str);
    string get() const;
    bool isNull() const;

private:
    string _str;
};
#endif


/* C interface for DougString class */

#ifdef __cplusplus
#include "dougstring.h"
struct CDougString : public DougString {
     CDougString() : DougString() {}
     CDougString(const string& host_name) : DougString(host_name) {}
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque handle thing */
struct CDougString;
typedef struct CDougString CDougString;

CDougString* dougstring_create();
void dougstring_destroy(CDougString* c);

#ifdef __cplusplus
}
#endif

#endif // DOUGSTRING_H

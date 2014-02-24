#ifndef DOUGBOOL_H
#define DOUGBOOL_H

#ifdef __cplusplus

#include <string>
using namespace std;

class DougBool
{
  public:
    DougBool(bool b) : _b(b) {}
    bool get() const { return _b ; }
    void set(bool b) { _b = b; }

    void set(string s, bool* ok)
    {
        *ok = true;
        if ( s == string("0") || s == string("false") ) {
            _b = false;
        } else if ( s == string("1") || s == string("true") ) {
            _b = true;
        } else {
            *ok = false;
        }
    }

    void set(int i, bool* ok)
    {
        *ok = true;
        if ( i == 0 ) {
            _b = false;
        } else if ( i == 1 ) {
            _b = true;
        } else {
            *ok = false;
        }
    }

    string toString()
    {
        return _b ? string("true") : string("false");
    }

  private:
    DougBool() : _b(false) {}
    bool _b;
};

#endif

/*
 * C Interface for DougBool Class
 */

#ifdef __cplusplus
#include <string>
using namespace std;
struct CDougBool : public DougBool {
    CDougBool(bool b) : DougBool(b) {}
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque handle thing */
struct CDougBool;
typedef struct CDougBool CDougBool;

CDougBool* dougbool_create();
void dougbool_destroy(CDougBool* c);

#ifdef __cplusplus
}
#endif

#endif // DOUGBOOL_H

#ifndef DOUGCOLOR_H
#define DOUGCOLOR_H

#ifdef __cplusplus

#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
using namespace std;

class DougColor
{
public:
    DougColor();
    DougColor(int red, int green, int blue, int a = 255);

    inline int red()   const { return _r ; }
    inline int green() const { return _g ; }
    inline int blue()  const { return _b ; }
    inline int alpha() const { return _a ; }

    vector<int> rgb() const;
    vector<int> rgba() const;
    void set_rgb(int red, int green, int blue);
    void set_rgba(int red, int green, int blue, int a);

    string rgb_hexname() const;
    string rgba_hexname() const;

private:
    int _r; int _g; int _b; int _a;
    void _clamp(int r, int g, int b, int a);
};

#endif

/* C Interface for DougColor  class */

#ifdef __cplusplus
struct CDougColor : public DougColor {
     CDougColor() : DougColor() {}
     CDougColor(int red, int green, int blue) :
         DougColor(red,green,blue,255) {}
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque handle thing */
struct CDougColor;
typedef struct CDougColor CDougColor;

CDougColor* dougcolor_create();
void dougcolor_destroy(CDougColor* c);
int dougcolor_red(CDougColor* c);
int dougcolor_green(CDougColor* c);
int dougcolor_blue(CDougColor* c);
int dougcolor_alpha(CDougColor* c);

#ifdef __cplusplus
}
#endif

#endif // DOUGCOLOR_H

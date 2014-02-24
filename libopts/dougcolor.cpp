#include "dougcolor.h"

DougColor::DougColor()
{
}

DougColor::DougColor(int red, int green, int blue, int a)
{
    _r = red;
    _g = green;
    _b = blue;
    _a = a;
    _clamp(_r,_g,_b,_a);
}

string DougColor::rgb_hexname() const
{
    std::stringstream stream;
    stream << "#" << std::hex << _r << std::hex << _g << std::hex << _b;
    string hex( stream.str() );
    return hex;
}

string DougColor::rgba_hexname() const
{
    std::stringstream stream;
    stream << rgb_hexname() << std::hex << _a;
    string hex( stream.str() );
    return hex;
}

void DougColor::_clamp(int r, int g, int b, int a)
{
    if (r < 0) _r = 0 ;
    if (g < 0) _g = 0 ;
    if (b < 0) _b = 0 ;
    if (a < 0) _a = 0 ;

    if (r > 255) _r = 255 ;
    if (g > 255) _g = 255 ;
    if (b > 255) _b = 255 ;
    if (a > 255) _a = 255 ;
}

vector<int> DougColor::rgb() const
{
    vector<int> v;
    v.push_back(_r);
    v.push_back(_g);
    v.push_back(_b);
    return v;
}

vector<int> DougColor::rgba() const
{
    vector<int> v;
    v.push_back(_r);
    v.push_back(_g);
    v.push_back(_b);
    v.push_back(_a);
    return v;
}

void DougColor::set_rgb(int red, int green, int blue)
{
    _r = red;
    _g = green;
    _b = blue;
    _clamp(_r,_g,_b,_a);
}

void DougColor::set_rgba(int red, int green, int blue, int a)
{
    _r = red;
    _g = green;
    _b = blue;
    _a = a;
    _clamp(_r,_g,_b,_a);
}

//
// C Interface For DougColor Class
//
extern "C" {

CDougColor* dougcolor_create()
{
    CDougColor* c = NULL;
    try {
        c = new CDougColor();
    } catch (...) {}
    return c;
}

void dougcolor_destroy(CDougColor* c)
{
    if ( c == 0 || c == NULL ) return;

    delete c;
    c = 0 ;
}

int dougcolor_red(CDougColor* c)   { return c ? c->red()   : 0; }
int dougcolor_green(CDougColor* c) { return c ? c->green() : 0; }
int dougcolor_blue(CDougColor* c)  { return c ? c->blue()  : 0; }
int dougcolor_alpha(CDougColor* c) { return c ? c->alpha() : 0; }

}

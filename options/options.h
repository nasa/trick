#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include "option_types.h"

#include <iterator>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <iomanip>
#include <sstream>
using namespace std;

#define OPTIONS_GET(T) \
    T* valptr = _##T##s[opt];\
    strval = \
           T##_to_str(valptr,ok);

#define OPTIONS_SET(T) \
    T* valptr = _##T##s[opt];\
    if ( _presetCB_##T##s[opt] != NULL ) { \
        ptr_presetCB_##T func = _presetCB_##T##s[opt]; \
        func(valptr,val.c_str(),(int*)ok); \
    } \
    T presetval = *valptr;\
\
    if ( *ok ) {\
        str_to_##T(val,valptr,ok);\
    }\
\
    if ( *ok && _postsetCB_##T##s[opt] != NULL ) { \
        ptr_postsetCB_##T func = _postsetCB_##T##s[opt]; \
        func(valptr,presetval,(int*)ok); \
    }

#define OPTIONS_ADD(T) \
void Options::add(T* var, const char* opt, const T& val, \
                  const string& info,\
                  ptr_presetCB_##T presetCB, \
                  ptr_postsetCB_##T postsetCB)\
{\
    bool ok = true;\
    _opts.insert(opt); \
    _##T##s.insert(make_pair(opt,var)); \
    string strval = \
                  T##_to_str(&val,&ok);\
    _ivals.insert(make_pair(opt, strval)); \
    _types.insert(make_pair(opt,#T)); \
    _info.insert(make_pair(opt, info)); \
    _presetCB_##T##s.insert(make_pair(opt,presetCB)); \
    _postsetCB_##T##s.insert(make_pair(opt,postsetCB)); \
    *var = val;\
}

#define OPTIONS_DECLARE_TYPE(T) \
public:\
typedef void(*ptr_presetCB_##T)(T* valptr,const char* strval,int* ok);\
typedef void(func_presetCB_##T)(T* valptr,const char* strval,int* ok);\
typedef void(*ptr_postsetCB_##T)(T* valptr,T postsetval,int* ok);\
typedef void(func_postsetCB_##T)(T* valptr,T postsetval,int* ok);\
void add(T* var, const char* opt, const T& val, \
         const string& info="",\
         ptr_presetCB_##T  presetCB=NULL,\
         ptr_postsetCB_##T postsetCB=NULL);\
    static void str_to_##T(const string& strval, T* valptr, bool*ok);\
    static string \
            T##_to_str(const T* const valptr, bool*ok);\
private:\
    map<string,T*> _##T##s;\
    map<string,ptr_presetCB_##T> _presetCB_##T##s;\
    map<string,ptr_postsetCB_##T> _postsetCB_##T##s;


class Options
{
  public:
    Options();
    ~Options();

    void parse(int argc, char **argv, const char *plugin_name, bool *ok);
#ifdef USE_DOUG_H
    int tcl_cmd(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *objv[]);
#endif
    string usage() ;

    // The following macro hack defines public add() member per type e.g.:
    //  void add(int* var, const char* opt, int val,
    //           ptr_presetCB_int  presetCB=NULL,
    //           ptr_postsetCB_int postsetCB=NULL);
    // It also defines private members that go with each type
    #define FOREACH_TYPE(T) OPTIONS_DECLARE_TYPE(T)
    #include "option_types.h"

    static bool is_hexadecimal(const string &sval);

private:
    string _plugin_name;
    set<string> _opts;
    map<string,string> _ivals;  // option->string_value (init val add()ed)
    map<string,string> _types;  // option->var_type (e.g -width->int)
    map<string,string> _info;   // option's info (e.g. "width of screen")

    string _get(const string& opt, bool *ok);
    void _set(const string& opt, const string& val, bool* ok);
    string _options();
} ;
#endif 

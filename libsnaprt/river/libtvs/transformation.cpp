#include "transformation.hh"
#include "../libutils/port_dl.h"
#include "../libutils/exception.hh"
#include <iostream>
#include <sstream>
#include <cstdlib>
#ifndef WIN32
#include <dlfcn.h>
#endif
using std::stringstream;
using std::cerr;
using std::endl;

// Transformation using a function name. Names may be loaded from a .so
// by specifying a libname, or may be global functions. In the latter
// case, the code containing the global function should be compiled with
// -rdynamic to ensure that the linker exports the symbol.
Transformation::Transformation(const string& funcname, const string& libname):
                       _funcname(funcname), _progHandle(0)
{
    _openTransformation(libname);
}

void Transformation::_openTransformation(const string& libname)
{
    char* error;

    if (libname == "") {
        // Try to find function internally
#ifdef RTLD_DEFAULT
        _transform = (Transform) dlsym(RTLD_DEFAULT, _funcname.c_str());
#else
        _transform = (Transform) dlsym(NULL, _funcname.c_str());
#endif
    } else {
        // Open up external shared object
        _progHandle = dlopen(libname.c_str(), RTLD_LAZY);
        if (!_progHandle) {
            setError( string("Couldn't load shared object: ") + libname );
            return;
        }
        _transform = (Transform) dlsym(_progHandle, _funcname.c_str());
    }

    if ((error = dlerror()) != NULL) {
        setError( string("Unable to find function ") + _funcname
                  + " from " + libname + ": " + error );
        return;
    } else {
        // Success
    }
}

void Transformation::close()
{
    if (_progHandle) dlclose(_progHandle);
}

Transformation::Transformation(Transform usertransform): 
                      _transform(NULL),
                      _progHandle(NULL)
{
    _transform = usertransform;
}

void Transformation::addInparam(const TrickParam& param, const string alias)
{
    _inputs.addParam(param, alias);
}

void Transformation::addOutparam(const TrickParam& param, const string alias)
{
    _outputs.addParam(param, alias);
}

TrickParam& Transformation::getInparam(const string& name)
{
    return _inputs.getParam(name);
}

TrickParam& Transformation::getOutparam(const string& name)
{
    return _outputs.getParam(name);
}

// Index represents where to read values from the values array
// and where to place output values
void Transformation::callTransform(const int index)
{
    // Set up input values
    _inputs.setCurrent(index);
    try {
        // Transform, and copy outputs to the index position
        _transform(_inputs, _outputs);
        _outputs.setFromCurrent(index);
    } catch ( std::exception& s ) {
       // Transform threw exception
       cerr << "Exception thrown by transform " << _funcname << ": " 
            << s.what() << endl;
       throw Exception("Exception thrown by transform " + _funcname + ": " 
                + s.what());
    } catch (...) {
       cerr << "Unknown exception thrown in transform " << _funcname << endl;
    }
}

void Values::addParam(const TrickParam& param, const string& alias)
{
    _names.push_back(param.name);
    _values[param.name] = param;
    if (alias != "") _aliases[alias] = param.name;
}

TrickParam& Values::getParam(const string& name)
{
    if (validName(name)) {
        return _values[_resolveName(name)];
    } else {
        throw Exception("Can't find parameter " + name); 
    }
}

string Values::_resolveName(const string& incoming_name)
{
    string realName;

    // Check for alias
    realName = _aliases[incoming_name];         
    if (realName != "") return realName;

    // Check for normal name
    if (_values.count(incoming_name)) return incoming_name;

    return "";
}

double& Values::operator[](const string& name)
{
    if (validName(name)) {
        return _values[_resolveName(name)].incoming_val;
    } else {
        throw Exception("Unknown variable name: " + name);
    }
}

void Values::setUnits(const string& name, const string& units)
{
    if (validName(name)) _values[_resolveName(name)].units = units;
}

string Values::getUnits(const string& name)
{
    string ret;
    if (validName(name)) ret = _values[_resolveName(name)].units;
    return ret;
}

void Values::setCurrent(const int index)
{
    for (unsigned int ii = 0; ii < _names.size(); ii++) {
        string& name = _names[ii];
        if (_values[name].type != TrickParam::CONSTANT &&
            _values[name].type != TrickParam::ONESHOT ) {
            _values[name].incoming_val = _values[name].vals[index];
        }
    }
}

void Values::setFromCurrent(const int index)
{
    for (unsigned int ii = 0; ii < _names.size(); ii++) {
        string& name = _names[ii];
        if (_values[name].type != TrickParam::CONSTANT &&
            _values[name].type != TrickParam::ONESHOT ) {
            _values[name].vals[index] = _values[name].incoming_val; 
        }
    }
}

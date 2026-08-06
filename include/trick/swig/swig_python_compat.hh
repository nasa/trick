/**
 * Trick's SWIG interface code uses a handful of Python C API calls the Python 2 way.
 * Through SWIG 4.4 those names were supplied for Python 3 builds by compatibility macros in SWIG.
 * SWIG 4.5.0 removed them (swig commit 79f7a2b), so Trick defines them here instead.
 *
 * Only the macros Trick uses are defined.
 */

#ifndef SWIG_PYTHON_COMPAT_HH
#define SWIG_PYTHON_COMPAT_HH

#include <Python.h>

/* Under Python 2 these are the real C API names and must not be redefined. */
#if PY_VERSION_HEX >= 0x03000000

#ifndef PyInt_Check
#define PyInt_Check(x) PyLong_Check(x)
#endif

#ifndef PyInt_AsLong
#define PyInt_AsLong(x) PyLong_AsLong(x)
#endif

#ifndef PyInt_FromLong
#define PyInt_FromLong(x) PyLong_FromLong(x)
#endif

#ifndef PyString_FromString
#define PyString_FromString(x) PyUnicode_FromString(x)
#endif

#endif

#endif

//@trick_link_dependency{Classes.cpp}

#ifndef CLASSES_HH
#define CLASSES_HH

#include "trick/SimObject.hh"

void foo();

class Normal {};
template <class T> class Template1 {};
template <class T, typename U> struct Template2 {};

namespace Foo {

class FooNormal {};
template <class T> class FooTemplate1 {};
template <class T, typename U> struct FooTemplate2 {};

};

#endif

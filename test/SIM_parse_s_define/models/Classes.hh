//@trick_link_dependency{Classes.cpp}

#ifndef CLASSES_HH
#define CLASSES_HH

#ifdef UNDEFINED
#error ERROR MESSAGE
#endif
#ifdef UNDEFINED
#warning WARNING MESSAGE
#endif
//Weird comments are to verify comment removal doesn't break code
#include "trick///SimObject.hh" //some comment
#include <.//string> /*
a
b
c
d"
*/

//Line continuation test \
More comment /* \
a\
abcd \
efg
void foo();

class Normal {};
template <class T> class Template1 {};
template <class T, typename U> struct Template2 {};

namespace Foo {

class FooNormal {};
template <class T> class FooTemplate1 {};
template <class T, typename U> struct FooTemplate2 {};
};

class ParseTest {
    std::string str;
    
    public:
    ParseTest(std::string s) {
        str = s;
    }
    
    ParseTest() {
        str = "COMMENT PARSE //TEST"; // \
test comment
        str = "COMMENT PARSE /*TEST";
        str = "COMMENT PARSE */TEST";
        str = 
"COMMENT PARSE TEST";
    }
};


#endif

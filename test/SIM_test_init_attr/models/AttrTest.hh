/**
PURPOSE: (stuff)
LIBRARY_DEPENDENCIES: ((AttrTest.cpp))
*/
#pragma once

struct Inner {
    int i;
};

struct AnotherInner {
    int j;
};

struct Outer {
    Inner inner;
    AnotherInner another_inner;
};

class AttrTest {
  public:
   void initialization();
   void scheduled();
};



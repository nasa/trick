
#ifndef OVERLOADEDVARIABLE_HH
#define OVERLOADEDVARIABLE_HH

/*
   This inheritance heirarchy tests the case where a variable name is overloaded
   in inherited classes.  Child1 and Child2 test the extreme cases where we
   have diamond inheritance with overloading.
 */

#ifndef __APPLE__
#ifndef SWIG
// (Alex 2/3/14) I hate to do this, but I cannot generate the io_code I want to for
// these classes. The clang compiler does not take class::variable as an argument to
// the offsetof macro.  I could not figure out a proper offsetof statement for "i"
// in Child1 and Child2.  So rather than trying to fix ICG, I'm commenting these
// classes out.

class MomMom {
    public:
    int i;
};

class DadDad {
    public:
    int i;
};

class Mom : public MomMom {
    public:
    int i;
    int j;
};

class Dad : public MomMom {
    public:
    int i;
    int j;
};

class Child1 : public Mom, public Dad {
    public:
        Child1() {
            Mom::i = 1 ;
            Dad::i = 3 ;
        }
};

class Child2 : public Mom, public Dad {
    public:
        int i;
        Child2() {
            Mom::i = 2 ;
            Dad::i = 4 ;
            i = 6 ;
        }
};

#endif
#endif


#endif

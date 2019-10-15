#ifndef EXCLUDE6
#define EXCLUDE6

/**
 * This file is meant to be excluded from ICG. It will produce ICG-generated
 * code that does not compile (as long as
 * https://github.com/nasa/trick/issues/422 isn't fixed).
 */
class Exclude6A {
    private:
    void operator=(const Exclude6A&);
};

class Exclude6B {
    const std::vector<Exclude6A> foos;
};

#ifdef SWIG
This file is meant to be excluded from SWIG. It will cause an error if SWIG tries to process it.
#endif

#endif

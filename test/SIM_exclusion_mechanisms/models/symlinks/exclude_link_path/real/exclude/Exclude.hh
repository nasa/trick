#ifndef EXCLUDE5
#define EXCLUDE5

/**
 * This file is meant to be excluded from ICG. It will produce ICG-generated
 * code that does not compile (as long as
 * https://github.com/nasa/trick/issues/422 isn't fixed).
 */
class Exclude5A {
    private:
    void operator=(const Exclude5A&);
};

class Exclude5B {
    const std::vector<Exclude5A> foos;
};

#ifdef SWIG
This file is meant to be excluded from SWIG. It will cause an error if SWIG tries to process it.
#endif

#endif

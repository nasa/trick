#ifndef ICG_EXCLUDE1
#define ICG_EXCLUDE1

/**
 * This file is meant to be excluded from ICG. It will produce ICG-generated
 * code that does not compile (as long as
 * https://github.com/nasa/trick/issues/422 isn't fixed).
 */
class IcgExclude1A {
    private:
    void operator=(const IcgExclude1A&);
};

class IcgExclude1B {
    const std::vector<IcgExclude1A> foos;
};

#endif

#ifndef ICG_EXCLUDE4
#define ICG_EXCLUDE4

/**
 * This file is meant to be excluded from ICG. It will produce ICG-generated
 * code that does not compile (as long as
 * https://github.com/nasa/trick/issues/422 isn't fixed).
 */
class IcgExclude4A {
    private:
    void operator=(const IcgExclude4A&);
};

class IcgExclude4B {
    const std::vector<IcgExclude4A> foos;
};

#endif

#ifndef ICG_EXCLUDE3
#define ICG_EXCLUDE3

/**
 * This file is meant to be excluded from ICG. It will produce ICG-generated
 * code that does not compile (as long as
 * https://github.com/nasa/trick/issues/422 isn't fixed).
 */
class IcgExclude3A {
    private:
    void operator=(const IcgExclude3A&);
};

class IcgExclude3B {
    const std::vector<IcgExclude3A> foos;
};

#endif

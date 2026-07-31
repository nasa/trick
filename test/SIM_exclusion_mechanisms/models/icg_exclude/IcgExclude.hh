#ifndef ICG_EXCLUDE2
#define ICG_EXCLUDE2

/**
 * This file is meant to be excluded from ICG. It will produce ICG-generated
 * code that does not compile (as long as
 * https://github.com/nasa/trick/issues/422 isn't fixed).
 */
//*
    // "//*" would fail ICG, but since this file is on the TRICK_ICG_EXCLUDE list, it should be fully excluded from ICG and not cause a problem.
//*
class IcgExclude2A {
    private:
    void operator=(const IcgExclude2A&);
};

class IcgExclude2B {
    const std::vector<IcgExclude2A> foos;
};

#endif

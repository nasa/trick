// @trick_parse{everything}
// @trick_link_dependency{ExtLibDir.cpp}

#ifndef EXT_LIB_DIR3
#define EXT_LIB_DIR3

/**
 * This file is part of a Trickified library and is meant to be excluded from
 * ICG via TRICK_EXT_LIB_DIRS. If the exclusion logic failts, the generated I/O
 * code will result in multiple-definition errors at link time as the symbols
 * will also exist in the Trickified partially-linked object.
 */
class ExtLibDir3 {
    private:
    void operator=(const ExtLibDir3&);
};


#endif

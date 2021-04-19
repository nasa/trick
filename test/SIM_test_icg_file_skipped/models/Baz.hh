// @trick_parse{everything}
#include "Foo.hh"
#include "trick/memorymanager_c_intf.h"

class Baz {
    public:
    Foo foo;
    int m;

    Baz(): m(42) {}

    int test_baz_member_m_correct_offset() {
        char ref_name[] = "testfsSimObject.baz.m";
        REF2* ref = ref_attributes(ref_name);
        if(*((int*)ref->address) == m) {
            return 0;
        }
        else {
            return 1;
        }
    }
};

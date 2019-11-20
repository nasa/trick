
Trick provides checkpointable versions of std::STLs.

The list of Trick::STLs are

- deque
- list
- map
- multimap
- multiset
- pair
- priority_queue
- queue
- set
- stack
- vector

The Trick STLs require a C++11 compliant compiler.  As of this writing
gcc 4.4+ on Linux systems meets these requirements using the "-std=c++0x"
command line argument.

To use Trick STLs include the proper header file from sim_services/STL/include
and use Trick::STL_name instead of std::STL_name.  Trick STLs must be public,
or the proper init_attr friends must be present for checkpointing to work.  A
Trick comment with "**" has not effect, Trick STLs will always be checkpointed.
If checkpointing is not desired use the std::STL_name instead.

Example:

```
#include "sim_services/STL/include/trick_vector.hh"

class A {
    public:
        Trick::vector< int > my_vector ; // -- trick_comment
}
```

[Continue to Threads](Threads)

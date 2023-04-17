| [Home](/trick) → [Developer Docs](Developer-Docs-Home) → Tooling and Sanitizers |
|------------------------------------------------------------------|

Lots of development and debugging tools require a binary to be instrumented with compiler flags. Trick does compiling and linking steps separately and uses several variables to propogate flags to different parts of the build. The following is a convenience function that can be added to your bashrc to easily modify the flags in your environment:

```
add-trickenv () {
    export CFLAGS="$CFLAGS $1"
    export CXXFLAGS="$CXXFLAGS $1"
    export LDFLAGS="$LDFLAGS $1"
    export TRICK_CFLAGS="$TRICK_CFLAGS $1"
    export TRICK_CXXFLAGS="$TRICK_CXXFLAGS $1"
    export TRICK_LDFLAGS="$TRICK_LDFLAGS $1"
    export TRICK_SYSTEM_CFLAGS="$TRICK_SYSTEM_CFLAGS $1"
    export TRICK_SYSTEM_CXXFLAGS="$TRICK_SYSTEM_CXXFLAGS $1"
    export TRICK_SYSTEM_LDFLAGS="$TRICK_SYSTEM_LDFLAGS $1"
}
```

To debug a sim, you will likely need to run a clean build of all of Trick with these flags set. 

## Tools that are known to work well with Trick

GDB/LLDB: `-g`

gcov: `-fprofile-arcs -ftest-coverage -O0`

tsan: `-g -fsanitize=thread`

asan: `-g -fsanitize=address -fsanitize-recover=address`

Suggest running asan instrumented sims with:

`ASAN_OPTIONS=halt_on_error=0 ./S_main* <your args>`


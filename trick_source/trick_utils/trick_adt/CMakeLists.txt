# Abstract data types: lists, queues, stacks, trees, and maps. Compiled into libtrick.a.
trick_add_library(trick_adt OBJECT
    SOURCES
        src/bst.c
        src/bubble_sort.c
        src/dllist.c
        src/lqueue.c
        src/lstack.c
        src/MapStrToPtr.c
        src/record_array.c
)

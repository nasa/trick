# Shared memory (tsm_*). Compiled into libtrick.a.
trick_add_library(trick_shm OBJECT
    SOURCES
        src/tsm_disconnect.c
        src/tsm_init.c
        src/tsm_init_with_lock.c
        src/tsm_reconnect.c
)

target_link_libraries(trick_shm PRIVATE Threads::Threads)

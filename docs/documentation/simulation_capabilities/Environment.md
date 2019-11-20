
This class saves the Trick environment variables at compile-time.  The variables
are saved inside the S_main executable.  These variables are read-only after they
are compiled into the sim.

```c++
const char * get_trick_env( char * variable_name )
```

[Continue to STL Checkpointing](STL-Checkpointing)

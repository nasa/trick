List of features that have been deprecated.

### Trick Specific Units to Standard Units

Deprecated: 19
To be Removed: 21

This feature automatically converted Trick specific units to the standard unit names, e.g. "M" to "m" for meters.  Trick 17 allowed the user to suppress warning messages Trick was doing this conversion.  Trick 19 removes the option to suppress these warning messages.  Trick 21 will remove the capability.

### TRICK_CPPC environment variable to specify c++ Compiler

Deprecated: 19
To be Removed: 21

Moving this environment variable to conform to the rest of the world.  In 19 if TRICK_CPPC is found in the environment a warning is emitted and the variable is used.  In 21, the variable will no longer be recognized.
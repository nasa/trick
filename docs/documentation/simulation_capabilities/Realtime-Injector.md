
The Real Time Injector (RTI) allows the user to set simulation variables
synchronously without impacting real-time performance.  The RTI performs the injection
as a top of frame job ensuring thread-safe variable operations.

### Adding and Injecting Variables

```
trick.rti_add( char * var_name , long long value)
trick.rti_add( char * var_name , double value)
trick.rti_fire( unsigned int thread_id)
```

The rti_add call is typically executed in the input file or variable server,
rti_add requires a variable name and a value (the value may also be a variable name, expression, etc.).
The rti_add call invokes an overloaded function that currently allows either a 64bit integer or double
precision float value to be inputted as the value. The RTI will down cast the
value to the variable type pointed to by var_name.  If the value is a variable name, the value of the
variable is determined at the time of the rti_add call, NOT when the injection (rti_fire call) is done.

Multiple variables are allowed to be injected at the same time with separate
rti_add statements. Trick keeps an internal queue of all pending variable injections.

The injection occurs at the top of the next real-time frame only after the rti_fire() call is commanded.
All variable(s) added with rti_add are set according to the values in the rti_add statement(s).
The queue of pending injections is then cleared.

### Adjusting Firing Frame

A frame multiple and offset may be set to only allow injections to occur on a frame multiple boundary
and offset.

```python
trick.rti_set_frame_multiple( unsigned int thread_id , unsigned int mult )
trick.rti_set_frame_offset( unsigned int thread_id , unsigned int offset )
```

### Listing Scheduled Injections

```
trick.rti_list()
```

rti_list prints the list of pending injections to the screen.
Listing the injections is not a real-time friendly operation.

### Debugging Injections

```
trick.rti_set_debug( True | False )
```

Debug statements are printed if the rti debug flag is true.  But we don't need them
because there aren't any bugs. :D  Debug statements are normally set to Off.

[Continue to Monte Carlo](UserGuide-Monte-Carlo)

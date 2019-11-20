
The Trick::CommandLineArguments class stores the command line arguments specified
by the user when starting the simulation.  The class provides routines to
access the arguments.

### Accessing the Command Line Arguments

The command line arguments are available to user classes through the following two calls.

```
int command_line_args_get_argc() ;
char ** command_line_args_get_argv() ;
```

Specific directories and file names may be accessed by the following calls.

```
const char * command_line_args_get_output_dir() ;
const char * command_line_args_get_input_file() ;
const char * command_line_args_get_default_dir() ;
const char * command_line_args_get_cmdline_name() ;
```

### Modifying the Output Directory

By default the RUN directory of the input file is the output directory.  To change this
specify the <tt>-O</tt> or the <tt>-OO</tt> command line arguments.

```
S_main_${TRICK_HOST_CPU}.exe RUN_dir/input.py -O <output_dir>
```

All Trick core output files will be written to the specified <output_dir>.

### TimeStamping the Output Directory

The user may also specify a time stamped output directory in the input file.

```
trick.output_dir_timestamped_on()
```

This will create a dated subdirectory in the current output directory.  All of the Trick
core output files will be written to the output directory.  This option may be used in
conjunction with the <tt>-O</tt> or the <tt>-OO</tt> command line arguments.

[Continue to Environment](Environment)

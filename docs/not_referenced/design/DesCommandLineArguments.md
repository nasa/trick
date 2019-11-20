# Command Line Arguments Design

## Processing the Command Line Arguments

The main routine of the Command Line Arguments object is called by the
main program and the command line arguments of the main program are sent in
to the routine.  This routine copies the command line arguments and saves
them for all of the other Trick core classes to use. It also processes the
command line arguments to retrieve to output_dir, the input_file, the
default_dir, and the cmdline_name.

@copydetails Trick::CommandLineArguments::process_sim_args(int nargs , char **args)
Trick::CommandLineArguments::process_sim_args(int nargs , char **args)

## Timestamping the Output Directory

Before Simulation Initialization is complete the user may choose to
timestamp the output directory.

@copydetails Trick::CommandLineArguments::output_dir_timestamed_on()
Trick::CommandLineArguments::output_dir_timestamed_on()

## Removing Timestamping the Output Directory

@copydetails Trick::CommandLineArguments::output_dir_timestamed_off()
Trick::CommandLineArguments::output_dir_timestamed_off()



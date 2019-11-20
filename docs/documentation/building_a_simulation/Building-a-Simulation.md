The building blocks of a basic Trick simulation are C/C++ structures/classes (models), a Python input file and a Trick simulation definition file (S_define).  The S_define contains simulation objects which offer a way to turn the C/C++ function/methods into simulation jobs.  Trick generates the necessary Python glue code which makes the C/C++ structures/classes accessible by the Python input file.  The input file configures the simulation and is a command-line argument to the simulation executable.

... to be continued ...

1. As with any simulation development task, a Trick simulation development task begins with a system design. However, with Trick, the programmer does not have to start from scratch. Trick provides a time based cyclic executive, with a limited event capability, which schedules developer defined jobs (C subroutines) for execution based upon execution cycle frequencies and a multitude of different job classifications. This step can be viewed as fitting the developer’s "pieces" into the Trick "puzzle". The design time at this level should be relatively short, since we are talking about big pieces at this step in the development.
1. The next step involves source code development. The developer must design and implement the "jobs" which comprise the end-simulation. During source code development, the developer is required to adhere to stringent job interface (calling argument) specification guidelines, as well as a few in-code documentation guidelines. Otherwise, programming style is left to the developer. Math model source code design and implementation for a specific model is by far the most time consuming procedure in the entire simulation development scenario (excluding verification and validation). This is as it should be; i.e., simulations of the past often require abhorrent labor hours, not for math model development, but for executive and input and output mechanism development.
1. Next the developer must create a simulation definition file. The simulation definition file defines the source modules (jobs) and data structures used for a particular simulation. The simulation definition file contains the following information:
  * global data structures, including types, versions, source code names, and default initialization data files,
  * math model jobs (source code routines), including scheduled time intervals, version, calling argument specification (job interface) , and process specifier (for multi-process sim),
  * model delineation (job groupings),
  * state integration jobs and time intervals,
  * specialized parameter collections,
  * job dependencies for distributed process simulations.
1. With a complete simulation definition file, the developer invokes the Trick simulation Configuration Processor (CP). CP reads the simulation definition file and generates all simulation specific source code for the runtime executive, and all ASCII data base files for the user interface. CP also compiles the simulation specific source code and links in the object code libraries. Trick takes care of all executive, I/O, and file management chores that have traditionally given simulation developers fits in the past.
1. The developer may now create data product specification files, if data analysis is required. These files specify logged parameters to access, and display data in either plot or table format.
1. We now move into the simulation user domain (included in the developer’s domain). At this point the simulation is ready to operate. The user must first generate an input file.
1. The user now may execute one or more simulation runs.
1. During or after simulation execution the user may use the UI to post process simulation output data in either plot or tabular format.

[Continue to Model Source Code](Model-Source-Code)

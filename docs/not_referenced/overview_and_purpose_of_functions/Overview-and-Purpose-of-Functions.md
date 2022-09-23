The Trick processors and utilities covered in this document are listed in Table 1.

All functions listed in Table 1, can be started at the UNIX command prompt by entering the function’s name and associated arguments followed by a return. For help with CP, ICG and MIS, use the -h option. Man pages are available for CP, ICG and MIS too. Specific startup and termination characteristics for each function are discussed in the later subsections. Most of these functions can also be executed through the Graphical User Interfaces.

Name                         | Description
----                         | -----------
trick-CP                     | Configuration Processor - builds simulation executable
trick-fxplot                 | Data Products plots, tables, and displays generation
trick-dp                     | Data Products user interface
trick-ICG                    | Interface Code Generator - parses header files and builds I/O code
trick-killsim                | Kills runaways simulations
make                         | Standard UNIX make command
S_main_${TRICK_HOST_CPU}.exe | Specific Trick Simulation
trick-version                | Displays the currently installed Trick Release Version

**Table 1. Trick Processor and Utility Functions**

Figure 1 is a high-level picture of how a model developer interacts with Trick to produce a simulation executable. The developer creates math model source code, default data for the models, and a simulation definition file. The developer may optionally use external libraries he/she has already created. Once these tasks are complete, the developer runs the Configuration Processor (CP) on the simulation definition file (S_define). The Configuration Processor (CP) then does its magic. Using the S_define as a blue print, CP gathers the default math model data, auto generates IO code from source headers, parses math model source code to find object dependencies, job specifications etc., and then creates a master makefile. With this master makefile in hand, CP calls UNIX make, which creates the simulation executable. CP (and other utilities) also creates a database.

[[/not_referenced/overview_and_purpose_of_functions/images/figure_1.png|figure_1]]

**Figure 1 Trick Simulation Development Processor Interactions**

Figure 2 is a high-level picture of how a simulation user operates the simulation. The simulation developer/user is responsible for creating an “input” file for the simulation and data recording specification files (if needed). Once these are created, the simulation is ready to execute. The simulation will output data if the user has designated variables for logging. Data may be analyzed by creating a Data Products specification file.e that contains data structure information and model job info. The “catalog” utility provides a means to view the database.

[[/not_referenced/overview_and_purpose_of_functions/images/figure_2.png|figure_2]]

**Figure 2 Trick Simulation Operation**

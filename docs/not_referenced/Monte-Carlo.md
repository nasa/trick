
Trick offers a convenient method for repeatedly running a simulation with varying inputs. We call this capability
"Monte Carlo". Monte Carlo is a well-known technique where mathematical problems are solved using random numbers
and probability statistics. By "Monte Carlo", we mean running the simulation repeatedly over a varying input space.
How the inputs vary is up to you, the developer. How the input space is varied may not fall into the strict definition
of Monte Carlo, i.e. using bell curves, linear distributions, etc.

Use this section as a reference. Refer to the tutorial for a full blown example.

### Monte Carlo Tutorial

The tutorial has an example of how to use Trick-based Monte Carlo. The example shows how to use Monte Carlo to optimize
the ground distance of a jet-propelled ball. The ball has a jet which yields upward force. The jet may only fire four
times. The firing times are set in the input file. The Monte Carlo technique is used to run the simulation repeatedly
with varying jet-firing sequences. The tutorial shows how to use predetermined (hard-coded) sequences, as well as how
to use random sequences. The tutorial also shows how to use data products to analyze the multitudinous curves that a
Monte Carlo simulation produces.

### Structure

The curious will want to know the internal design of Monte Carlo. In the case of optimization, or when feeding new inputs
to the simulation based on past results, the design becomes prerequisite. That said, here are a few brief points about
the design:

- Monte Carlo is designed to be distributed. You must have ssh or rsh setup to run Monte Carlo. Runs may occur in parallel
  across a network. In fact, all Monte Carlo runs are distributed, even when running with one machine.
- There is no wrapper or script around S_main*.exe. Monte Carlo is a child class of the Executive class.
- GNU's standard library is used to generate "random" data.
- The simulation input file is only processed once (by each distributed "slave").
- fork() is used to keep simulation runs in their own address space.
- Optimization is possible through special developer-written jobs that wrap the simulation run. These special jobs make
  it possible to change run inputs based on past simulation results.
- To run Monte Carlo, you run the S_main*.exe as usual. All configuration is done with input files.
- All data is saved for each and every run. Data is saved in a MONTE_* directory. The MONTE_* directory will hold a
  RUN_* directory for each simulation run.
- stderr, stdout, and send_hs from distributed "slaves" are saved to files.
- Post-processing is in place to view 1000+ curves simultaneously.
- A master which oversees the creation and management of slaves and the dispatching of runs.
- One or more slaves that process the runs and return results to the master.

#### The Master

The master is the command center of a Monte Carlo simulation. It does not process runs directly. Rather, it delegates them
to one or more slaves which perform the actual execution. The master is responsible for spawning and managing the state of
these slaves, dispatching and tracking the progress of each run, and handling the retrying of failed and timed out runs.
Its life cycle consists of the following:

- Initialize
- While there are unresolved runs:
  - Spawn any uninitialized slaves.
  - Dispatch runs to ready slaves.
  - Receive results from finished slaves.
  - Check for timeouts.
- Shutdown the slaves and terminate.

#### Slaves

Slaves are the workhorses of a Monte Carlo simulation. They are responsible for the actual execution of runs and the
reporting of results back to the master. Slaves are robust enough to handle runs that fail abnormally and will continue
executing until explicitly killed or disconnected. A slave's life cycle consists of the following:

- Initialize
- Connect to and inform the master of the port over which the slave is listening for dispatches.
- Until the connection to the master is lost or the master commands a shutdown:
  - Wait for a new dispatch.
  - Process the dispatch.
  - Write the exit status to the master.
- Run the shutdown jobs and terminate.

### Setting Up a Monte Carlo Simulation

It is important to note that the only initialization jobs the master runs are those with phase zero. As such, if one
wishes to use the functions in the following discussion in user model code, a few will have effect only in phase zero
initialization jobs. These jobs are explicitly noted below.

#### Monte Carlo Remote Shell

To start Monte Carlo slaves you must have either rsh or ssh installed. It is best to setup the remote shell so that it
doesn't prompt for a password every time you run it. See tutorial for a tiny ssh test.

#### Activating Monte Carlo

A Monte Carlo simulation is enabled via one of:

<b>C++:</b> Trick::MonteCarlo::set_enabled <br>
<b>C:</b> ::mc_set_enabled

#### Specifying the Number of Runs

This tells Monte Carlo how many simulation runs to execute. For a series of random runs, Monte Carlo will execute the
simulation the number of runs specified. When MonteVarFile is specified as the input variable's type, Trick will execute
the number of runs specified, not exceeding the number of values contained in the input variable's data file. For multiple
MonteVarFile variables, Trick will execute the least number of runs specified, not exceeding the least number of values
contained in the input variable's data file.

The number of runs to be dispatched is specified via one of:

<b>C++:</b> Trick::MonteCarlo::set_num_runs<br>
<b>C:</b> ::mc_set_num_runs

#### Ranges

This optional section tells Monte Carlo which runs to execute.

A subset of runs to be dispatched can be achieved via one of:

<b>C++:</b> Trick::MonteCarlo::add_range<br>
<b>C:</b> ::mc_add_range

All ranges will be combined, and runs falling in any of the specified ranges will be dispatched. If no ranges are
specified, all runs will be dispatched. For example, the following lines in the input file result in runs 100 through
200, 250, and 300 through 500 being dispatched:

```python
trick.mc_add_range(100, 200)
trick.mc_add_range(250)
trick.mc_add_range(300, 500)
```

#### Monte Carlo Input Variables

The following classes (which are derived from the Trick::MonteVar abstract base class) are used to specify
which input variables are available for changing from run to run. The type of class tells Trick how to
generate the value for the variable from run to run.

- Trick::MonteVarCalculated
  - The user feeds Monte Carlo with calculated values. The values are calculated in user-created Monte Carlo
    jobs. The primary purpose of the MonteVarCalculated type formula is for optimization (see the Optimization section) <br>
  Parameter Descriptions:
    - name - the fully qualified name of the simulation variable
    - unit - the variable's units.
- Trick::MonteVarFile
  - Pull values from an input file. Use MonteVarFile when you want to hard-code various values.  
  Parameter Descriptions:
    - name - the fully qualified name of the simulation variable
    - file_name - the name of the file containing the values to use
    - column - the column in the data file containing the values for this variable
    - unit - the variable's units  
  Below is an example of an input file. The data should be in tabular format with each column
  containing the data for a variable and each row is for a different run.
    - Column 1 contains the run number (optional, the values can begin in column 1).  
    - Column 2 contains the values for the first variable.  
    - Column 3 contains the values for the second variable.
```
#this is a comment
0 1.00000 1.50000
1 1.50000 2.00000
2 2.00000 2.50000
3 2.50000 3.00000
```
- Trick::MonteVarFixed
  - Use this class type to specify a constant value. <br>
  Parameter Descriptions:
    - name - the fully qualified name of the simulation variable
    - value - the constant value to use for the variable
    - unit - the variable's units.
- Trick::MonteVarRandom
  - Use this class type to auto-generate the input values using a distribution formula. <br>
  Parameter Descriptions:
    - name - the fully qualified name of the simulation variable
    - unit - the variable's units
    - distribution - the random distribution method (GAUSSIAN, FLAT, or POISSON)
      - <b> GAUSSIAN </b>: This specifies the following probability density function to be used for the variable: <br>
       <i>P(x)= 0.0, if  x < min <br>
       Or: 0.0, if  x > max <br>
       Or: 0.0, if  x < rel_min +\f$\mu\f$ <br>
       Or: 0.0, if  x > rel_max +\f$\mu\f$ <br>
       Otherwise: \f${\displaystyle\frac{1}{\sigma\sqrt{2 \pi}} \; exp\biggr(-\frac{(x-\mu)^2}{2\mu^2}\biggr)}\f$ </i> <br>
      Gaussian Parameter Descriptions:
        - Seed - the randomization seed. (use Trick::MonteVarRandom::set_seed))
        - Sigma - the standard deviation. The larger the value, the broader the bell curve. (use Trick::MonteVarRandom::set_sigma))
        - Mu - specifies the center of the bell curve. (use Trick::MonteVarRandom::set_mu)
        - Min, Max - absolute cutoff limits. Any values ouside of these bounds are discarded. (use Trick::MonteVarRandom::set_min, Trick::MonteVarRandom::set_max)
        - Rel_min, Rel_max - cutoff limits relative to mu \f$(\mu)\f$. Any values ouside of these bounds are discarded.
                           (use Trick::MonteVarRandom::set_min_is_relative, Trick::MonteVarRandom::set_max_is_relative)
      - <b> POISSON </b>: This specifies the following probability density function to be used for the variable:\n
      <i> P(n)= \f${\displaystyle\frac{\mu^ne^{-\mu}}{n!}}\f$ </i> <br>
      Poisson Parameter Descriptions:
        - Seed - the randomization seed. (use Trick::MonteVarRandom::set_seed)
        - Mu\f$(\mu)\f$ - non-negative, real-valued number that specifies the mean of the distribution. (use Trick::MonteVarRandom::set_mu)
        - Min, Max - absolute, non-negative, cutoff limits. Any values outside of these bounds are discarded.
                   (use Trick::MonteVarRandom::set_min, Trick::MonteVarRandom::set_max)
        - Rel_min, Rel_max - cutoff limits relative to mu \f$(\mu)\f$. Any values outside of these bounds are discarded.
                           (use Trick::MonteVarRandom::set_min_is_relative, Trick::MonteVarRandom::set_max_is_relative)
      - <b> FLAT</b>: Uniform distribution. No bell. Returns uniform random values between (-\f$\infty\f$, +\f$\infty\f$) bracketed
            optionally by [min, max]. (use Trick::MonteVarRandom::set_min, Trick::MonteVarRandom::set_max)
    - engine - the C++11 predefined pseudo-random engine type. NO_ENGINE is the default (results in Trick coded random number engine).
      Other options using the C++11 <random> facilities of the Standard Template Library:
      (Requires --std=c++0x or --std=c++11 on Trick configure command line when building Trick.)
        - TRICK_DEFAULT_ENGINE    - <b>SUGGESTED FOR USE</b>. std::ranlux_base_01 for c++0x, std::mt19937 for c++11
        - C++ TR1 options: (pre-C++11 compiler, GCC versions 4.4 through 4.6). 
          (Requires --std=c++0x on Trick configure command line when building Trick.)
          - RANLUX_BASE_01_ENGINE - std::ranlux_base_01 Engine
          - RANLUX_64_BASE_01_ENGINE - std::ranlux64_base_01 Engine
          - (others such as std::mt19937 not provided, because they return 
             outside the canonical 0 <= x < 1 range in some GCC versions, 
             which can cause infinite loops in distributions.) 
        - C++11 options: (C++11 compiler, versions 4.7, 4.8). 
          (Requires --std=c++11 on Trick configure command line when building Trick.)
          - MINSTD_RAND_ENGINE    - std::minstd_rand Minimal Standard Linear Congruential Engine
          - MT19937_ENGINE        - std::mt19937 Mersenne Twister Engine. Said to provide better behavior than Linear Congruential Engines.
          - MT19937_64_ENGINE     - std::mt19937_64  64 bit Mersenne Twister Engine.
          - RANLUX_24_BASE_ENGINE - std::ranlux24_base Engine
          - RANLUX_44_BASE_ENGINE - std::ranlux48_base Engine
          - RANLUX_24_ENGINE      - std::ranlux24 Engine
          - RANLUX_44_ENGINE      - std::ranlux48 Engine
          - KNUTH_B_ENGINE        - std::knuth_b Engine

After constructing such a variable, it can be added via:

<b>C++:</b> Trick::MonteCarlo::add_variable

C wrapper functions are not available for creating and adding variables. As such, C simulations can add variables
<i>only</i> through the input file. To create a variable in the input file, prepend the constructor with
`trick.`. For example:

```python
variable0 = trick.MonteVarCalculated("ball.obj.state.input.mass")
variable0.thisown = 0 # tell Python not to free the underlying C++ class when the wrapper is garbage collected
variable1 = trick.MonteVarFile("ball.obj.state.input.position[0]", "RUN_monte/values.txt", 2)
variable1.thisown = 0
variable2 = trick.MonteVarFixed("ball.obj.state.input.position[1]", 5)
variable2.thisown = 0
variable3 = trick.MonteVarRandom("ball.obj.state.input.velocity[0]", trick.MonteVarRandom.GAUSSIAN, "", 
variable3.thisown = 0
trick.MonteVarRandom.NO_ENGINE)
variable3.set_seed(1)
variable3.set_sigma(0.6667)
variable3.set_mu(4.0)
variable3.set_min(-4.0)
variable3.set_max(4.0)
variable3.set_sigma_range(0)  # integer argument, default is 1. 0 turns limit feature off.
variable3.set_min_is_relative(True) # default true. When True, set_min value is relative to mean mu
variable3.set_max_is_relative(True) # default true. When True, set_max value is relative to mean mu
```

Calling a C++ function in the input file is not as simple as prepending it with `trick.`. To add a variable
in the input file, use the following syntax:

```python
trick_mc.mc.add_variable(variable0)
trick_mc.mc.add_variable(variable1)
trick_mc.mc.add_variable(variable2)
trick_mc.mc.add_variable(variable3)
```

Variables can also be added from jobs of type <code>"monte_master_pre"</code> or <code>"monte_master_post"</code> while
the Monte Carlo is running. Note that new variables will effect only runs that have yet to be dispatched.

### Distributed Monte Carlo

To run Monte Carlo distributed across a network, you simply need to call `add_slave` for each slave.

<b>C++:</b> Trick::MonteCarlo::add_slave  
<b>C:</b> ::mc_add_slave

```python
slave0 = trick.MonteSlave("WonderWoman")
slave0.thisown = 0 # tell Python not to free the underlying C++ class when the wrapper is garbage collected
trick_mc.mc.add_slave(slave0)
slave1 = trick.MonteSlave("CatWoman")
slave1.thisown = 0
trick_mc.mc.add_slave(slave1)
slave2 = trick.MonteSlave("LoisLane")
slave2.thisown = 0
trick_mc.mc.add_slave(slave2)
```

It is really that easy.  But the following bullets need to be remembered:
- ssh is used to launch simulations across the network.
- Each slave machine will work in parallel with other machines.
- Each slave will do as much work as it can. The faster the machine, the more work it will do.
- If a slave dies, the master is smart enough to redistribute the missing work.
- Communication between master and slave(s) will be done with socket communication (handshaking_disabled)
- There is no way to be "nice" to other users on a machine.  The Monte Carlo is going to hog the CPU.
- Monte Carlo runs distributed even when there are NO slaves. If no slaves are added, Trick will add a single slave on "localhost".
Slaves can also be added from jobs of type `"monte_master_pre"` or `"monte_master_post"`
while the Monte Carlo is running.

### Output
Data logged for each run is stored in a <code>RUN_<run number></code> directory within a
<code>MONTE_<run directory></code> directory on the machine that processed the run. Existing directories and files will be
overwritten. These directories are not cleaned out by subsequent Monte Carlos. So, for instance, if the user runs a Monte
Carlo with 1000 runs, and then reruns the same Monte Carlo with 500 runs, the first 500 <code>RUN_*</code> directories
will contain data from the second Monte Carlo, while the last 500 will still exist and contain data from the first Monte
Carlo. The following files are Monte Carlo specific:

- <b>MONTE_<run directory>/monte_header</b><br>
  This file contains the input file lines that configured the initial state of the Monte Carlo, such as information on the
  number of runs and variables. This file is also created during a dry_run.

- <b>MONTE_<run directory>/monte_runs</b><br>
  This file lists the values used for each variable for each run. This file is also created during a dry_run.

- <b>MONTE_<run directory>/run_summary</b><br>
  This file contains the summary statistical information that is printed out to the screen after a Monte Carlo completes.

- <b>MONTE_<run directory>/RUN_\<run number\>/monte_input</b><br>
  This file contains the input file commands necessary to rerun a single run as a stand alone simulation.

### Dry Run
A dry run generates only the monte_header and monte_runs files without actually processing any runs. It is useful for
verifying input values before running a full Monte Carlo. A dry run is specified via one of:

<b>C++:</b> Trick::MonteCarlo::set_dry_run<br>
<b>C:</b> ::mc_set_dry_run

### Making Monte Carlo Less Verbose

By default, Monte Carlo is fairly verbose. If you need to suppress the messages from a Monte Carlo run:

<b>C++:</b> Trick::MonteCarlo::set_verbosity <br>
<b>C:</b> ::mc_set_verbosity

Possible values for the verbosity argument are:
- NONE (0) - report no messages
- ERROR (1) - report error messages
- INFORMATIONAL (2) - report error and informational messages
- ALL (3) - report all messages

### Optimization

Monte Carlo has no decision making capability. It runs a predetermined set of inputs or a random set. In order to optimize,
it is usually necessary to base current inputs from past results. Intelligence must be involved for the decision making.
Currently, Trick has no built-in "intelligence" for optimization. It offers a framework for you, the brains, to
optimize the simulation. The framework allows on-the-fly input modification based on past simulation results.

The framework is a set of monte jobs which run at critical times for analyzing results and building new inputs. The
jobs are written by the developer. The job's class determines what role the job plays (i.e. where it is run) in the
optimization process. The Monte Carlo classed job is specified in the S_define like any other job.

In order to get a handle on how to plug in to the optimization framework, it helps to have a better understanding of the
roles the master and slave play in the master/slave design.

The below table contains a description of the Monte Carlo specific Trick jobs:

<table>
<tr><th>Trick Job </th> <th>Description </th>
<tr><td><b>monte_master_init</b><br>
<td>Runs once in the master before any slaves are spawned.</td></tr>
<tr><td><b>monte_slave_init</b><br></td>
<td>Runs once in each slave upon spawning.</td></tr>
<tr><td><b>monte_master_pre</b><br></td>
<td>Runs in the master before each run is dispatched. This is where you could modify inputs before they are sent to a slave.</td></tr>
<tr><td><b>monte_slave_pre</b><br></td>
<td>Runs in the slave before each dispatch received from the master is executed. Inputs are processed before this job.</td></tr>
<tr><td><b>monte_slave_post</b><br></td>
<td>Runs in the slave each time this slave completes a run. This is where you could send custom results back to the master.</td></tr><tr><td><b>monte_master_post</b><br></td>
<td>Runs in the master each time a slave completes a run. This is where you could receive custom results from a slave.</td></tr>
<tr><td><b>monte_slave_shutdown</b><br></td>
<td>Runs once in the slave before termination.</td></tr>
<tr><td><b>monte_master_shutdown</b><br></td>
<td>Runs once in the master after all runs have completed.</td></tr>
</table>

#### The Post-Run Connection

Post-run communication can be done via the Trick comm package in the post-run jobs. The underlying sockets are already connected at the time the post-run jobs are executed, so the user can simply use the C wrapper functions `::mc_read` and `::mc_write` to pass additional data between the master and slave.

#### Where To Put Optimization Code

Since the master is in charge of dispatching, the optimization code will reside in the master. It is debatable whether to put the actual decision making in the `monte_master_pre` or `monte_master_post` jobs. The tutorial uses `monte_master_pre`, which may seem less intuitive, since the `monte_master_post` is the one receiving the results. It turned out that the decision making for that particular algorithm was easier before the run rather than after. This is not a hard and fast rule. Wherever it makes sense for the problem at hand is where the decision making should go.

### Job Order

This is the order in which jobs are executed in a MonteCarlo sim:

- Program starts
  - Master runs constructors
  - Master runs default data jobs
  - Master processes input file
  - Master runs monte_master_init jobs

- For each slave
  - Master spawns slave
  - Slave runs constructors
  - Slave runs default_data jobs
  - Slave processes input file (the same one that the master is using)
  - Slave runs monte_slave_init jobs

- For each run sent to a slave by the master
  - Master runs monte_master_pre jobs
  - Slave parses input from master (this is where the variables being swept are set)
  - Slave runs monte_slave_pre jobs
  - Slave runs initialization jobs
  - Slave runs the simulation
  - Slave runs shutdown jobs
  - Slave runs monte_slave_post jobs
  - Master runs monte_master_post jobs

- When all runs have completed
  - Each slave runs monte_slave_shutdown jobs
  - Master runs monte_master_shutdown jobs

### User Accessible Routines

- ::mc_set_enabled
- ::mc_get_enabled
- ::mc_set_dry_run
- ::mc_get_dry_run
- ::mc_set_localhost_as_remote
- ::mc_get_localhost_as_remote
- ::mc_set_custom_slave_dispatch
- ::mc_get_custom_slave_dispatch
- ::mc_set_timeout
- ::mc_get_timeout
- ::mc_set_max_tries
- ::mc_get_max_tries
- ::mc_set_user_cmd_string
- ::mc_get_user_cmd_string
- ::mc_set_custom_pre_text
- ::mc_get_custom_pre_text
- ::mc_set_custom_post_text
- ::mc_get_custom_post_text
- ::mc_set_verbosity
- ::mc_get_verbosity
- ::mc_set_num_runs
- ::mc_get_num_runs
- ::mc_set_run_directory
- ::mc_get_run_directory
- ::mc_get_slave_id
- ::mc_add_range
- ::mc_add_slave
- ::mc_start_slave
- ::mc_stop_slave
- ::mc_set_output_directory
- ::mc_disable_slave_GUIs
- ::mc_write
- ::mc_read
- ::mc_get_connection_device
- ::mc_set_current_run
- ::mc_get_current_run
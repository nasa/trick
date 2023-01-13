# MonteCarloGeneration Model

# Revision History
| Version | Date | Author | Purpose |
| :--- |:---| :--- | :--- |
| 1 | April 2020 | Gary Turner | Initial Version |
| 2 | March 2021 | Gary Turner | Added Verification |
| 3 | October 2022 | Isaac Reaves | Converted to Markdown |

# 1 Introduction

The MonteCarlo Model is used to disperse the values assigned to variables at the start of a simulation. Dispersing the initial
conditions and configurations for the simulation allows for robust testing and statistical analysis of the probability of
undesirable outcomes, and measuring the confidence levels associated with achieving desirable outcomes.

Conventionally, most of the time we think about dispersing variables, we think about apply some sort of statistical
distribution to the value. Most often, that is a normal or uniform distribution, but there may be situations in which other
distributions are desired. In particular, this model provides an extensible framework allowing for any type of distribution to
be applied to any variable.

For extensive analysis of safety-critical scenarios, where it is necessary to demonstrate high probability of success with high
confidence, traditional MonteCarlo analyses require often many thousands of runs. For long duration simulations, it may
not be feasible to run the number of simulations necessary to reach the high confidence of high success probability that is
necessary to meet requirements. Typically, failure cases occur out near the edges of state-space, but most of the runs will be
“right down the middle”; using conventional MonteCarlo techniques, most of these runs are completely unnecessary. With
a Sequential-MonteCarlo configuration, a small number of runs can be executed, allowing for identification of problem
areas, and a focussing of the distribution on those areas of state-space, thereby reducing the overall number of runs while
adding complexity to the setup. While this model does not (at this time) provide a Sequential-MonteCarlo capability, the
organization of the model has been designed to support external tools seeking to sequentially modify the distributions being
applied to the dispersed variables, and generate new dispersion sets.

# 2 Requirements

1. The model shall provide common statistical distribution capabilities, including:
    1. Uniform distribution between specified values
        1. as a floating-point value
        1. as an integer value
    1. Normal distribution, specified by mean and standard deviation
    1. Truncated Normal Distribution, including
        1. symmetric and asymmetric truncations
        1. it shall be possible to specify truncations by:
            1. some number of standard deviations from the mean,
            1. a numerical difference from the mean, and
            1. an upper and lower limit
1. The model shall provide an extensible framework suitable for supporting other statistical distributions
1. The model shall provide the ability to assign a common value to all runs:
    1. This value could be a fixed, user-defined value
    1. This value could be a random assignment, generated once and then applied to all runs
1. The model shall provide the capability to read values from a pre-generated file instead of generating its own values
1. The model shall provide the ability to randomly select from a discrete data set, including:
    1. enumerations,
    1. character-strings,
    1. boolean values, and
    1. numerical values
1. The model shall provide the capability to compute follow-on variables, the values of which are a function of one or more dispersed variables with values generated using any of the methods in requirements 1-5.
1. The model shall provide a record of the generated distributions, allowing for repeated execution of the same scenario using exactly the same conditions.
1. The model shall provide summary data of the dispersions which have been applied, including:
    1. number of dispersions
    1. types of dispersions
    1. correlations between variables

# 3 Model Specification

## 3.1 Code Structure

The model can be broken down into its constituent classes; there are two principle components to the model – the variables,
and the management of the variables.

### 3.1.1 Variable Management (MonteCarloMaster) 

MonteCarloMaster is the manager of the MonteCarlo variables. This class controls how many sets of dispersed variables
are to be generated; for each set, it has the responsibility for
* instructing each variable to generate its own dispersed value
* collecting those values and writing them to an external file

### 3.1.2 Dispersed Variables (MonteCarloVariable)

MonteCarloVariable is an abstract class that forms the basis for all dispersed variables. The following classes inherit from
MonteCarloVariable:
* MonteCarloVariableFile will extract a value for its variable from a specified text file. Typically, a data file will
comprise some number of rows and some number of columns of data. Each column of data represents the possible
values for one variable. Each row of data represents a correlated set of data to be applied to several variables; each
data-set generation will be taken from one line of data. Typically, each subsequent data-set will be generated from the
next line of data; however, this is not required.
* In some situations, it is desirable to have the next line of data to be used for any given data set be somewhat randomly
chosen. This has the disadvantageous effect of having some data sets being used more than others, but it supports
better cross-population when multiple data files are being used. 
    * For example, if file1 contained 2 data sets and file2 contained 4 data sets, then a sequential sweep through
these file would set up a repeating pattern with line 1 of file2 always being paired with line 1 of file1. For
example, in 8 runs, we would get this pattern of line numbers from each run:
        * (1,1), (2,2), (1,3), (2,4), (1,1), (2,2), (1,3), (2,4)
    * If the first file was allowed to skip a line, the pattern can produce a more comprehensive combination of
data:
        * (1,1), (1,2), (2,3), (1,4), (2,1), (2,2), (2,3), (1,4)
* MonteCarloVariableFixed provides fixed-values to a variable for all generated data-sets. The values can be
represented as a double, int, or STL-string. 
* MonteCarloVariableRandom is the base class for all variables being assigned a random value. The values can be
represented as a double, int, or STL-string. There are several subclasses:
    * MonteCarloVariableRandomNormal provides a variable with a value dispersed according to a normal
distribution specified by its mean and standard deviation.
    * MonteCarloVariableRandomUniformInt provides a variable with a value dispersed according to a uniform
distribution specified by its upper and lower bounds. This class represents a discrete distribution, providing an
integer value.
    * MonteCarloVariableRandomUniform provides a variable with a value dispersed according to a uniform
distribution specified by its upper and lower bounds. This class represents a continuous distribution.
        * MonteCarloVariableRandomStringSet represents a discrete variable, drawn from a set of STL-strings. The
class inherits from MonteCarloVariableRandomUniform; this distribution generates a continuous value in [0,1)
and scales and casts that to an integer index in {0, …, size-1} where size is the number of available strings
from which to choose.

   Note – an astute reader may question why the discrete MonteCarloVariableRandomStringSet inherits from
the continuous MonteCarloVariableRandomUniform rather than from the discrete
MonteCarloVariableRandomUniformInt. The rationale is based on the population of the vector of
selectable strings in this class. It is desirable to have this vector be available for population outside the
construction of the class, so at construction time the size of this vector is not known. However, the
construction of the MonteCarloVariableRandomUniformInt requires specifying the lower and upper
bounds, which would be 0 and size-1 respectively. Because size is not known at construction, this cannot
be specified. Conversely, constructing a MonteCarloVariableRandomUniform with bounds at [0,1) still
allows for scaling to the eventual size of the strings vector.

* MonteCarloVariableSemiFixed utilizes a two-step process. First, a seed-variable has its value generated, then that
value is copied to this variable. The seed-variable could be a “throw-away” variable, used only to seed this value, or it
could be an instance of another dispersed variable. Once the value has been copied to this instance, it is retained in this
instance for all data sets. The seed-variable will continue to generate a new value for each data set, but they will not be
seen by this variable after that first set.

   The seed-variable can be any type of MonteCarloVariable, but note that not all types of MonteCarloVariable actually
make sense to use in this context. Most of the usable types are specialized types of MonteCarloVariableRandom.

   However, restricting the seed-variable in such a way would limit the extensibility of the model. All
MonteCarloVariableRandom types use the C++ \<random\> library for data generation. Limiting the
MonteCarloVariableSemiFixed type to be seeded only by something using the \<random\> library violates the concept of
free-extensibility. Consequently, the assigned value may be extracted from any MonteCarloVariable type. The only
constraint is that the command generated by the seed-variable includes an “=” symbol; everything to the right of that
symbol will be assigned to this variable.

* MonteCarloPythonLineExec provides a line of executable Python code that can be used to compute the value of this
variable. So rather than generating an assignment statement, e.g.

```
var_x = 5
```

when the MonteCarloMaster processes an instance of this class, it will use a character string to generate an
instruction statement, e.g.

```
var_x = math.sin(2 * math.pi * object.circle_fraction)
```

(in this case, the character string would be “math.sin(2 * math.pi * object.circle_fraction)” and
object.circle_fraction could be a previously-dispersed variable).

   A particularly useful application of this capability is in generating systematic data sweeps across a domain, as
opposed to random distributions within a domain. These are commonly implemented as a for-loop, but we can use
the MonteCarloPythonLineExec to generate them internally. The first data assignment made in each file is to a
run-number, which can be used as an index. The example shown below will generate a sweep across the domain
[20,45) in steps of 2.5.

```
object.sweep_variable = (monte_carlo.master.monte_run_number % 10) * 2.5 + 20
```

   * MonteCarloPythonFileExec is used when simple assignments and one-line instructions are insufficient, such as
when one generated-value that feeds into an analytical algorithm to generate multiple other values. With this class,
the execution of the Python file generated by MonteCarloMaster will hit a call to execute a file as specified by this
class. This is an oddity among the bank of MonteCarloVariable implementations. In all other implementations,
the identifying variable_name is used to identify the variable whose value is to be assigned (or computed). With
the MonteCarloPythonFileExec implementation, the variable_name is hijacked to provide the name of the file to
be executed.

## 3.2 Mathematical Formulation

No mathematical formulation. The random number generators use the C++ \<random\> library.

# 4 User's Guide

## 4.1 What to expect

This role played by this model can be easily misunderstood, so let’s start there.
**This model generates Python files containing assignments to variables.**

That’s it!! It does not manage MonteCarlo runs. It does not execute any simulations. When it runs, it creates the requested
number of Python files and exits.

This design is deliberate; we want the model to generate the instruction sets that will allow execution of a set of dispersed
configurations. At that point, the simulation should cease, returning control to the user to distribute the execution of those
configurations according to whatever distribution mechanism they desire. This could be:

* something really simple, like a wild-card, \<executive\> `MONTE_RUN_test/RUN*/monte_input.py`
* a batch-script,
* a set of batch-scripts launching subsets onto different machines,
* a load-management service, like SLURM
* any other mechanism tailored to the user’s currently available computing resources

The intention is that the model runs very early in the simulation sequence. If the model is inactive (as when running a regular, non-MonteCarlo run), it will take no action. But when this model is activated, the user should expect the simulation to terminate before it starts on any propagation.

**When a simulation executes with this model active, the only result of the simulation will be the generation of files containing the assignments to the dispersed variables. The simulation should be expected to terminate at t=0.**

## 4.1.1 Trick Users

The model is currently configured for users of the Trick simulation engine. The functionality of the model is almost exclusively independent of the chosen simulation engine, with the exceptions being the shutdown sequence, and the application of units information in the variables.

Found at the end of the `MonteCarloMaster::execute()` method, the following code:

```c++
exec_terminate_with_return(0, __FILE__, __LINE__,message.c_str());
```

is a Trick instruction set to end the simulation.

Found at the end of `MonteCarloVariable::insert_units()`, the following code:

```c++
// TODO: Pick a unit-conversion mechanism
// Right now, the only one available is Trick:
trick_units( pos_equ+1);
```

provides the call to

```c++
MonteCarloVariable::trick_units(
 size_t insertion_pt)
{
 command.insert(insertion_pt, " trick.attach_units(\"" + units + "\",");
 command.append(")");
```

which appends Trick instructions to interpret the generated value as being represented in the specified units.

The rest of the User’s Guide will use examples of configurations for Trick-simulation input files

## 4.1.2 Non-Trick Users

To configure the model for simulation engines other than Trick, the Trick-specific content identified above should be replaced with equivalent content that will result in:
* the shutdown of the simulation, and
* the conversion of units from the type specified in the distribution specification to the type native to the variable to which the generated value is to be assigned.

While the rest of the User’s Guide will use examples of configurations for Trick-simulation input files, understand that these are mostly just C++ or Python code setting the values in this model to make it work as desired. Similar assignments would be required for any other simulation engine.

## 4.2 MonteCarlo Manager (MonteCarloMaster)

### 4.2.1 Instantiation

The instantiation of MonteCarloMaster would typically be done directly in the S_module. The construction of this instance takes a single argument, a STL-string describing its own location within the simulation data-structure.

The MonteCarloMaster class has a single public-interface method call, `MonteCarloMaster::execute()`. This method has 2 gate-keeping flags that must be set (the reason for there being 2 will be explained later):
* `active`
* `generate_dispersions`

If either of these flags is false (for reference, `active` is constructed as false and `generate_dispersions` is constructed as true) then this method returns with no action. If both are true, then the model will generate the dispersions, write those dispersions to the external files, and shut down the simulation.

An example S-module

```c++
class MonteCarloSimObject : public Trick::SimObject
{
 public:
 MonteCarloMaster master; // <--- master is instantiated
 MonteCarloSimObject(std::string location)
 :
 master(location) // <--- master is constructed with this STL-string
 {
 P_MONTECARLO ("initialization") master.execute(); // <--- the only function
call
 }
};
MonteCarloSimObject monte_carlo("monte_carlo.master"); // <--- location of “master”
                                                               is passed as an
                                                               argument
```

### 4.2.2 Configuration

The configuration of the MonteCarloMaster is something to be handled as a user-input to the simulation without requiring re-compilation; as such, it is typically handled in a Python input file. There are two sections for configuration:
* modifications to the regular input file, and
* new file-input or other external monte-carlo initiation mechanism

#### 4.2.2.1 Modifications to the regular input file

A regular input file sets up a particular scenario for a nominal run. To add monte-carlo capabilities to this input file, the
following code should be inserted somewhere in the file:

```python
if monte_carlo.master.active:
 # insert non-mc-variable MC configurations like logging
 if monte_carlo.master.generate_dispersions:
 exec(open(“Modified_data/monte_variables.py").read())
```

Let’s break this down, because it explains the reason for having 2 flags:

| `generate_dispersions` | `active` | Result |
| :--- |:---| :--- |
| false | false | Regular (non-monte-carlo) run |
| false | true | Run scenario with monte-carlo configuration and pre-generated dispersions |
| true | false | Regular (non-monte-carlo) runs |
| true | true | Generate dispersions for this scenario, but do not run the scenario |

 1. If the master is inactive, this content is passed over and the input file runs just as it would without this content
 2. Having the master `active` flag set to true instructs the simulation that the execution is intended to be part of a monte-carlo analysis. Now there are 2 types of executions that fit this intent:
    * The generation of the dispersion files
    * The execution of this run with the application of previously-generated dispersions

Any code to be executed for case (a) must go inside the `generate_dispersions` gate. Any code to be executed for
case (b) goes inside the `active` gate, but outside the `generate_dispersions` gate.

You may wonder why this distinction is made. In many cases, it is desirable to make the execution for monte-carlo
analysis subtly different to that for regular analysis. One commonly used distinction is logging of data; the logging
requirement may differ between a regular run and one as part of a monte-carlo analysis (typically, monte-carlo runs
execute with reduced logging). By providing a configuration area for a monte-carlo run, we can support these
distinctions.
Note – any code to be executed for only non-monte-carlo runs can be put in an else: block. For example, this code
will set up one set of logging for a monte-carlo run, and another for a non-monte-carlo run of the same scenario:
```python
if monte_carlo.master.active:
 exec(open(“Log_data/log_for_monte_carlo.py”).read())
 if monte_carlo.master.generate_dispersions:
 exec(open(“Modified_data/monte_variables.py").read())
else:
 exec(open(“Log_data/log_for_regular.py”).read())
```
 3. If the `generate_dispersions` flag is also set to true, the `MonteCarloMaster::execute()` method will execute,
generating the dispersion files and shutting down the simulation.

#### 4.2.2.2 Initiating MonteCarlo

Somewhere outside this file, the `active` and generate_dispersion flags must be set. This can be performed either in a separate input file or via a command-line argument. Unless the command-line argument capability is already supported, by far the easiest mechanism is to create a new input file that subsequently reads the existing input file:

```
monte_carlo.master.activate("RUN_1")
exec(open("RUN_1/input.py").read())
```

The activate method takes a single string argument, representing the name of the run. This must be exactly the same name as the directory containing the original input file, “RUN_1” in the example above. This argument is used in 2 places (\<argument\> in these descriptions refers to the content of the argument string):

* In the creation of a `MONTE_<argument>` directory. This directory will contain some number of sub-directories identified as, for example, RUN_01, RUN_02, RUN_03, etc. each of which will contain one of the generated dispersion files.
* In the instructions written into the generated dispersion files to execute the content of the input file found in `<argument>`.

#### 4.2.2.3 Additional Configurations

There are additional configurations instructing the MonteCarloMaster on the generation of the new dispersion files. Depending on the use-case, these could either be embedded within the `if monte_carlo.master.generate_dispersions:` block of the original input file, or in the secondary input file (or command-line arguments if configured to do so).

* Number of runs is controlled with a single statement, e.g.

    ```monte_carlo.master.set_num_runs(10)```

* Generation of meta-data. The meta-data provides a summary of which variables are being dispersed, the type of dispersion applied to each, the random seeds being used, and correlation between different variables. This is written out to a file called MonteCarlo_Meta_data_output in the MONTE_* directory.

    ```monte_carlo.master.generate_meta_data = True```

* Changing the name of the automatically-generated monte-directory. By default, this takes the form “MONTE_\<run_name\>” as assigned in the MonteCarloMaster::activate(...) method. The monte_dir variable is public and can be reset after activation and before the `MonteCarloMaster::execute()` method runs. This is particularly useful if it is desired to compare two distribution sets for the same run.

    ```monte_carlo.master.monte_dir = “MONTE_RUN_1_vers2”```

* Changing the input file name. It is expected that most applications of this model will run with a typical organization of a Trick simulation. Consequently, the original input file is probably named input.py, and this is the default setting for the input_file_name variable. However, to support other cases, this variable is public and can be changed at any time between construction and the execution of the `MonteCarloMaster::execute()` method.

    ```monte_carlo.master.input_file_name = “modified_input.py”```

* Padding the filenames of the generated files. By default, the generated RUN directories in the generated MONTE_* directory will have their numerical component padded according to the number of runs. When:
    * between 1 - 10 runs are generated, the directories will be named RUN_0, RUN_1, … 
    * between 11-100 runs are generated, the directories will be named RUN_00, RUN_01, …
    * between 101-1000 runs are generated, the directories will be named RUN_000, RUN_001, …
    * etc.

    Specification of a minimum padding width is supported. For example, it might be desired to create 3 runs with names RUN_00000, RUN_00001, and RUN_00002, in which case the minimum-padding should be specified as 5 characters

    ```monte_carlo.master.minimum_padding = 5```

* Changing the run-name. For convenience, the run-name is provided as an argument in the MonteCarloMaster::activate(...) method. The run_name variable is public, and can be reset after activation and before the `MonteCarloMaster::execute()` method runs. Because this setting determines which run is to be launched from the dispersion files, resetting run_name has limited application – effectively limited to correcting an error, which could typically be more easily corrected directly.

    ```monte_carlo.master.run_name = “RUN_2”```

## 4.3 MonteCarlo Variables (MonteCarloVariable)

The instantiation of the MonteCarloVariable instances is typically handled as a user-input to the simulation without requiring re-compilation. As such, these are usually implemented in Python input files. This is not a requirement, and these instances can be compiled as part of the simulation build. Both cases are presented.

### 4.3.1 Instantiation and Registration

For each variable to be dispersed, an instance of a MonteCarloVariable must be created, and that instance registered with the MonteCarloMaster instance:

1. Identify the type of dispersion desired
2. Select the appropriate type of MonteCarloVariable to provide that dispersion.
3. Create the new instance using its constructor.
4. Register it with the MonteCarloMaster using the `MonteCarloMaster::add_variable( MonteVarloVariable&)` method

#### 4.3.1.1 Python input file implementation for Trick:

When the individual instances are registered with the master, it only records the address of those instances. A user may create completely new variable names for each dispersion, or use a generic name as illustrated in the example below. Because these are typically created within a Python function, it is important to add the thisown=False instruction on each creation to prevent its destruction when the function returns.

```python
mc_var = trick.MonteCarloVariableRandomUniform( "object.x_uniform", 0, 10, 20)
mc_var.thisown = False
monte_carlo.master.add_variable(mc_var)
mc_var = trick.MonteCarloVariableRandomNormal( "object.x_normal", 0, 0, 5)
mc_var.thisown = False
monte_carlo.master.add_variable(mc_var)
```

#### 4.3.1.2 C++ implementation in its own class:

In this case, the instances do have to be uniquely named.

Note that the registering of the variables could be done in the class constructor rather than in an additional method (process_variables), thereby eliminating the need to store the reference to MonteCarloMaster. In this case, the `generate_dispersions` flag is completely redundant because the variables are already registered by the time the input file is executed. Realize, however, that doing so does carry the overhead of registering those variables with the MonteCarloMaster every time the simulation starts up. This can a viable solution when there are only a few MonteCarloVariable instances, but is generally not recommended; using an independent method (process_variables) allows restricting the registering of the variables to be executed only when generating new dispersions.

```c++
class MonteCarloVarSet {
 private:
  MonteCarloMaster & master;
 public:
  MonteCarloVariableRandomUniform x_uniform;
  MonteCarloVariableRandomNormal x_normal;
  ...
  MonteCarloVarSet( MonteCarloMaster & master_)
   :
   master(master_),
   x_uniform("object.x_uniform", 0, 10, 20),
   x_normal ("object.x_normal", 0, 0, 5),
   ...
 { };

 void process_variables() {
   master.add_variable(x_uniform);
   master.add_variable(x_normal);
 ...
 }
};
```

#### 4.3.1.3 C++ implementation within a Trick S-module:

Instantiating the variables into the same S-module as the master is also a viable design pattern. However, this can lead to a very long S-module so is typically only recommended when there are few variables. As with the C++ implementation in a class, the variables can be registered with the master in the constructor rather than in an additional method, with the same caveats presented earlier.

```c++
class MonteCarloSimObject : public Trick::SimObject
{
 public:
  MonteCarloMaster                master;
  MonteCarloVariableRandomUniform x_uniform;
  MonteCarloVariableRandomNormal  x_normal;
  ...
  MonteCarloSimObject(std::string location)
  :
    master(location),
    x_uniform("object.x_uniform", 0, 10, 20),
    x_normal ("object.x_normal", 0, 0, 5),
    ...
{ };
  void process_variables() {
    master.add_variable(x_uniform);
    master.add_variable(x_normal);
    ...
};
  {
    P_MONTECARLO ("initialization") master.execute();
} };
MonteCarloSimObject monte_carlo("monte_carlo.master");
```

### 4.3.2 input-file Access

If using a (compiled) C++ implementation with the registration conducted at construction, the `generate_dispersions` flag is not used in the input file.

```python
if monte_carlo.master.active:
 if monte_carlo.master.generate_dispersions:
 exec(open(“Modified_data/monte_variables.py").read())
```

(where monte_variables.py is the file containing the mc_var = … content described earlier)

```python
if monte_carlo.master.active:
 if monte_carlo.master.generate_dispersions:
 monte_carlo_variables.process_variables()
```

If using a (compiled) C++ implementation with a method to process the registration, that method call must be contained inside the `generate_dispersions` gate in the input file:
```
if monte_carlo.master.active:
 # add only those lines such as logging configuration
```

### 4.3.3 Configuration

For all variable-types, the variable_name is provided as the first argument to the constructor. This variable name must include the full address from the top level of the simulation. After this argument, each variable type differs in its construction arguments and subsequent configuration options.

#### 4.3.3.1 MonteCarloVariable

MonteCarloVariable is an abstract class; its instantiable implementations are presented below. There is one important configuration for general application to these implementations, the setting of units. In a typical simulation, a variable has an inherent unit-type; these are often SI units, but may be based on another system. Those native units may be different to those in which the distribution is described. In this case, assigning the generated numerical value to the variable without heed to the units mismatch would result in significant error.

```set_units(std::string units)```

This method specifies that the numerical value being generated is to be interpreted in the specified units.

Notes
* if it is known that the variable’s native units and the dispersion units match (including the case of a dimensionless value), this method is not needed.
* This method is not applicable to all types of MonteCarloVariable; use with MonteCarloVariableRandomBool and MonteCarloPython* is considered undefined behavior.

#### 4.3.3.2 MonteCarloVariableFile

The construction arguments are:

1. variable name
2. filename containing the data
3. column number containing data for this variable
4. (optional) first column number. This defaults to 1, but some users may want to zero-index their column numbers, in which case it can be set to 0.

There is no additional configuration beyond the constructor

There is no additional configuration beyond the constructor.

#### 4.3.3.3 MonteCarloVariableFixed

The construction arguments are:
1. variable name
2. value to be assigned

Additional configuration for this model includes the specification of the maximum number of lines to skip between runs.
`max_skip`.  This public variable has a default value of 0 – meaning that the next run will be drawn from the next line of data, but this can be adjusted.

#### 4.3.3.4 MonteCarloVariableRandomBool

The construction arguments are:
1. variable name
2. seed for random generator
There is no additional configuration beyond the constructor.

#### 4.3.3.5 MonteCarloVariableRandomNormal

The construction arguments are:
1. variable name
2. seed for random generator, defaults to 0
3. mean of distribution, defaults to 0
4. standard-deviation of distribution, defaults to 1.

The normal distribution may be truncated, and there are several configuration settings associated with truncation. Note that for all of these truncation options, if the lower truncation bound is set to be larger than the upper truncation bound, the generation of the dispersed value will fail and the simulation will terminate without generation of files. If the upper andlower bound are set to be equal, the result will be a forced assignment to that value.

`TruncationType`

This is an enumerated type, supporting the specification of the truncation limits in one of three ways:
* `StandardDeviation`: The distribution will be truncated at the specified number(s) of standard deviations away from the mean.
* `Relative`: The distribution will be truncated at the specified value(s) relative to the mean value.
* `Absolute`: The distribution will be truncated at the specified value(s).

`max_num_tries`

The truncation is performed by repeatedly generating a number from the unbounded distribution until one is found that lies within the truncation limits. This max_num_tries value determines how many attempts may be made before the algorithm concedes. It defaults to 10,000. If a value has not been found within the specified number of tries, an error message is sent and the value is calculated according to the following rules:
* For a distribution truncated at only one end, the truncation limit is used
* For a distribution truncated at both ends, the midpoint value between the two truncation limits is used.

`truncate( double limit, TruncationType)`

This method provides a symmetric truncation, with the numerical value provided by limit being interpreted as a number of standard-deviations either side of the mean, a relative numerical value from the mean, or an absolute value.

The value limit should be positive. If a negative value is provided, it will be negated to a positive value.

The use of TruncationType Absolute and this method requires a brief clarification because this may result in an asymmetric distribution. In this case, the distribution will be truncated to lie between (-limit, limit) which will be asymmteric for all cases in which the mean is non-zero.

`truncate( double min, double max, TruncationType)`

This method provides a more general truncation, with the numerical value provided by min and max being interpreted as a number of standard-deviations away from the mean, a relative numerical value from the mean, or an absolute value.

Unlike the previous method, the numerical arguments (min and max) may be positive or negative, and care must be taken especially when specifying min with TruncationType StandardDeviation or Relative. Realize that a positive value of min will result in a lower bound with value above that of the mean; min does not mean “distance to the left of the mean”, it means the smallest acceptable value relative to the mean.

`truncate_low( double limit, TruncationType)`

This method provides a one-sided truncation. All generated values will be above the limit specification.

`truncate_high( double limit, TruncationType)`

This method provides a one-sided truncation. All generated values will be below the limit specification.

`untruncate()`

This method removes previously configured truncation limits.

#### 4.3.3.6 MonteCarloVariableRandomStringSet

The construction arguments are:
1. variable name
2. seed for random generator

This type of MonteCarloVariable contains a STL-vector of STL-strings containing the possible values that can be assigned by this generator. This vector is NOT populated at construction time and must be configured.

`add_string(std::string new_string)`

This method adds the specified string (`new_string`) to the vector of available strings

#### 4.3.3.7 MonteCarloVariableRandomUniform

The construction arguments are:
1. variable name
2. seed for random generator, defaults to 0
3. lower-bound of distribution, default to 0
4. upper-bound for distribution, defaults to 1

There is no additional configuration beyond the constructor

#### 4.3.3.8 MonteCarloVariableRandomUniformInt

The construction arguments are:
1. variable name
2. seed for random generator, defaults to 0
3. lower-bound of distribution, default to 0
4. upper-bound for distribution, defaults to 1

There is no additional configuration beyond the constructor

#### 4.3.3.9 MonteCarloVariableSemiFixed

The construction arguments are:
1. variable name
2. reference to the MonteCarloVariable whose generated value is to be used as the fixed value.

There is no additional configuration beyond the constructor.

#### 4.3.3.10 MonteCarloPythonLineExec

The construction arguments are:
1. variable name
2. an STL-string providing the Python instruction for the computing of the value to be assigned to the specified variable.

There is no additional configuration beyond the constructor.

#### 4.3.3.11 MonteCarloPythonFileExec
The construction argument is:
1. name of the file to be executed from the generated input file.

There is no additional configuration beyond the constructor.

## 4.4 Information on the Generated Files

This section is for informational purposes only to describe the contents of the automatically-generated dispersion files. Users do not need to take action on any content in here.

The generated files can be broken down into 3 parts:
* Configuration for the input file. These two lines set the flags such that when this file is executed, the content of the original input file will configure the run for a monte-carlo analysis but without re-generating the dispersion files.

```python
monte_carlo.master.active = True
monte_carlo.master.generate_dispersions = False
```

* Execution of the original input file. This line opens the original input file so that when this file is executed, the original input file is also executed automatically.

```python
exec(open('RUN_1/input.py').read())
```

* Assignment to simulation variables. This section always starts with the assignment to the run-number, which is also found in the name of the run, so RUN_0 gets a 0, RUN_1 gets a 1, etc. This value can be used, for example, to generate data sweeps as described in section MonteCarloPythonLineExec above.

```python
monte_carlo.master.monte_run_number = 0
object.test_variable1 = 5
object.test_variable1 = 1.23456789
...
```

## 4.5 Extension

The model is designed to be extensible and while we have tried to cover the most commonly used applications, complete anticipation of all use-case needs is impossible. The most likely candidate for extension is in the area of additional distributions. In this case:
* A new distribution should be defined in its own class
* That class shall inherit from MonteCarloVariable or, if it involves a random generation using a distribution found in the C++ `<random>` library, from MonteCarloVariableRandom.
    * Populate the command variable inherited from MonteCarloVariable. This is the STL string representing the content that the MonteCarloMaster will place into the generated dispersion files.
    * Call the `insert_units()` method inherited from MonteCarloVariable
    * Set the `command_generated` flag to true if the command has been successfully generated.

## 4.6 Running generated runs within an HPC framework

Modern HPC (High Performance Computing) labs typically have one or more tools for managing the execution of jobs across multiple computers.  There are several linux-based scheduling tools, but this section focuses on running the generated runs using a SLURM (Simple Linux Utility for Resource Management) array job.  Consider this script using a simulation built with gcc 4.8 and a user-configured run named `RUN_example` which has already executed once with the Monte-Carlo Generation model enabled to generate 100 runs on disk:

```bash
#SBATCH --array=0-99

# This is an example sbatch script demonstrating running an array job in SLURM.
# SLURM is an HPC (High-Performance-Computing) scheduling tool installed in
# many modern super-compute clusters that manages execution of a massive
# number of user-jobs.  When a script like this is associated with an array
# job, this script is executed once per enumerated value in the array. After
# the Monte Carlo Generation Model executes, the resulting RUNs can be queued
# for SLURM execution using a script like this. Alternatively, sbatch --wrap
# can be used.  See the SLURM documentation for more in-depth information.
#
# Slurm: https://slurm.schedmd.com/documentation.html

# $SLURM_ARRAY_TASK_ID is automatically provided by slurm, and will be an
# integer between 0-99 per the "SBATCH --array" flag specified at the top of
# this script
echo "SLURM has provided us with array job integer: $SLURM_ARRAY_TASK_ID"
# Convert this integer to a zero-padded string matching the RUN naming
# convention associated with thi
RUN_NUM=`printf %02d $SLURM_ARRAY_TASK_ID`
# Execute the single trick simulation run associated with RUN_NUM
echo "Running RUN_$RUN_NUM ..."
./S_main_Linux_4.8_x86_64.exe MONTE_RUN_example/RUN_${RUN_NUM}/monte_input.py
```

The above script can be executed within a SLURM environment by running `sbatch <path/to/script.sh>`. This single command will create 100 independent array jobs in SLURM, allowing the scheduler to execute them as resources permit.  Be extra careful with the zero-padding logic in the script above.  The monte-carlo generation model will create zero-padded `RUN` names suitable for the number of runs requested to be generated by the user. The `%02d` part of the script above specifies 2-digit zero-padding which is suitable for 100 runs. Be sure to match this logic with the zero-padding as appropriate for your use-case.

For more information on SLURM, refer to the project documentation: https://slurm.schedmd.com/documentation.html

# 5 Verification

The verification of the model is provided by tests defined in `test/SIM_mc_generation`.  This sim was originally developed by by JSC/EG NASA in the 2020 timeframe. The verification section of the original documentation is omitted from this markdown file because it heavily leverages formatting that markdown cannot support. It can be viewed [here](MCG_verification_2020.pdf)

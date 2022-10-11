# MonteCarloGeneration Model

## Table of Contents
### [1. Introduction](#Introduction)
### [2. Requirements](#Requirements)
### [3. Model Specification](#Model Specification)
#### &nbsp; &nbsp; [3.1 Code Structure](#Code Structure)
##### &nbsp; &nbsp; &nbsp; &nbsp; [3.1.1 Variable Management (MonteCarloMaster)](#Variable Management)
##### &nbsp; &nbsp; &nbsp; &nbsp; [3.1.2 Disperesd Variables (MonteCarloVariable)](#Disperesd Variables)
#### &nbsp; &nbsp; [3.2 Mathematical Formulation](#Mathematical Formulation)
### [4. User's Guide](#User's Guide)
#### &nbsp; &nbsp; [4.1 What to expect](#What to expect)
##### &nbsp; &nbsp; &nbsp; &nbsp; [4.1.1 Trick Users](#Trick Users)
##### &nbsp; &nbsp; &nbsp; &nbsp; [4.1.2 Disperesd Variables (MonteCarloVariable)](#Disperesd Variables)
#### &nbsp; &nbsp; [4.2 MonteCarlo Manager (MonteCarloMaster)](#MonteCarlo Manager)
##### &nbsp; &nbsp; &nbsp; &nbsp; [4.2.1 Instantiation](#Instantiation)
##### &nbsp; &nbsp; &nbsp; &nbsp; [4.2.2 Configuration](#Configuration)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.2.2.1 Modifications to the regular input file](#Modifications to the regular input file)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.2.2.2 Initiating MonteCarlo](#Initiating MonteCarlo)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.2.2.3 Additional Configurations](#Additional Configurations)
#### &nbsp; &nbsp; [4.3 MonteCarlo Variables (MonteCarloVariable)](#MonteCarlo Variables)
##### &nbsp; &nbsp; &nbsp; &nbsp; [4.3.1 Instantiation and Registration](#Instantiation and Registration)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.3.1.1 Python input file implementation for Trick:](#Python input file implementation for Trick:)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.2.2.3 C++ implementation in its own class:](#C++ implementation in its own class:)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.2.2.3 C++ implementation within a Trick S-module:](#C++ implementation within a Trick S-module:)
##### &nbsp; &nbsp; &nbsp; &nbsp; [4.3.2 Input-file Access](#Input-file Access)
##### &nbsp; &nbsp; &nbsp; &nbsp; [4.3.3 Configuration](#Configuration)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.3.1.1 MonteCarloVariable](#MonteCarloVariable)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.3.1.2 MonteCarloVariableFile](#MonteCarloVariableFile)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.3.1.3 MonteCarloVariableFixed](#MonteCarloVariableFixed)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.3.1.4 MonteCarloVariableRandomBool](#MonteCarloVariableRandomBool)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.3.1.5 MonteCarloVariableRandomNormal](#MonteCarloVariableRandomNormal)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.3.1.6 MonteCarloVariableRandomStringSet](#MonteCarloVariableRandomStringSet)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.3.1.7 MonteCarloVariableRandomUniform](#MonteCarloVariableRandomUniform)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.3.1.8 MonteCarloVariableRandomUniformInt](#MonteCarloVariableRandomUniformInt)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.3.1.9 MonteCarloVariableSemiFixed](#MonteCarloVariableSemiFixed)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.3.1.10 MonteCarloPythonLineExec](#MonteCarloPythonLineExec)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [4.3.1.11 MonteCarloPythonFileExec](#MonteCarloPythonFileExec)
#### &nbsp; &nbsp; [4.4 Information on the Generated Files](#Information on the Generated Files)
#### &nbsp; &nbsp; [4.5 Extension](#Extension)
### [5. Verification](#Verification)
#### &nbsp; &nbsp; [5.1 RUN_nominal](#RUN_nominal)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.1.1 Uniform Distribution](#Uniform Distribution)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.1.2 Normal Distribution](#Normal Distribution)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.1.3 Truncated Normal Distribution](#Truncated Normal Distribution)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [5.3.1.1 Truncated by Standard Deviations from Mean](#Truncated by Standard Deviations from Mean)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [5.3.1.2 Truncated by Difference from Mean](#Truncated by Difference from Mean)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [5.3.1.3 Truncated by Specified Bounds](#Truncated by Specified Bounds)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.1.4 Truncated on Left Only](#Truncated on Left Only)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.1.5 Truncated on Right Only](#Truncated on Right Only)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.1.6 Dispersion in Non-native units](#Dispersion in Non-native units)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.1.7 Discrete Integer (Uniform Distribution)](#Discrete Integer)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.1.8 Discrete String (Uniform Distribution)](#Discrete String)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.1.9 Discrete Boolean (Uniform Distribution)](#Discrete Boolean)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.1.10 Python Code Injection](#Python Code Injection)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [5.1.10.1 Line of Code](#Line of Code)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [5.1.10.2 Execution of a Function](#Execution of a Function)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [5.1.10.3 Execution of File or Script](#Execution of File or Script)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.1.11 Extraction From File](#Extraction From File)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.1.12 Assignment of Fixed Value](#Assignment of Fixed Value)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.1.13 Assignment of Semi-Fixed Value](#Assignment of Semi-Fixed Value)
#### &nbsp; &nbsp; [5.2 Reading Values From a File](#Reading Values From a File)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.2.1 Sequential Lines](#Sequential Lines)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.2.2 Random Lines with Linked Variables](#Random Lines with Linked Variables)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.2.3 Random Lines with Independent Variables](#Random Lines with Independent Variables)
#### &nbsp; &nbsp; [5.3 Distribution Analyses](#Distribution Analyses)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.3.1 Uniform Distribution](#Uniform Distribution)
##### &nbsp; &nbsp; &nbsp; &nbsp; [5.3.2 Normal Distribution](#Normal Distribution)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [5.3.2.0 Any normal distribution may be truncated. As we saw in section 5.1.3, a normal distribution can be truncated according to one of 3 methods for specifying the range:](#truncation)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [5.3.2.1 Truncated by Prescribed Range](#Truncated by Prescribed Range)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [5.3.2.2 Truncated by Difference from Mean](#Truncated by Difference from Mean)
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [5.3.2.3 Truncated by Standard Deviations from Mean](#Truncated by Standard Deviations from Mean)

## Revision History
| Version | Date | Author | Purpose |
| :--- |:---| :--- | :--- |
| 1 | April 2020 | Gary Turner | Initial Version |
| 2 | March 2021 | Gary Turner | Added Verification |
| 3 | October 2022 | Isaac Reaves | Converted to Markdown |

## 1 Introduction
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

## 2 Requirements
### 1. The model shall provide common statistical distribution capabilities, including:
#### &nbsp; &nbsp; (a). Uniform distribution between specified values  
##### &nbsp; &nbsp; &nbsp; &nbsp; i. as a floating-point value  
##### &nbsp; &nbsp; &nbsp; &nbsp; ii. as an integer value  
#### &nbsp; &nbsp; (b) Normal distribution, specified by mean and standard deviation  
#### &nbsp; &nbsp; (c) Truncated Normal Distribution, including  
##### &nbsp; &nbsp; &nbsp; &nbsp; i. symmetric and asymmetric truncations  
##### &nbsp; &nbsp; &nbsp; &nbsp; ii. it shall be possible to specify truncations by:  
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; A. some number of standard deviations from the mean,  
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; B. a numerical difference from the mean, and  
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; C. an upper and lower limit  
### 2. The model shall provide an extensible framework suitable for supporting other statistical distributions
### 3. The model shall provide the ability to assign a common value to all runs:
#### &nbsp; &nbsp; (a) This value could be a fixed, user-defined value
#### &nbsp; &nbsp; (b) This value could be a random assignment, generated once and then applied to all runs
### 4. The model shall provide the capability to read values from a pre-generated file instead of generating its own values
### 5. The model shall provide the ability to randomly select from a discrete data set, including:
#### &nbsp; &nbsp; (a) enumerations,
#### &nbsp; &nbsp; (b) character-strings,
#### &nbsp; &nbsp; (c) boolean values, and
#### &nbsp; &nbsp; (d) numerical values
### 6. The model shall provide the capability to compute follow-on variables, the values of which are a function of one or more dispersed variables with values generated using any of the methods in requirements 1-5.
### 7. The model shall provide a record of the generated distributions, allowing for repeated execution of the same scenario using exactly the same conditions.
### 8. The model shall provide summary data of the dispersions which have been applied, including:
#### &nbsp; &nbsp; (a) number of dispersions
#### &nbsp; &nbsp; (b) types of dispersions
#### &nbsp; &nbsp; (c) correlations between variables

## 3 Model Specification
### 3.1 Code Structure
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
MonteCarloVariableRandom types use the C++ <random> library for data generation. Limiting the
MonteCarloVariableSemiFixed type to be seeded only by something using the <random> library violates the concept of
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

### 3.2 Mathematical Formulation
No mathematical formulation. The random number generators use the C++ <random> library.

## 4 User's Guide

### 4.1 What to expect

This role played by this model can be easily misunderstood, so let’s start there.
This model generates Python files containing assignments to variables.

That’s it!! It does not manage MonteCarlo runs. It does not execute any simulations. When it runs, it creates the requested
number of Python files and exits.

This design is deliberate; we want the model to generate the instruction sets that will allow execution of a set of dispersed
configurations. At that point, the simulation should cease, returning control to the user to distribute the execution of those
configurations according to whatever distribution mechanism they desire. This could be:

* something really simple, like a wild-card, <executive> MONTE_RUN_test/RUN*/monte_input.py
* a batch-script,
* a set of batch-scripts launching subsets onto different machines,
* a load-management service, like SLURM
* any other mechanism tailored to the user’s currently available computing resources

The intention is that the model runs very early in the simulation sequence. If the model is inactive (as when running a regular, non-MonteCarlo run), it will take no action. But when this model is activated, the user should expect the simulation to terminate before it starts on any propagation.

When a simulation executes with this model active, the only result of the simulation will be the generation of files containing the assignments to the dispersed variables. The simulation should be expected to terminate at t=0.

#### 4.1.1 Trick Users

The model is currently configured for users of the Trick simulation engine. The functionality of the model is almost exclusively independent of the chosen simulation engine, with the exceptions being the shutdown sequence, and the application of units information in the variables.

Found at the end of the MonteCarloMaster::execute() method, the following code:

```
exec_terminate_with_return(0, __FILE__, __LINE__,message.c_str());
```

is a Trick instruction set to end the simulation.

Found at the end of MonteCarloVariable::insert_units(), the following code:

``` 
// TODO: Pick a unit-conversion mechanism
// Right now, the only one available is Trick:
trick_units( pos_equ+1);
```

provides the call to

```
MonteCarloVariable::trick_units(
 size_t insertion_pt)
{
 command.insert(insertion_pt, " trick.attach_units(\"" + units + "\",");
 command.append(")");
```

which appends Trick instructions to interpret the generated value as being represented in the specified units.

The rest of the User’s Guide will use examples of configurations for Trick-simulation input files

### 4.1.2 Non-Trick Users

To configure the model for simulation engines other than Trick, the Trick-specific content identified above should be replaced with equivalent content that will result in:
* the shutdown of the simulation, and
* the conversion of units from the type specified in the distribution specification to the type native to the variable to which the generated value is to be assigned.

While the rest of the User’s Guide will use examples of configurations for Trick-simulation input files, understand that these are mostly just C++ or Python code setting the values in this model to make it work as desired. Similar assignments would be required for any other simulation engine.

### 4.2 MonteCarlo Manager (MonteCarloMaster)
### 4.2.1 Instantiation

The instantiation of MonteCarloMaster would typically be done directly in the S_module. The construction of this instance takes a single argument, a STL-string describing its own location within the simulation data-structure.

The MonteCarloMaster class has a single public-interface method call, MonteCarloMaster::execute(). This method has 2 gate-keeping flags that must be set (the reason for there being 2 will be explained later):
* active
* generate_dispersions

If either of these flags is false (for reference, active is constructed as false and generate_dispersions is constructed as true) then this method returns with no action. If both are true, then the model will generate the dispersions, write those dispersions to the external files, and shut down the simulation.

An example S-module

```
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

### 4.2.2.1 Modifications to the regular input file

A regular input file sets up a particular scenario for a nominal run. To add monte-carlo capabilities to this input file, the
following code should be inserted somewhere in the file:

```
if monte_carlo.master.active:
 # insert non-mc-variable MC configurations like logging
 if monte_carlo.master.generate_dispersions:
 exec(open(“Modified_data/monte_variables.py").read())
```

Let’s break this down, because it explains the reason for having 2 flags:

| Generate_dispersions | Active | Result |
| :--- |:---| :--- |
| false | false | Regular (non-monte-carlo) run |
| false | true | Run scenario with monte-carlo configuration and pregenerated dispersions |
| true | false | Regular (non-monte-carlo) runs |
| true | true | Generate dispersions for this scenario, but do not run the scenario |

 1. If the master is inactive, this content is passed over and the input file runs just as it would without this content
 2. Having the master active flag set to true instructs the simulation that the execution is intended to be part of a monte-carlo analysis. Now there are 2 types of executions that fit this intent:
    * The generation of the dispersion files
    * The execution of this run with the application of previously-generated dispersions

Any code to be executed for case (a) must go inside the generate_dispersions gate. Any code to be executed for
case (b) goes inside the active gate, but outside the generate_dispersions gate.

You may wonder why this distinction is made. In many cases, it is desirable to make the execution for monte-carlo
analysis subtly different to that for regular analysis. One commonly used distinction is logging of data; the logging
requirement may differ between a regular run and one as part of a monte-carlo analysis (typically, monte-carlo runs
execute with reduced logging). By providing a configuration area for a monte-carlo run, we can support these
distinctions.
Note – any code to be executed for only non-monte-carlo runs can be put in an else: block. For example, this code
will set up one set of logging for a monte-carlo run, and another for a non-monte-carlo run of the same scenario:
```
if monte_carlo.master.active:
 exec(open(“Log_data/log_for_monte_carlo.py”).read())
 if monte_carlo.master.generate_dispersions:
 exec(open(“Modified_data/monte_variables.py").read())
else:
 exec(open(“Log_data/log_for_regular.py”).read())
```
 3. If the generate_dispersions flag is also set to true, the MonteCarloMaster::execute() method will execute,
generating the dispersion files and shutting down the simulation. 

### 4.2.2.2 Initiating MonteCarlo

Somewhere outside this file, the active and generate_dispersion flags must be set. This can be performed either in a separate input file or via a command-line argument. Unless the command-line argument capability is already supported, by far the easiest mechanism is to create a new input file that subsequently reads the existing input file:

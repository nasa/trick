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
###### &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [Any normal distribution may be truncated. As we saw in section 5.1.3, a normal distribution can be truncated according to one of 3 methods for specifying the range:](#truncation)
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
1. The model shall provide common statistical distribution capabilities, including:

   (a). Uniform distribution between specified values  
      i. as a floating-point value  
      ii. as an integer value  
   (b) Normal distribution, specified by mean and standard deviation  
   (c) Truncated Normal Distribution, including  
      i. symmetric and asymmetric truncations  
      ii. it shall be possible to specify truncations by:  
         A. some number of standard deviations from the mean,  
         B. a numerical difference from the mean, and  
         C. an upper and lower limit  

 2. The model shall provide an extensible framework suitable for supporting other statistical distributions
 3. The model shall provide the ability to assign a common value to all runs:
(a) This value could be a fixed, user-defined value
(b) This value could be a random assignment, generated once and then applied to all runs
 4. The model shall provide the capability to read values from a pre-generated file instead of generating its own values
 5. The model shall provide the ability to randomly select from a discrete data set, including:
(a) enumerations,
(b) character-strings,
(c) boolean values, and
(d) numerical values
 6. The model shall provide the capability to compute follow-on variables, the values of which are a function of one or
more dispersed variables with values generated using any of the methods in requirements 1-5.
 7. The model shall provide a record of the generated distributions, allowing for repeated execution of the same
scenario using exactly the same conditions.
 8. The model shall provide summary data of the dispersions which have been applied, including:
(a) number of dispersions
(b) types of dispersions
(c) correlations between variables

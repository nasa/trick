# Interface Code Generator Design

The Interface Code Generator (ICG) creates class/struct layout information, called
attributes, used to convert string variable names to simulation memory
addresses.  The attributes are used with several of Trick's services
including checkpointing, data recording, and the variable server.

ICG execution is divided into 2 parts.

1. Executing the Clang C++ frontend to parse all header files and extract
   class/struct information.
1. Print class/struct information as attributes in input/output source code.

## Clang C++ Frontend

ICG uses the Clang frontend.  Clang is a C/C++ frontend to the LLVM compiler.
Clang and LLVM provide libraries that allow developers to create their own tools
using the compiler.  ICG calls the clang parser to process the header
files and converts the source text into a tree form called an abstract syntax tree
(AST).  Clang completely handles all textual parsing.  ICG provides classes to
provide input parameters to the compiler and to extract the information out of the
AST.

### Parser Setup

The frontend has some parameters that must be set in order for the parser to
execute.

#### Parsing Language

The S_source.hh file where parsing begins is written in C++.  The frontend is set to
parse C++.

#### Target Architecture

The frontend requires a target architecture set.  ICG does not pass the AST to the
next stage of compiler tools after the frontend, so it doesn't matter what the
target architecture is set to.  Setting the target to the the default architecture
works.

### Preprocessor

The frontend includes a preprocessor.  The preprocessor handles include and define
statements.  ICG provides a pair of classes that interact with the clang preprocesor.
The HeaderSearchDirs class to provide the preprocessor with include search
directories and default defines.  The CommentSaver class saves all comments parsed
by the preprocessor.  These classes are used throughout the rest of ICG execution.

#### HeaderSearchDirs

The HeaderSearchDirs manages the header search directories used by the preprocessor
and provides an interface for querying the inclusion status of a directory.  Since
this class is closely related to the preprocessor, it also handles define macros.

The HeaderSearchDirs class provides a list of include search directories to the
clang preprocessor.  There are two types of include directories, system include
directories and user provided directories.  Both types must be supplied to the
preprocessor.

The system include directories are retrieved by running the TRICK_CPPC compiler
in verbose preprocessor mode with no input code.  When the compiler is run
in this mode, it prints the system include directories built into the compiler.
This class parses that output for the system include directories and feeds the
list into the preprocessor.

The user include directories are retrieved from the command line when executing
ICG.  Typically ICG is called with the TRICK_CXXFLAGS environment variable
included as an argument.  TRICK_CXXFLAGS may include -I<include_dir> strings.
These include directories are parsed from the command line and fed to the
preprocessor.

This class parses the TRICK_ICG_EXCLUDE environment variable and manages a list
of directories ICG should not write any output source code.

The HeaderSearchDirs class provides boolean responses to questions to whether a
path is in a user directory or is in an TRICK_ICG_EXCLUDE directory.

#### CommentSaver

The CommentSaver class saves comments found in user code and provides an iterface
for retrieving comments based on file name/line number.  The class also provides
an interface to retrieving several fields from a Trick header comment including
the ICG and ICG_EXCLUDE_TYPES fields.

The clang preprocessor includes a comment handler interface.  ICG uses
this interface to attach the CommentSaver class.  Each comment handled by this
class is stored in a map indexed by file name and line number.

### Abstract Syntax Tree (AST) Traversal

Traversing the resulting AST is bulk of ICG's work.  The clang compiler provides
an interface allowing ICG to attach handlers to process interesting events during
code parsing.  In this case, ICG only attaches a handler when clang is finished
parsing the entire input file, called HandleTranslationUnit.

When HandleTranslationUnit is start parsing the AST searching for top level
declarations we want to process.  From the top level recursively calls
tree parsers that specialize in extracting information about typedefs,
classes and structs, fields within each class/struct, and enumerations.

The following diagram shows the recursive class instantiation sequence as well
as the data returned to the caller.

## Writing Class/Struct Attributes




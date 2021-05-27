This section details the syntax for creating headers and source code that Trick can process.

It also details the operation of the Trick Interface Code Generator (ICG) that processes headers, and the Module Interface Specification Processor (MIS) that processes source code.

#### Programming Language Support

The majority of model source for simulations is written in C and C++ . Trick supports auto generating IO code to peek and poke C and C++ structures, classes, and enumerations. Trick also generates the necessary makefile rules to compile and link C and C++ model code into the simulation.

Models written in other languages may be included in the simulation. It is possible to include Fortran 77, Fortran 90, Ada, and/or Java code in the simulation. These models cannot be called directly from the Trick scheduler, but may be called through C language wrapper functions provided by the user that executes the other language calls.

#### C Header Files

Trick processes header files in order to auto generate IO source code for the simulation. IO source code is the heart of how Trick does its input processing. The following describes the syntax for a header file.

```C++
/* [TRICK_HEADER]
PURPOSE:
    (Purpose statement.)
[LANGUAGE: (C++)]
[LIBRARY DEPENDENCY:
    (
     (object.o|model.c|lib.a|lib.so|<relative_path>/lib.a)
     [(object_n.o|model_n.c|lib_n.a|lib_n.so|<relative_path>/lib_n.a)]
    )]
[ICG IGNORE TYPES:
    ((Type #1) (Type #n)])]
[PYTHON_MODULE: (module_name)]
[REFERENCES:
    ((Reference #1) (Reference #n)])]
[ASSUMPTIONS AND LIMITATIONS:
    ((Assumption #1) (Assumption #n)])]
[PROGRAMMERS:
   (((Name) (Company) (Date) [(other info)])
   [((Name) (Company) (Date) [(other info)])]]
[ICG: (No|Nocomment)]

Introduced in Trick 16
@trick_parse(everything|attributes|dependencies_only)
@trick_exclude_typename(Type)

*/

typedef enum {
    enum_label [=enum_value],
    last_enum_label [= enum_value]
} enum_name ;

[typedef] struct [struct_tag] {
    char|short|int|long|long long|
    unsigned char|unsigned short|unsigned int|unsigned long|unsigned long long|
    float|double [*]* param_name [[dim]]* ;
        /* [**|*i|*o|*io] (trick_io|io)([**|*i|*o|*io]) (trick_chkpnt_io|cio)([**|*i|*o|*io])
           trick_units([measurement_units]) description */

    any_other_type [*]* param_name [[dim]]* ;
        /* [**|*i|*o|*io] trick_io([**|*i|*o|*io]) trick_chkpnt_io([**|*i|*o|*io])
           trick_units(measurement_units) description */
} struct_name ;

class <class_name> {
    [
     friend InputProcessor ;
     friend init_attr<class_name>() ;
    ]

    (public|protected|private):
    char|short|int|long|long long|
    unsigned char|unsigned short|unsigned int|unsigned long|unsigned long long|
    float|double [*]* param_name [[dim]]* ;
        /* [**|*i|*o|*io] trick_io([**|*i|*o|*io]) trick_chkpnt_io([**|*i|*o|*io])
           trick_units([measurement_units]) description */

    any_other_type [*]* param_name [[dim]]* ;
        /* [**|*i|*o|*io] trick_io([**|*i|*o|*io]) trick_chkpnt_io([**|*i|*o|*io])
           trick_units([measurement_units])measurement_units description */
} ;
```
##### Comment Header

The Trick comment header, which is optional, begins with `/* PURPOSE:`. Within the Trick comment header, the `PROGRAMMERS`, `REFERENCES`, `ASSUMPTIONS AND LIMITATIONS` and `ICG` are optional entries. Since parentheses, (), are used to delineate fields within the comment header, parentheses are not allowed as characters within the comment fields. Any other formatted comments may appear before and/or after the Trick comment header.

###### C++ Language Override, `LANGUAGE: (C++)`

If a header file has a C++ extension (e.g *.hh ) Trick’s parsers will realize that it is a C++ file and handle it appropriately. If the extension is *.h, Trick will assume it is a C file (not C++). If you want to make a C++ header file name with the *.h extension, you must explicitly tell Trick it is a C++ file with the `LANGUAGE: (C++)` declaration in the Trick comment header.

###### Telling ICG to ignore this header file, `ICG: (No)`

If `ICG: (No)` is in the comment header, Trick will not to process the header. This is useful if the header contains anything that Trick cannot process, or if the programmer wishes Trick to skip this header. For skipping entire sets of headers, see next item.

If `ICG: (Nocomment)` is in the comment header, Trick will not process any comments within the file. This option is useful if the user wants ICG to process the file but the file does not have comments that are Trick compliant.

###### Library Dependencies

```
LIBRARY DEPENDENCY:
    ((relative_path/model_1.c)
     (relative_path/model_2.cpp))
```

Library dependencies list out model source code files required by the simulation. There are several locations to add library dependencies, one of which is in model source headers. The format of dependencies in the S_define file is a relative path to the model source file. The path is relative to -I include paths found in TRICK_CFLAGS and TRICK_CXXFLAGS.

For example if the full path to our model is /this/is/a/full/path/to/model.c and in our TRICK_CFLAGS we have -I/this/is/a/full as one of the included search paths, the library dependency must complete the full path to the file, in this case path/to/model.c. Library dependencies in the S_define file differ from ones found in model source code as they must be the full path to the source file not the object file.

This is the preferred library dependency syntax. There are other forms of library dependencies that are still valid and listed below.

The LIBRARY DEPENDENCY field may contain the object code files which the current file depends on. A self-reference is not necessary.

For example, for a file this.c which calls

* a function within the file that.c
* a function in a user object library my_library/libdog.a
* a function foo.c

The `LIBRARY DEPENDENCY` field might look like this:

```
LIBRARY DEPENDENCY:
    ((this.o)
     (that.o)
     (my_library/libdog.a)
     (libcow.so)
     (${FOO_ENV_VAR}/foo.o))
```

For references to objects outside the current source directory, the directory paths must be specified relative to the bin_${TRICK_HOST_CPU} directory. In this example, the that.c function might also have its own library dependency list, but the that.c dependencies do not need to appear in the this.c dependency list. The CP will automatically search and sort all the object code dependencies; the developer just has to make sure all dependencies are listed in the appropriate files.

There are two ways to specify dependencies to actual libraries, i.e. lib\*.a files:

`* <relative path>="">/<libraryname>.a`

If you use a relative path to the library, Trick will search the TRICK_CFLAGS for a directory that contains source code for the library. Once Trick locates the source code, it will automatically build the library and link it in the simulation.

`* <libraryname>.a`

If you do NOT specify a relative path, Trick will NOT build the library for you. It will simply search your -L paths in your `TRICK_USER_LINK_LIBS` for the library. If found, it will link the library into the simulation.

You may also have Trick link in a shared (`lib*.so`) library. You must supply the `*.so` extension. Trick will not automatically build a shared library, but it is smart enough to use it during link time.

The LIBRARY DEPENDENCY field also handles the `#ifdef`, `#else` and `#endif` statements such that different object files and libraries may be linked for different cases. The previous example might look like this:

```
LIBRARY DEPENDENCY:
    ((this.o)
     (that.o)
#ifdef __animals
     (my_library/libdog.a)
     (libcow.so)
#else
     (my_library/lib.a)
#endif
     (${FOO_ENV_VAR}/foo.o))
```

Best practice is to add library dependencies for source code files for prototypes listed in the header.

###### `ICG_IGNORE_TYPES`

The `ICG IGNORE TYPES` field lists the structs or classes to be ignored. Any parameters of this type or inherited from are ignored. The `ICG IGNORE TYPES` field is only valid for the current file. It does not extend to included header files.

###### `PYTHON_MODULE`

Specifying a `python_module` name will place any class/struct and function definitions in this header file in a python module of the same name. All classes and functions are flattened into the python `trick` namespace by default. This capability allows users to avoid possible name collisions between names when they are flattened.

##### Compiler Directives

Trick handles all compiler directives (`#if`, `#ifdef`, `#endif`, `#define`, `#include`, etc.) ICG also uses the -D and -U command line arguments for defines and undefines, respectively.

##### trick_parse

The trick_parse directive is a Doxygen style field serving the same functionality as the `PURPOSE:` keyword and `ICG: (No)`.  The trick_parse directive like all Doxygen style directives are prefixed with either a `\` or an `@` character.

`@trick_parse(everything)`:  Treat this comment as the Trick header comment.  Search for library dependencies in this comment, and treat all comments following variable definitions as Trick comments.

`@trick_parse(attributes)`:  Treat this comment as the Trick header comment.  Search for library dependencies in this comment.  Create I/O attributes for the classes and structures in the file, but do not read comments following variable definitions.

`@trick_parse(dependencies_only)`:  Treat this comment as the Trick header comment.  Search for library dependencies in this comment.  Do not process the file any further.

##### `trick_exclude_typename`

`@trick_exclude_typename(type)` is equivalent to `ICG_IGNORE_TYPES` in a Doxygen style field. The `trick_exclude` field lists the structs or classes to be ignored.  Multiple `trick_exclude_typename` fields may be used to ignore multiple types.
  
##### Enumerated Type Definitions

Trick provides complete support for enumerated types. Simple mathematical expressions using enumerated types are supported as well.

An example follows:

a.h

```C
typedef enum {
   FIRST_ENUM = 45
} A_ENUM ;
```

b.h

```C
#include "a.h"

typedef enum {
  ME_TOO = 2
} OTHER_ENUM;

typedef enum {
  SECOND_ENUM = FIRST_ENUM,
  THIRD_ENUM = FIRST_ENUM * 3,
  FOURTH_ENUM = SECOND_ENUM * 4,
  FIFTH_ENUM = ME_TOO * 6
} B_ENUM ;
```

c.h

```C
#include "b.h"

typedef struct {
  int dummy;                      /* No comment necessary */
  A_ENUM ae;                      /* No comment necessary */
  B_ENUM be;                      /* No comment necessary */
  int ia1[FIRST_ENUM];            /* No comment necessary */
  int ia2[SECOND_ENUM];           /* No comment necessary */
  int ia3[FIFTH_ENUM];            /* No comment necessary */
} DATA ;
```

##### Data Structure Definitions and Parameter Declarations

The data structure type definition statements, `typedef struct { ... } name;`, and `typedef union { ... } name;` `struct Foo { } name;` follows standard C syntax, and are supported by Trick. However, Trick requires a C comment immediately following every parameter declaration.

##### Parameter Data Types

Trick allows any data type declaration within the data structure `typedef` statement. However, only the following data types will be processed by Trick:

1 `int`,
1 `short`,
1 `long`,
1 `long long`
1 `char`,
1 `(un)signed int`,
1 `(un)signed short`,
1 `(un)signed long`,
1 `(un)signed char`,
1 `(un)signed long long`,
1 `(un)signed short int`,
1 `(un)signed long int`,
1 `float`,
1 `double`,
1 `wchar_t`,
1 `FILE *`
1 Bit fields (signed and unsigned) and
1 previously processed structure, union, enumerated types and typedefs.

All other types are ignored. Types may be defined and used within the same header if the types are defined before they are used (this is a C syntax rule, too).

##### Pointers

Any combination of pointers and array dimensions up to 8 dimensions may be used for parameter declarations; for example, `double ** four_dimensional_array[2][2];`, will be processed. Void pointers and function pointers are not processed. Parameters declared with pointers (like `four_dimensional_array` example), are treated differently; these are called unconstrained arrays. Trick will generate dynamic memory allocation source code for the developer which allows the developer to size the array dimensions (represented by the pointers) via special syntax in the runstream input file. The developer may 1) use the input file to input data to the arrays, 2) output the data via standard Trick logging functions, or 3) share the data through the variable server.

The user does have the option to perform their own memory management for parameters declared as pointers. In this case, instead of specifying the allocation in the input file, the user may allocate the data in a job. In order for Trick to process the data as if it was its own managed memory (and provide capabilities like logging, checkpointing, etc.), the memory address, and number and size of the allocation must be passed to the Trick `TMM_declare_extern_var` function. The user is also responsible for freeing the memory when done.

##### Intrinsic typedef and struct Support

Types declared using `typedef struct`, `typedef union`, and `typedef enum` are recognized by Trick. Intrinsic typedefs are supported as well and may be nested in structures. The example that follows details a header that Trick will handle:

```C
typedef unsigned char my_uchar;
typedef char my_char;
typedef wchar_t my_wchar;
typedef short int my_shortint;
typedef short my_short;
typedef unsigned short int my_ushortint;
typedef unsigned short my_ushort;
typedef int my_int;
typedef unsigned int my_uint;
typedef long int my_longint;
typedef long my_long;
typedef unsigned long int my_ulongint;
typedef unsigned long my_ulong;
typedef float my_float;
typedef double my_double;
typedef my_short my_short2;

struct Animal_Sound {
   int moo ;            /* -- Cow */
   int baa ;            /* -- Lamb */
   int sss ;            /* -- Snake */
};

typedef struct {
  my_uchar uc ;         /* -- unsigned char */
  my_char  c ;          /* -- char */
  my_char  ca[80] ;     /* -- char */
  my_wchar  wc;          /* -- wchar_t */
  my wchar wca[100];   /* -- wchar_t */
  my_shortint si ;      /* -- short int */
  my_short *s ;         /* -- short stuff */
  my_ushortint usi ;    /* -- short stuff */
  my_ushort us ;        /* -- short stuff */
  my_int i ;            /* -- count */
  my_int ia[5] ;        /* -- count */
  my_uint ui ;          /* -- count */
  my_longint li ;       /* -- count */
  my_long l ;           /* -- count */
  my_ulongint uli ;     /* -- count */
  my_ulong ul ;         /* -- count */
  my_float f ;          /* -- count */
  my_double d ;         /* -- count */
  my_short2 s20;        /* -- short 20 */
  my_short2 s21;        /* -- short 21 */
  struct Animal_Sound as /* -- Wild Kingdom */
} DATA ;

typedef DATA MY_DATA;
typedef MY_DATA MY_DATA_2;

typedef struct {
   DATA id;        /* -- testing typedef of struct */
   MY_DATA mid;    /* -- testing typedef of struct */
   MY_DATA_2 mid2; /* -- testing typedef of struct */
} DATA_2 ;
```

##### Parameter Comments

Each parameter declaration within a data structure definition may be accompanied by a trailing comment. There are six possible fields in the parameter comment, but only two are required. All six fields of the parameter comment are stored for later reuse at simulation runtime.

###### The Input/Output Specification

The first three fields in the parameter comment are optional and specify the input/output processing for the parameter. I/O permissions may be set globally or individual capabilities may set their permissions separately. I/O permissions for checkpointing is available to set separately.

To set all permissions for general variable access, start the comment with one of the following fields, `[**|*i|*o|*io]`, `trick_io([**|*i|*o|*io])` or `io([**|*i|*o|*io])`. These are equivalent forms to set general variable access.

* `**` indicates that Trick will not allow input or output for this parameter; i.e. the user can not input this parameter, record this parameter, or view its value.
* `*i` indicates that only input is allowed for the parameter. Parameter may be input through the checkpoint file or ref_assignment, but the parameter will not be recordable or written to a checkpoint file.
* `*o` indicates only output is allowed for the parameter. Parameter may be checkpointed or logged only. They are not reloaded during a checkpoint reload.
* `*io` specifies that both input and output are allowed for the parameter. This is the default condition if the field is omitted from the comment. Parameter may be in input file, may be checkpointed and logged.

Checkpoint I/O may be set separately by adding `trick_chkpnt_io([**|*i|*o|*io])` or `cio([**|*i|*o|*io])` to the comment. If this optional field is not present, the general I/O access field is used to determine checkpoint permissions.

* `**` indicates that Trick will not allow checkpoint input or output. General variable access may still be available.
* `*i` indicates only checkpoint input is allowed for the parameter. Parameters will not be written to the checkpoint.
* `*o` indicates only checkpoint output is allowed for the parameter. Parameter is written to the checkpoint, but not reloaded.
* `*io` specifies that both input and output are allowed for the checkpointing.

###### The Measurement Units Specification
The second field, `trick_units([measurement_units])`, is a required field and specifies the internal source code units for the parameter. These units are important because they give the input processor the knowledge of what units the user's input data needs to be converted to. Trick uses a third-party package, UDUNITS, for units support. It's syntax is specified [here](https://www.unidata.ucar.edu/software/udunits/udunits-current/doc/udunits/udunits2lib.html#Syntax).

###### User Defined Attributes Fields

Following the measurement units specification, in the parameter comment, are two optional, user-defined attribute fields. Using these fields, a user can associate (up to 2) character strings with a parameter. These strings are stored in the ATTRIBUTES structures (in the io_src directory) generated by ICG. The first of these optional fields is delimited by brackets (‘[‘ and ‘]’) and is stored in the element ATTRIBUTES->alias. The second is delimited by braces (‘{‘ and ‘}’) and is stored in the element ATTRIBUTES->user_defined. The definition of the ATTRIBUTES structure is found in $TRICK_HOME/trick_source/sim_services/include/attributes.h.

###### Description Fields

The description field is required and must be the last field of the comment. The description field is basically everything after the first three fields. The description field may span multiple lines.

##### C++ Header Files

C++ headers may include constructs and concepts not found in C header files. In addition to all C syntax, Trick parses and understands many C++ features.

##### Public, Protected, and Private Access

Trick generates several files to support its various features. The data recorder and checkpointer rely on code produced by the Interface Code Generator (ICG), which bookkeeps the memory layout of variables within the simulation. `public` members are always available to these features. `protected` and `private` data is also available if there is no use of `TRICK_ICG` in the header file. If use is found, Trick will issue a warning during simulation compilation, and `private` and `protected` data will only be accessible to Trick if the following `friend`s are added to the offending classes:

```C++
friend class InputProcessor ;
friend void init_attr<class_name>() ;
```

The input processor and variable server rely on code produced by a third-party tool, the [Simplified Wrapper and Interface Generator (SWIG)](http://www.swig.org/). SWIG provides the functions that allow access to simulation variables from Python contexts. These features can only access `public` members. It is not possible to expose `protected` and `private` data to them.

##### Inheritance

Trick may use model code with any type of inheritance. Some limitations are present to Trick's ability to input process, checkpoint, etc. inherited variables.

* Public and protected inherited variables are available for access.
* Protected and private inheritance is ignored.
* Multiple inheritance is processed but not well tested.
* Template inheritance is not currently supported.

##### Namespaces

Currently one level of namespace is supported. Additional levels of namespaces are ignored. Similarly classes and enumerations embedded in other classes are ignored.

```C++
namespace my_ns {
// BB is processed
class BB {
public:
std::string str ;
// Class CC is ignored.
class CC {
...
}
} ;
// Everything enclosed in inner_ns is ignored.
namespace inner_ns {
...
} ;
} ;
```

##### Function Overloading

Trick parses function declarations for input file use. The python input processor understands class method overloading. Overloaded methods with different arguments may be called in the input files. Default arguments are to methods are understood and honored in the input file. Operator overloading is skipped by Trick processors. Operator overloading is not implemented in the input file.

##### Templates and the Standard Template Libraries (STL)

Trick attempts to process user defined templates. Simple templates are handled. We do not have a good definition of simple. Typedefs of templates is supported and encouraged. All protected and private data is ignored within templates. This is because it is not possible to specify the correct io_src friend function. Templates within templates are not processed. Finally abstract templates are not supported by Trick. These templates should be excluded from Trick processing. See below to see how to exclude code from processing.

STLs may be used in models. However, STL variables are not data recordable, they are not visible in the variable server, nor are they directly accessible in the input file. STL variables may be checkpointed with user help. Trick provides function templates to checkpoint and restore STLs. The user needs to call 3 functions, a checkpoint, post_checkpoint, and restart class jobs.

```C++
// A class with STLs to be checkpointed. 3 methods are defined to help Trick checkpoint the STLs
class STLCheckpoint {
public:
std::map< int , double > my_double_map ;
std::vector < double > my_double_vec ;
int checkpoint(string object_name);
int post_checkpoint(string object_name);
int restart(string object_name);
} ;
// The checkpoint job converts STLs to array data.
int STLCheckpoint::checkpoint(string object_name) {
/* checkpoint_stl is a templated function that takes an STL,
a sim_object name, and a variable name (usually the same
as the STL name) as arguments. It outputs memory_manager
arrays named object_name.<variable_name> that contain the
data in the STLs.
*/
checkpoint_stl(my_double_map, object_name , “my_double_map”) ;
checkpoint_stl(my_double_vec , object_name ,“my_double_vec”) ;
}
// The post_checkpoint job frees memory allocated in checkpoint job
int STLCheckpoint::post_checkpoint(string object_name) {
//delete_stl takes the same arguments as checkpoint_stl
checkpoint_stl(my_double_map, object_name , “my_double_map”) ;
checkpoint_stl(my_double_vec , object_name ,“my_double_vec”) ;
}
// The restart job restores STLs from a checkpoint file.
int STLCheckpoint::restart(string object_name) {
//restore_stl takes the same arguments as checkpoint_stl
restore_stl(my_double_map, object_name , “my_double_map”) ;
resotre_stl(my_double_vec , object_name ,“my_double_vec”) ;
}
```

Calls to checkpoint the STLs in the S_define.

```C++
class theSimObject : public Trick::SimObject {
public:
STLCheckpoint stls ;
theSimObject() {
/*
"name" is the string that is the sim_object
instance name. It is present in all sim objects
and automatically set by Trick.
*/
("checkpoint") stls.checkpoint(name) ;
("post_checkpoint") stls.post_checkpoint(name) ;
("restart") stls.restart(name) ;
} ;
```

##### Noncopyable Objects

Sometimes classes contain members that are not copyable or the math modeler wants to prevent the class from being copied. Declaring an unimplemented private copy constructor and assignment, "=", operator prevents the class from being copied.
```C++
class CantCopyMe {
private:
CantCopyMe( const CantCopyMe & ) ;
CantCopyMe & operator = ( const CantCopyMe ) ;
}
```

When using such classes in Trick, classes that include non copyable classes must also declare themselves not copyable. this extends all the way up to sim objects in the S_define.

```C++
class MysimObject : public Trick::SimObject {
public:
CantCopyMe ccm ;
private:
MysimObject( const MysimObject & ) ;
MysimObject& operator = ( const MysimObject) ;
}
```

##### Source Code in Header Files

Trick attempts to skip over class code in header files while searching for class variables and method declarations. However, code can sometimes confuse Trick and cause it to abort processing of header files. It is recommended to keep code out of the header file..

##### Library Dependencies

It is good practice to list all the source code files that define class methods in the class header file.

##### Excluding Header File Code

There are several ways to exclude code from processing.

Excluding Directories

Add paths to exclude to the TRICK_ICG_EXCLUDE environment variable or makefile variable. This works for both C and C++ headers.

###### Excluding File

Add `ICG: (No)` to the Trick comment header.

###### Excluding Lines

When processing header files Trick defines 2 #define variables, TRICK_ICG and SWIG. Code may be excluded by enclosing it in #ifndef blocks.
```C++
#ifndef TRICK_ICG
code that cannot be processed by ICG
#ifndef SWIG
code that cannot be processed by ICG or SWIG
#endif
#endif
```

##### Source Files

By source files, in this context, we mean functional model source code, i.e. *.c files.

```C
/* [TRICK_HEADER]
PURPOSE:
    (Purpose statement.)
[REFERENCES:
    ((Reference #1) (Reference #n)])]
[ASSUMPTIONS AND LIMITATIONS:
    ((Assumption #1) (Assumption #n)])]
[LIBRARY DEPENDENCY:
    (
     (object.o|lib.a|lib.so|<relative_path>/lib.a)
     [(object_n.o|lib_n.a|lib_n.so|<relative_path>/lib_n.a)]
    )]
[PROGRAMMERS:
   (((Name) (Company) (Date) [(other info)])
   [((Name) (Company) (Date) [(other info)])]]
*/

// source code...
```

##### Comment Header

The Trick header is an optional comment block at the top of each source file. It is used for auto-documentation, and more importantly is the means of specifying dependencies to objects or libraries not processed by Trick. Separate functions within a source file do NOT require additional headers. Since parentheses, ( ), are used to delineate fields within the comment header, parentheses are not allowed as characters within the comment fields. NOTE: Even if you are coding a C++ file, you must still specify the comment header using C style comments (not C++ style comments).

###### Job Description

* The PURPOSE field should be a brief description of what the module does.
* The REFERENCES field may contain any number of references, with each reference possessing any number of sub items; notice the nested parentheses for the REFERENCES field.
* The ASSUMPTIONS AND LIMITATIONS field may contain any number of assumptions and limitations delimited by parentheses.
* The LIBRARY DEPENDENCIES. See Library_Dependencies section in the model header section
* The PROGRAMMERS field may contain any number of programmer fields, each of which may contain any number of sub items; e.g. programmer name, company, mod date, etc. The programmer fields are meant to provide an in-code means to track code changes.

##### Source Code

Trick is only interested in the header comment if one is present in source code files. Anything goes for the rest of the source code file.

##### Trick Version Compatibility

Trick is always changing. The interface to Trick functions may change with each major version. Sometimes even minor version upgrades changes the interface. When Trick builds model source code it includes -DTRICK_VER=<version> and -DTRICK_MINOR=<minor_version> to the TRICK_CFLAGS and TRICK_CXXFLAGS. This allows developers to key off the Trick version in model source code. If there are any compile issues dependent on Trick version, this #define may be useful.

[Continue to Environment Variables](Environment-Variables)

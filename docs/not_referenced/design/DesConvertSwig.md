# convert_swig

The purpose of convert_swig is to create SWIG interface files for the given
C/C++ header file (usually S_source.hh) and each of the header files that it
(recursively) includes. SWIG (Simplified Wrapper and Interface Generator) is
an interface compiler that connects programs written in C and C++ with scripting
languagues such as Perl and Python.

In Trick the list of header files to be processed is usually produced by the script
make_swig_makefile.pm, as it's creating Makefile_swig. This list is stored in
the file ".S_library_swig". So, if .S_library_swig exists, we can just open and read it.

Otherwise we need to process S_source.hh to produce the list of header files.

Specifically, we want to generate  SWIG interfaces for those header files that are:

1. actual dependencies of S_source.hh, GIVEN THE CURRENT environment and
1. not excluded from ICG processing ( by ICG_NO or ICG_EXCLUDE).

The header files that are actually included are the dependencies we care
about. Keep in mind that the preprocessor and the current ENVIRONMENT
may cause some headers to be conditionally included or excluded. We only
want to generate SWIG interfaces for headers that are ACTUALLY included.

Whereas the preprocessor can (using the gcc -MM option) generate a list
of dependencies that satisfy 1) (above), it can't handle that ICG exclusions.
And, whereas the function get_headers() can generate a list of dependences
which are flagged if they contain ICG_NO, it doesn't handle conditional includes.

So, the strategy that we employ is to generate and then find the
intersection of both lists. Then we eliminate those that are in 1)
$TRICK_HOME/trick_source, or 2) flagged as containing ICG_NO  or 3) are
in ICG_EXCLUDE'd directories.

First, create a list headers using the GCC with the -MM option. GCC will
handle conditional inclusion.

Second, create a list where the files are flagged if they contain ICG_NO.

Then we generate the intersection of the two lists and then eliminate the dependencies that:

1. are in $TRICK_HOME/trick_source.
1. contain ICG_NO.
1. are in ICG_EXCLUDE'd directories.

to create the final list of header dependencies that we need to convert into SWIG interfaces.

Next we need to determine which of the files do not have up-to-date SWIG files.
For each header file in final dependency list, if the corresponding SWIG (.i) file
doesn't exist or the header file is newer than the existing SWIG file, then record
that a new SWIG file needs needs to be created. The global hash %out_of_date
represents a list of header files whose corresponding .i files need to be regenerated.

Finally, call process_file() to create SWIG interface files for each of the out_of_date headers.

## process_file

### Synopsis

This subroutine processes S_source.h and each of it's requisite header files to
generate the corresponding SWIG interfaces files.

### Parameters

<b>sim_ref</b> Is this parameter ever used?

<b>in_file</b> The name of input file, invariably "S_source.hh".

### Design

This function first reads the contents of <in_file> and each of the header files
that it (recursively) includes into a hash (%file_contents), keyed by the
corresponding filenames. It then converts the contents of the each header whose
corresponding SWIG interface file is out of date into a SWIG interface file.

#### Creating the %file_contents hash

To create the file_contents hash, we first run the input file through the
C/C++ preprocessor with the -dI option. This creates <b>one</b> file containing
the contents of all of the included header files delimited by "line markers".
The line markers indicate which header file each of content came from.
Preprocessor line markers are of the form: '#' <linenum> <filename> <flags>
They are described in Chapter 9, "Preprocessor Output" of the GCC document,
"Preprocessor Output - The C Preprocessor". Whether in_file is run through
the C or C++ preprocessor depends on its name suffix.

For each line in the ONE big file, check whether it's a linemarker or not.
if it's a linemarker ( telling us where the following content is from)
extract the header filename. This will be our current filecontents hash key.
If it's not a linemarker, then it must be content. So, append it to the string,
whose key is the current file name ($curr_name).

#### Creating SWIG interface files

The global hash <i>\%out_of_date</i> represents the list of header files whose
corresponding SWIG interface files are out of date. It is generated in the main
part of the convert_swig program.

For each of these out of date header files, we generate a SWIG interface file
from the contents stored in the <i>\%file_contents</i> hash.

First we remove the friend init_attr functions from the headers content.
They don't need to be wrapped.

Then, for each of the #includes in the out_of_date header file
create a corresponding %import directive.

Next, we generate a module name and path for the SWIG interface file. The module
name is generated from an md5 hash of the header file's full name.

Finally we open the SWIG interface file, and in it we:

1. write a %module directive that identifies the module.
1. write a #include directive to include trick_swig interface utilities.
1. write a #include directive to include the header file to which this
 interface file corresponds and from which it was derived.
1. create a SWIG interface for each class declared in the corresponding
 header file using the %trick_swig_class_typemap() macro. This macro is
 defined in swig_class_typedef.i, included by trick_swig.i (see above).
1. Write the SWIG interface code (processed header file) and the header
 file contents.


## process_contents

### Synopsis

Process header file contents for use in the corresponding SWIG interface file.

### Parameters

<b>contents_ref</b>
 (IN) reference to header file contents that are to be converted to a SWIG interface.

<b>new_contents_ref</b>
 (OUT) SWIG interface code, derived from the header file contents.

<b>curr_namespace</b>
 (IN) current namespace.

<b>class_names_ref</b>
 (OUT) reference to an array of class and/or struct names encountered when
 processing the header file contents.

<b>template_typedefs_ref</b>
 (OUT) Series of SWIG %template directives. %template directives create a type in
 the target language that corresponds to a C++ template instance.

### Description

While there's header file content remaining to be processed, repeatedly make the
best match with the following available patterns:

Case of :<br>
<b>typedef</b> <i>existing-type-name new-type-name</i> <b>';'</b><br>
Concatenate the matched text to the SWIG interface text.

Case of :<br>
<b>typedef enum</b> <i>optional-name</i> <b>'{'</b> <i>bracketed-content</i> <b>'}'</b> <i>enum-name</i><b>';'</b><br>
Concatenate the matched text to the SWIG interface text.

Case of :<br>
<b>typedef</b> ( <b>struct</b> | <b>union</b> ) <i>name</i> <b>'{'</b><br>
Call process_typedef_struct() to process the matched text and the bracketed
content of the struct that follows in the header file contents and update the
SWIG interface text accordingly.

Case of :<br>
<b>template '<'</b> <i>template-parameters</i> <b>'>' class</b> <i>class-name</i><br>
then just concatenate the matched text to the SWIG interface text.

Case of:<br>
<b>namespace</b> <i>name</i><br>
then call <i>process_namespace()</i> to process the matched text and the
bracketed content that follows in the header file contents and update the the
SWIG interface text accordingly.

Case of:<br>
( <b>class</b> | <b>struct</b> ) <i>class-name</i> ( <b>'{'</b> | <b>':'</b> )<br>
Call <i>process_class()</i> to process the matched text and the bracketed
that follows in the header file contents and update the the  SWIG interface text.

Default:
Match anything that doesn't match the other patterns and concatenate it to the
to the SWIG interface text. Note that (in Perl) <b>*?</b> in the regular
expression <b>(.*?)</b> is a non-greedy quantifier, so it gobbles up text only
until another match can be made.

## process_namespace

### Synopsis

Process namespaces found in a header file for use in the corresponding SWIG
interface file.

### Parameters

<b>namespace_string</b>
(IN) This is a string of the form: <b>namespace</b> <i>name</i>, that was
extracted from the header file contents. In the contents there should remain the bracketed
content to which this namespace applies.

<b>contents_ref</b>
(IN) This is a reference to the remainder of the header file (following the
 above string) to be processed.

<b>new_contents_ref</b>
(OUT) The SWIG code generated so far.

<b>curr_namespace</b>
(IN) current namespace.

<b>class_names_ref</b>
(OUT) reference to an array of class and/or struct names encountered when
processing the header file contents.

<b>template_typedefs_ref</b>
(OUT) Series of SWIG %template directives. %template directives create a type
in the target language that corresponds to a C++ template instance.

### Description

Extract the name from the <b>namespace_string</b> and append it to the current namespace's name.

Add the <b>namespace_string</b> to the SWIG interface text.

Call extract_bracketed() to extract the contents of the namespace from the header text.

Call process_contents() to convert the extracted namespace contents to a SWIG interface.

Append whatever wasn't matched in process contents to the SWIG interface text.

## process_class

### Synopsis
Process classes declarations found in a header file for use in the corresponding
SWIG interface file.

### Parameters

<b>class_string</b>
(IN) This is a string of the form:<br>
( <b>class</b> | <b>struct</b> ) <i>class-name</i> ( <b>'{'</b> | <b>':'</b> )<br>

<b>contents_ref</b>
(IN) This is a reference to the remainder of the header file (following the
class_string) to be processed.

<b>new_contents_ref</b>
(OUT) The SWIG code generated so far.

<b>curr_namespace</b>
(IN) current namespace.

<b>class_names_ref</b>
(OUT) reference to an array of class and/or struct names encountered when
processing the header file contents.

<b>template_typedefs_ref</b>
(OUT) Series of SWIG %template directives. %template directives create a type
in the target language that corresponds to a C++ template instance.

### Description

process_class() processes class declarations with the following steps:

1. Extract the class_name from the class_string.
Add the <b>class_string</b> to the SWIG interface text.

1. Call <b>extract_bracketed()</b> to extract the class contents between '{' and '}'.

1. While there's class content text remaining to be processed,
repeatedly search for data members that match : <b>template_name '<'</b> <i>template-params</i> <b>'>' name ;</b>
For each match, create a SWIG %template directive to create an instantiation
of the specific templated type used by the data member. Add the
SWIG %template directive to the templated typedefs string that
Otherwise append whatever wasn't matched in process contents to
the SWIG interface text.

## process_typedef_struct

### Synopsis

Process a type definition of a struct or union to make it suitable as SWIG
interface code. Extract the struct (or union) name and bracketed contents from
the header file text (typedef_struct_string and contents_ref) . Record the
extracted names in the list referenced by class_names_ref, and then reconsistute
the type definition, via the new_contents_ref.

### Parameters

<b>typedef_struct_string</b>
 (IN) This is a string of the form:<br>

     1. <b>typedef struct</b> [ <i>optional-name</i> ] <b>"{"</b>   OR<br>
     1. <b>typedef union</b> [ <i>optional-name</i> ] <b>"{"</b><br>

<b>contents_ref</b>
 (IN) This is a reference to the remainder of the header file (following the
 above string) to be processed.

<b>new_contents_ref</b>
 (OUT) The SWIG interface code generated so far.

<b>class_names_ref</b>
 (OUT) reference to an array of class and/or struct names encountered when
 processing the header file contents.

### Description

process_typedef_struct() processes a type definition of a struct or union
with the following steps:

1. Append the <b>typedef_struct_string</b> to the SWIG interface text
(via <b>new_contents_ref</b>).

1. Extract the optional-name from the typedef_struct_string.

1. Call extract_bracketed() to extract the struct contents from the header
text (via <b>contents_ref</b>), including the right bracket.

1. Extract the one or more typedef'ed names followed by a semi-colon, that
should still be in the header text.

1. Push the optional-name and the typedef'ed names into the class_names list
(via <b>class_names_ref</b>).

1. Append the bracketed struct contents and the one or more typedef'ed names
and the semi-colon that we just extracted to the SWIG interface text.


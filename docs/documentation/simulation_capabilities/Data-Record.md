
Data Recording provides the capability to specify any number of data recording groups,
each with an unlimited number of parameter references, and with each group recording
at different frequencies to different files in different formats. 
 
All data is written to the simulation output directory.

### Format of Recording Groups

Trick allows recording in three different formats. Each recording group is readable by
different external tools outside of Trick.

- DRAscii - Human readable and compatible with Excel.
- DRBinary - Readable by previous Trick data products.
- DRHDF5 - Readable by Matlab.

DRHDF5 recording support is off by default.  To enable DRHDF5 support Trick must be built with HDF5 support.
Go to http://www.hdf5group.org and download the latest pre-built hdf5 package for your system. Source packages are
available as well.  We recommend getting the static library packages above the shared.  Static packages make 
your executable larger, but you will not have to deal with LD_LIBRARY issues.  The HDF5 package may be installed
anywhere on your system.  To tell Trick you have HDF5 run ${TRICK_HOME}/configure --with-hdf5=/path/to/hdf5.
Re-compile Trick to enable HDF5 support.

### Creating a New Recording Group

To create a new recording group, in the Python input file instantiate a new group by format name:
<tt><variable_name> = trick.<data_record_format>() ;</tt>

For example:

```
drg = trick.DRBinary() ;
```

Note: drg is just an example name.  Any name may be used. 

### Adding a Variable To Be Recorded

To add variables to the recording group call the <tt>drg.add_variable("<string_of_variable_name">)</tt> method of the recording group.
For example:

```python
drg.add_variable("ball.obj.state.output.position[0]")
drg.add_variable("ball.obj.state.output.position[1]")
```

An optional alias may also be specified in the method as <tt>drg.add_variable("<string_of_variable_name"> [, "<alias>"])</tt>.  
If an alias is present as a second argument, the alias name will be used in the data recording file instead of the actual variable name.
For example:

```python
drg.add_variable("ball.obj.state.output.position[0]", "x_pos")
drg.add_variable("ball.obj.state.output.position[1]", "y_pos")
```

### Changing the Recording Rate

To change the recording rate call the <tt>set_cycle()</tt> method of the recording group.

```python
drg.set_cycle(0.01) 
```

### Buffering Techniques

Data recording groups have three buffering options:

- DR_Buffer - the group will save recorded data to a buffer and use a separate thread to write recorded 
data to disk.  This will have little impact to the performance of the simulation.  The downside
is that if the simulation crashes, the most recent recorded points may not be written to disk in time.
DR_Buffer is the default buffering technique. (For backwards compatibility, DR_Buffer can also be called DR_Thread_Buffer).
- DR_No_Buffer - the group will write recorded data straight to disk.  All data is guaranteed to be written
to disk at simulation termination time.  The downside of this method is that it is performed in
the main thread of the simulation and could impact real-time performance.
- DR_Ring_Buffer - the group will save a set number of records in memory and write this data to disk during
a graceful simulation termination.  The advantage of this method is that there is only a set, usually
small, number of records written.  The downside of this method is that if the simulation terminates
ungracefully, all recorded data may be lost.

To set the buffering technique call the <tt>set_buffer_type(trick.<buffering_option>)</tt> method of the recording group.
For example:

```python
drg.set_buffer_type(trick.DR_Buffer) 
```

All buffering options (except for DR_No_Buffer) have a maximum amount of memory allocated to
holding data.  See Trick::DataRecordGroup::set_max_buffer_size for buffer size information.

### Recording Frequency: Always or Only When Data Changes

Data recording groups have three recording frequency options:

- DR_Always - the group will record the variable value(s) at every recording cycle. (This is the default).
- DR_Changes - the group will record the variable value(s) only when a particular watched parameter (or parameters) value changes.
- DR_Changes_Step - like DR_Changes, except that a before and after value will be recorded for each variable,
creating a stair step effect (instead of point-to-point) when plotted.

To set the recording frequency call the <tt>set_freq(trick.<frequency_option>)</tt> method of the recording group. For example:

```python
drg.set_freq(trick.DR_Changes)
```

For DR_Changes or DR_Changes_Step, to specify parameter(s) to watch that will control when the variables added with <tt>add_variable</tt> are recorded,
call the <tt>add_change_variable(string)</tt> method of the recording group. For example:

```python
drg.add_change_variable("ball.obj.state.output.velocity[0]") 
```

So if we assume the <tt>add_variable</tt> statements from the example in @ref S_7_8_3 "7.8.3" combined with the above <tt>add_change_variable</tt> statement,
then <tt>ball.obj.state.output.position[0]</tt> and <tt>ball.obj.state.output.position[1]</tt> will be recorded only when
<tt>ball.obj.state.output.velocity[0]</tt> changes. Multiple parameters may be watched by adding more change variables, in which case
data will be recorded when any of the watched variable values change.

### Turn Off/On and Record Individual Recording Groups

At any time during the simulation, model code or the input processor can turn on/off individual
recording groups as well as record a single point of data.

```c++
/* C code */
dr_enable_group("<group_name">) ;
dr_disable_group("<group_name">) ;
dr_record_now_group("<group_name">) ;
```

This is the Python input file version:

```python
# Python code
trick.dr_enable_group("<group_name">) ;  # same as <group_name>.enable()
trick.dr_disable_group("<group_name">) ; # same as <group_name>.disable()
trick.dr_record_now_group("<group_name">) ;
```

### Changing the thread Data Recording runs on.

To change the thread that the data recording group runs on use the DataRecordGroup::set_thread
method.  The thread number follows the same numbering as the child threads in the S_define file.
This must be done before the add_data_record_group function is called.  Trick does not
provide data locks for data record groups.  It is up to the user to ensure that the data
recorded on *any* thread (including the master) is ready in order for data recording to
record a time homogeneous set of data.

```python
drg.set_thread(<thread_number>)
```

### Changing the Job Class of a Data Record Group

The default job class of a data record group is "data_record".  This job class is run after all
of the cyclic job classes have completed.  The job class of the data record group can be
changed through the set_job_class method.  The data recording job will be added to the end of
the job class queue it is set.

```python
drg.set_job_class(<string class_name>)
```
 

### Changing the Max File Size of a Data Record Group (Ascii and Binary only)

The default size of a data record is 1 GiB. A new size can be set through the set_max_file_size method. For unlimited size, pass 0.

```python
drg.set_max_file_size(<uint64 file_size_in_bytes>)
```

### Example Data Recording Group

This is an example of a data recording group in the input file

```python
# Data recording HDF5 test
drg0 = trick.DRHDF5("Ball")
drg0.add_variable("ball.obj.state.output.position[0]") 
drg0.add_variable("ball.obj.state.output.position[1]") 
drg0.add_variable("ball.obj.state.output.velocity[0]") 
drg0.add_variable("ball.obj.state.output.velocity[1]") 
drg0.add_variable("ball.obj.state.output.acceleration[0]") 
drg0.add_variable("ball.obj.state.output.acceleration[1]") 
drg0.set_cycle(0.01)
drg0.freq = trick.DR_Always
trick.add_data_record_group(drg0, trick.DR_Buffer)

# This line is to tell python not to free this memory when drg0 goes out of scope
drg0.thisown = 0
```

### User accessible routines

Create a new data recording group:

```c++
Trick::DRAscii::DRAscii(string in_name);
Trick::DRBinary::DRBinary(string in_name);
Trick::DRHDF5::DRHDF5(string in_name);
```

This list of routines is for all recording formats:

```c++
int dr_disable_group( const char * in_name );
int dr_enable_group( const char * in_name );
int dr_record_now_group( const char * in_name );

int Trick::DataRecordGroup::add_variable
int Trick::DataRecordGroup::add_change_variable
int Trick::DataRecordGroup::disable
int Trick::DataRecordGroup::enable
int Trick::DataRecordGroup::set_cycle
int Trick::DataRecordGroup::set_freq
int Trick::DataRecordGroup::set_job_class
int Trick::DataRecordGroup::set_max_buffer_size

```
This list of routines provide file size configuration for Ascii and Binary:

```c++

int set_max_size_record_group (const char * in_name, uint64_t bytes ) ;
int dr_set_max_file_size ( uint64_t bytes ) ;

int Trick::DataRecordGroup::set_max_file_size

```

This list of routines provide some additional configuration for DR_Ascii format only:

```c++
int Trick::DRAscii::set_ascii_double_format
int Trick::DRAscii::set_ascii_float_format
int Trick::DRAscii::set_delimiter
int Trick::DataRecordGroup::set_single_prec_only
```

### DRAscii Recording Format

The DRAscii recording format is a comma separated value file named log_<group_name>.csv.  The contents
of this file type are readable by the Trick Data Products packages, ascii editors, and Microsoft Excel.
The format of the file follows.  Users are able to change the comma delimiter to another string.  Changing
the delimiter will change the file extension from ".csv" to ".txt".

```
name_1 {units_1},name_2 {units_2},etc...
value1,value2,etc...
value1,value2,etc...
value1,value2,etc...
value1,value2,etc...
```

### DRBinary Recording Format

The DRBinary recording format is a Trick simulation specific format.  Files written in this format are named
log_<group_name>.trk.  The contents of this file type are readable by the Trick Data Products packages from
Trick 07 to the current version.  The format of the file follows.

<a id=drbinary-file></a>
### DRBinary-File
|Value|Description|Type|#Bytes|
|---|---|---|---|
|Trick-\<vv>-\<e>| \<vv> is trick version (2 chars, "07" or "10"). \<e> is endianness (1 char) 'L' -> little endian, and 'B' -> big endian.|char|10|
|*numparms*|Number of recorded variables |char|4|
|| List of Variable Descriptors | [Variable-Descriptor-List](#variable-descriptor-list)||
|| List Data Records |[Data-Record-List](#data-record-list)||
|EOF| End of File |||


<a id=variable-descriptor-list></a>
#### Variable-Descriptor-List
A Variable-Descriptor-List is a sequence of [Variable-Descriptors](#variable-descriptor).
The number of descriptors in the list is specified by *numparms*. The list describes each of the recorded variables, starting with the simulation time variable.

|Value|Description|Type|#Bytes|
|---|---|---|---|
|[*Time-Variable-Descriptor*](#time-variable-descriptor)| Descriptor for Variable # 1. This first descriptor always represents the simulation time variable.| [Variable-Descriptor](#variable-descriptor) |34|
|...|...|...|...|
|| Descriptor for Variable # *numparms* |[Variable-Descriptor](#variable-descriptor)|variable|

<a id=variable-descriptor></a>
#### Variable-Descriptor
A Variable-Descriptor describes a recorded variable. 

|Value|Description|Type|Bytes|
|---|---|---|---|
| *namelen*| Length of Variable Name |int|4|
| *name*   | Variable Name ||*namelen*|
| *unitlen*| Length of Variable Units |int|4|
| *unit*   | Variable Units ||*unitlen*|
| *type*   | Variable Type (see Notes 2. & 3.)|int|4|
| *sizeof(type)*   | Variable Type Size |int|4|

**Notes:**

1. the size of a Variable-Descriptor in bytes = *namelen* + *unitlen* + 16.
2. If *vv* = "07", use [Trick 07 Data Types](#trick-07-data-types).
3. If *vv* = "10", use [Trick 10 Data Types](#trick-10-data-types).

<a id=time-variable-descriptor></a>
#### *Time-Variable-Descriptor*
|Value|Description|Type|Bytes|
|---|---|---|---|
|17| Length of Variable Name |int|4|
|```sys.exec.out.time```| Variable Name |char|17|
|1| Length of Variable Units |int|4|
|```s```| Variable Units (see Note 1.) |char|1|
|11| Variable Type |int|4|
|8| Variable Type Size |int|4|

**Notes:**

1. Here, we are assuming "vv" = "10", and so, referring to [Trick 10 Data Types](#trick-10-data-types), Variable Type = 11, which corresponds to **double**.

<a id=data-record-list></a>
#### Data-Record-List
A Data-Record-List contains a collection of [Data-Records](#data-record), at regular times.

|Value|Description|Type|Bytes|
|---|---|---|---|
||Data-Record #1|[Data-Record](#data-record)||
|...|...|...|...|
||Data-Record #Last|[Data-Record](#data-record)||

<a id=data-record></a>
#### Data-Record
A Data-Record contains a collection of values for each of the variables we are recording, at a specific time.

|Value|Description|Type|Bytes|
|---|---|---|---|
|*time*|Value of Variable #1 (time) |*typeof( Variable#1 )*|*sizeof( typeof( Variable#1))* = 8|
|...|...|...|...|
|*value*|Value of Variable #numparms |*typeof( Variable#numparms)*|*sizeof( type-of( Variable#numparms))* |

<a id=trick-07-data-types></a>
### Trick 7 Data Types
The following data-types were used in Trick-07 data recording files (that is for, *vv* = "07").

|Type value|Data Type|
|---|---|
|0|char|
|1|unsigned char|
|2|string (char\*)|
|3|short|
|4|unsigned short|
|5|int|
|6|unsigned int|
|7|long|
|8|unsigned long|
|9|float|
|10|double|
|11|Bit field|
|12|unsigned Bit field|
|13|long long|
|14|unsigned long long|
|17|Boolean (C++)|

<a id=trick-10-data-types></a>
### Trick 10 Data Types
The following data-types are used in Trick versions >= 10, that is for, *vv* = "10".

|Type value|Data Type|
|---|---|
|1 | char |
|2|unsigned char|
|4| short |
|5|unsigned short|
|6| int |
|7|unsigned int|
|8| long |
|9|unsigned long|
|10| float |
|11| double |
|12|Bit field|
|13|unsigned Bit field|
|14|long long|
|15|unsigned long long|
|17|Boolean (C++)``|
### DRHDF5 Recording Format

HDF5 recording format is an industry conforming HDF5 formatted file.  Files written in this format are named
log_<group_name>.hd5.  The contents of this file type are readable by the Trick Data Products packages from
Trick 07 to the current version.  The contents of the file are binary and is not included here.  The HDF5 layout
of the file follows.

```
GROUP "/" {
    GROUP "header" {
        DATASET "file_names" {
            "param_1_file_name", "param_2_file_name", etc...
        }
        DATASET "param_names" {
            "param_1_name", "param_2_name", etc...
        }
        DATASET "param_types" {
            "param_1_type", "param_2_type", etc...
        }
        DATASET "param_units" {
            "param_1_units", "param_2_units", etc...
        }
    }
    DATASET "parameter #1" {
        value1 , value2 , value3 , etc...
    }
    DATASET "parameter #2" {
        value1 , value2 , value3 , etc...
    }
    .
    .
    .
    DATASET "parameter #n" {
        value1 , value2 , value3 , etc...
    }
}
```

[Continue to Checkpointing](Checkpoints)

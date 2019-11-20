
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

<table>
<tr><th>Value</th><th>Description</th><th>Type</th><th>Bytes</th></tr>
<tr><td colspan=4 align=center>START OF HEADER</td></tr>
<tr><td>Trick-\<vv\>-\<e\></td><td>\<vv\> is trick version, 2 characters (e.g. 07)
\<e\> is endianness, 1 character: L for little endian, B for big endian</td><td>string</td><td>10</td></tr>
<tr><td>\<numparms\></td><td>Number of parameters recorded</td><td>int</td><td>4</td></tr>
<tr><td>17</td><td>parameter \#1 Name string length</td><td>int</td><td>4</td></tr>
<tr><td>sys.exec.out.time</td><td>parameter \#1 Name (1st parameter is always the system time)</td><td>string</td><td>17</td></tr>
<tr><td>1</td><td>parameter \#1 Units Name string length</td><td>int</td><td>4</td></tr>
<tr><td>s</td><td>parameter \#1 Units Name</td><td>string</td><td>1</td></tr>
<tr><td>10</td><td>parameter \#1 Type (see Table 15)</td><td>int</td><td>4</td></tr>
<tr><td>8</td><td>parameter \#1 Size (number of bytes the value occupies)</td><td>int</td><td>4</td></tr>
<tr><td>\<namelen\></td><td>parameter \#2 Name string length</td><td>int</td><td>4</td></tr>
<tr><td>\<name\></td><td>parameter \#2 Name</td><td>string</td><td>\<namelen\></td></tr>
<tr><td>\<unitlen\></td><td>parameter \#2 Units Name string length</td><td>int</td><td>4</td></tr>
<tr><td>\<unit\></td><td>parameter \#2 Units Name</td><td>string</td><td>\<unitlen\></td></tr>
<tr><td>\<type\></td><td>parameter \#2 Type</td><td>int</td><td>4</td></tr>
<tr><td><size></td><td>parameter \#2 Size</td><td>int</td><td>4</td></tr>
<tr><td></td><td>.</td><td></td><td></td></tr>
<tr><td></td><td>.</td><td></td><td></td></tr>
<tr><td></td><td>.</td><td></td><td></td></tr>
<tr><td>\<namelen\></td><td>parameter \#n Name string length</td><td>int</td><td>4</td></tr>
<tr><td>\<name\></td><td>parameter \#n Name</td><td>string</td><td>\<namelen\></td></tr>
<tr><td>\<unitlen\></td><td>parameter \#n Units Name string length</td><td>int</td><td>4</td></tr>
<tr><td>\<unit\></td><td>parameter \#n Units Name</td><td>string</td><td>\<unitlen\></td></tr>
<tr><td>\<type\></td><td>parameter \#n Type</td><td>int</td><td>4</td></tr>
<tr><td><size></td><td>parameter \#n Size</td><td>int</td><td>4</td></tr>
<tr><td colspan=4 align=center>END OF HEADER, START OF RECORDED DATA</td></tr>
<tr><td>\<value\></td><td>parameter \#1 Value</td><td>10</td><td>8</td></tr>
<tr><td>\<value\></td><td>parameter \#2 Value</td><td>\<type\></td><td>\<size\></td></tr>
<tr><td></td><td>.</td><td></td><td></td></tr>
<tr><td></td><td>.</td><td></td><td></td></tr>
<tr><td></td><td>.</td><td></td><td></td></tr>
<tr><td>\<value\></td><td>parameter \#n Value</td><td>\<type\></td><td>\<size\></td></tr>
<tr><td colspan=4 align=center>REPEAT RECORDED DATA FOR EACH CYCLE</td></tr>
<tr><td colspan=4 align=center>END OF RECORDED DATA</td></tr>
</table>

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

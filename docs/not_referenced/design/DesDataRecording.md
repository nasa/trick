# Data Recording Design

The data recording design consists of 3 object types working together.  At the bottom
level are the data recording groups.  The data recording groups contain the user
specified variables to record, the frequency and the format to write the data in.
Each data recording group is associated with a buffering technique object.  These objects
determine when the data recording groups write their data.  Finally at the top is the
data recording dispatcher.  The dispatcher holds the list of all the buffering techniques
and serves as a single calling point for the simulation executive to fire off all
data recording group activities.

## DataRecordGroup

The first objects are a set of DataRecordGroups.  The DataRecordGroups are
responsible for copying simulation data into a temporary memory buffer
while the simulation is running.  The DataRecordGroups also are responsible
for formatting the simulation data into a recording format.

A base DataRecordGroup object provides functions common to all recording
groups and provides an interface for the specific recording formats to follow.
The base DataRecordGroup derives from the SimObject class.  This allows
new data recording groups to be added and removed from the scheduler as
other SimObjects.

The base class provides the following common functions:

1. Common initialization of a data record group.
1. Common copying of simulation data to memory buffers.

The base class provides function definitions for the specific formats to define:

1. Format specific initialization
1. Format specific write data from memory buffer to disk
1. Format specific shutdown of data recording files.

## DataRecordGroup Common Initialization

Initialization of a DataRecord group is handled in three phases.

The first is during the construction of the object.  New data recording group
objects are typically created within the run input file read in during simulation
initialization.

@copydetails Trick::DataRecordGroup::DataRecordGroup(string)
Trick::DataRecordGroup::DataRecordGroup(string)

The second part of a DataRecordGroup initialization is supplied by the user.
The user typically sets these in the run input file read in during simulation
initialization.  Typically the user will:

1. Add variables to be recorded.
1. Add variables to be added to the changes watch list.
1. Assign a different recording frequency.
1. Change the amount of memory data record is to allocate for memory buffers

The third part of initialization occurs at the end of simulation initialization.
During this phase all data recording groups finish their initialization.  Format
specific initialization is also called during this phase.

@copydetails Trick::DataRecordGroup::init()
Trick::DataRecordGroup::init()

### DataRecordGroup Common Copy Simulation Data

This routine copies simulation data into a large buffer which will be written to
disk when commanded by the DataRecordDispatcher.  Copying the simulation off into
a buffer allows the simulation to continue executing and data recording to use
a separate thread of execution to write the data to disk.  The following algorithm

-# Return if data recording is not enabled.
-# If the recording frequency is to record on parameter changes
   -# For each change parameter compare the current value against the previous frame value.
      -# If the value has changed mark the change_detected flag true
      -# Copy the current value to the previous frame value.
-# If the recording frequency is set to always or the change_detected flag is true
   -# If the recording frequency is to record on parameter changes step
       -# Copy to memory the previous value with the current time stamp
   -# If the copy memory pointer is at the end of the buffer, set the pointer to the
      beginning of the buffer
   -# Copy to memory the current value with the current time stamp 

### Data Recording Specific Formats Routines

#### ASCII Recording

@copydoc Trick::DRAscii
Trick::DRAscii

##### ASCII Initialization

@copydetails Trick::DRAscii::format_specific_init()
Trick::DRAscii::format_specific_init()

##### ASCII Write Data

@copydetails Trick::DRAscii::write_data()
Trick::DRAscii::write_data()

##### ASCII Shutdown

@copydetails Trick::DRAscii::shutdown()
Trick::DRAscii::shutdown()

#### Binary Recording

@copydoc Trick::DRBinary
Trick::DRBinary

##### Binary Initialization

@copydetails Trick::DRBinary::format_specific_init()
Trick::DRBinary::format_specific_init()

##### Binary Write Data

@copydetails Trick::DRBinary::write_data()
Trick::DRBinary::write_data()

##### Binary Shutdown

@copydetails Trick::DRBinary::shutdown()
Trick::DRBinary::shutdown()

#### HDF5 Recording

@copydoc Trick::DRHDF5
Trick::DRHDF5

##### HDF5 Initialization

@copydetails Trick::DRHDF5::format_specific_init()
Trick::DRHDF5::format_specific_init()

##### HDF5 Write Data

@copydetails Trick::DRHDF5::write_data()
Trick::DRHDF5::write_data()

##### HDF5 Shutdown

@copydetails Trick::DRHDF5::shutdown()
Trick::DRHDF5::shutdown()

## DataRecordBuffering

The second objects are a set of DataRecordBuffering objects.  These objects
are responsible for writing the copied simulation data to disk.  There are
three techniques to write data to disk.

A base DataRecordBuffering object provides a common interface for each of
the buffering techniques.  The common object provides routines for.  These
base routines may be overridden by the specific buffering techniques.

1. Processing simulation arguments
1. Initialization
1. Adding a data recording group

The base DataRecordBuffering provides hooks for:

1. Processing data at the end of each software frame
1. Processing data at shutdown.

An empty initialization routine is provided by the base object if the
specific buffering techniques do not require a specialized initialization routine.

The buffering classes do not write the data themselves, rather they call the data record
groups write_data routines to do the writing.

The 3 specific buffering techniques are

1. Write directly to disk immediately.
1. Use a thread to write data to disk asynchronously
1. Do not write data until simulation shutdown.  Allow data copying to
   overwrite data stored in the memory buffer.

### DataRecordBuffering Base Routines

#### DataRecordBuffering Process Simulation Arguments

@copydetails Trick::DataRecordBuffering::process_sim_args()
Trick::DataRecordBuffering::process_sim_args()

#### DataRecordBuffering Initialization

@copydetails Trick::DataRecordBuffering::init()
Trick::DataRecordBuffering::init()

#### DataRecordBuffering Adding a Recording Group

@copydetails Trick::DataRecordBuffering::add_group()
Trick::DataRecordBuffering::add_group()

### DataRecordDisk

This buffering technique calls all of the data record write_data routines in the
main simulation thread of execution.

#### DataRecordDisk End of Frame Processing

@copydetails Trick::DataRecordDisk::write_data()
Trick::DataRecordDisk::write_data()

#### DataRecordDisk Shutdown Processing

@copydetails Trick::DataRecordDisk::shutdown()
Trick::DataRecordDisk::shutdown()

### DataRecordRing

This buffering technique does not call the write_data routines during runtime.  Only
at shutdown does the write_data routines get called.  The last contents of the memory
buffer are then dumped to disk.

#### DataRecordRing End of Frame Processing

@copydetails Trick::DataRecordRing::write_data()
Trick::DataRecordRing::write_data()

#### DataRecordRing Shutdown Processing

@copydetails Trick::DataRecordRing::shutdown()
Trick::DataRecordRing::shutdown()

### DataRecordThreaded

This buffering technique uses a separate thread to write data to disk.  The main
thread of execution attempts to signal the thread at the end of each frame. The
thread calls the group write_data routines when signaled.

#### DataRecordThreaded Writer

@copydetails Trick::DataRecordThreaded::drt_writer()
Trick::DataRecordThreaded::drt_writer()

#### DataRecordThreaded End of Frame Processing

@copydetails Trick::DataRecordThreaded::write_data()
Trick::DataRecordThreaded::write_data()

#### DataRecordThreaded Shutdown Processing

@copydetails Trick::DataRecordThreaded::shutdown()
Trick::DataRecordThreaded::shutdown()

## DataRecordDispatcher

The third object is the DataRecordDispatcher.  The DataRecordDispatcher
calls each data recording buffering object's end of frame and shutdown
jobs.  It serves as a single entry point for all data recording activities.

The dispatcher is intended to be the only data recording object called by
the %Trick scheduler.  The dispatcher contains routines to

1. Process simulation arguments
1. Add data recording group
1. Initialization
1. End of Frame processing
1. Shutdown

### DataRecordDispatcher Process Simulation Arguments

@copydetails Trick::DataRecordDispatcher::process_sim_args()
Trick::DataRecordDispatcher::process_sim_args()

### DataRecordDispatcher Add Data Recording Group

When a data recording group is added to the DataRecordDispatcher the group is
tied to the requested buffering technique.

@copydetails Trick::DataRecordDispatcher::add_group()
Trick::DataRecordDispatcher::add_group()

### DataRecordDispatcher Initialization

@copydetails Trick::DataRecordDispatcher::init()
Trick::DataRecordDispatcher::init()

### DataRecordDispatcher End of Frame Processing

@copydetails Trick::DataRecordDispatcher::write_data()
Trick::DataRecordDispatcher::write_data()

### DataRecordDispatcher Shutdown

@copydetails Trick::DataRecordDispatcher::shutdown()
Trick::DataRecordDispatcher::shutdown()



When running a Trick simulation, unless specifically turned off, a server called the
"variable server" is always up and listening in a separate thread of execution. The
variable server is privy to simulation parameters and their values since it resides
in an asynchronous simulation thread. Threads share the same address space as their
siblings and parent. Clients connect to the variable server in order to set/get
values of Trick processed variables. You may already be familiar with the Trick
applications that use the variable server: the simulation control panel, Trick
View (TV) , [Event/Malfunction Trick View](/trick/documentation/running_a_simulation/runtime_guis/MalfunctionsTrickView) (MTV) , and the stripchart.

The variable server is a convenient way for external applications to interact with
the simulation. Any application that needs to set or get simulation parameters may
do so through the variable server. The external application need not be on the same
machine since the connection to the variable server is via a Trick communication
TCP/IP socket.

### User accessible routines

These commands are for enabling/disabling the variable server, and for getting its status.
The variable server is enabled by default.

```c
int var_server_set_enabled(int on_off);
int var_server_get_enabled();
```

<b>Disabling the variable server will disable all Trick runtime GUIs: simulation
control panel, TV, MTV, and stripchart.</b>

These commands are for toggling information messages from the variable server (i.e., commands received from <i>ALL</i> clients).
The messages go to the terminal, the simulation control panel, and the "send_hs" file in the RUN directory.
The variable server information message capability is off by default.

```c
int set_var_server_info_msg_off();
int set_var_server_info_msg_on();
```

These commands are also for toggling information messages from the variable server (i.e., commands received from <i>ALL</i> clients).
The messages only go to a dedicated "varserver_log" file in the RUN directory.
The variable server log capability is off by default.

```c
int set_var_server_log_off();
int set_var_server_log_on();
```

#### Getting and Setting the Variable Server Port Information

To set the variable server port to a fixed number in the input file use var_server_set_port()

```python
trick.var_server_set_port( unsigned int port )
```

To get the variable server host and port information in the input file use var_server_get_hostname() and
var_server_get_port().

```python
trick.var_server_get_hostname()
trick.var_server_get_port()
```

### Commands

The variable server accepts commands in the form of strings. The variable server parses
these commands using the Python input processor. So in theory, any Python valid syntax
is acceptable to the variable server. This section lists the commands that are specific
for the variable server. Commands are sent over a Trick communication TCP/IP socket to
the variable server. Multiple commands (newline separated) can be sent in the string
over the socket. The variable server will send back information to the requesting client.

If the command contains a syntax error, Python will print an error message to the screen, 
but nothing will be returned to the client.

#### Adding a Variable

```python
trick.var_add( string var_name )
```
or
```python
trick.var_add( string var_name , string units )
```

Adding a variable will tell the variable server to send the variable's value back to the
client at a specified frequency.  An optional units parameter may be attached to the
variable as the desired return units.  Multiple variables may be added to the list to be sent
back to the client.  The format of the returned values are described below, Ascii Format
or binary format.

Simulation time as a decimal number in "seconds" is available through a special var_add command.  This time marks the simulation time at the start of the variable server's task to copy variables.

```python
trick.var_add("time")
```

#### Time Homogeneous or Synchronous Data

##### Copying Data Out of Simulation.

```python
trick.var_set_copy_mode(int mode)
```

There are 3 options to when the variable server will copy data out from the simulation.
Each option has unique capabilites.

###### Asynchronous Copy (mode = trick.VS_COPY_ASYNC or 0)

This is the default. Values are copied out of the sim asynchronously.  Copies are done
approximately at the var_cycle() rate during run and freeze mode.  A separate thread
is used to copy the data.  The data is not guaranteed to be time homogenous.  This mode
does not affect the main thread real-time performance.

###### End of Main Thread Execution Copy (mode = trick.VS_COPY_SCHEDULED or 1)

This mode copies data at the end of execution frame.  Copies are done exactly at the
var_cycle() rate after the main thread has finished all of it's jobs scheduled to run
at that time step both in run and freeze mode.  All variables solely calculated in the
main thread are guaranteed to be time homogenous.  Variables calculated in child
threads are not guaranteed to be time homogenous.  Copying data may very slightly
affect the main thread real-time performance.

###### Top of Frame Copy (mode = trick.VS_COPY_TOP_OF_FRAME or 2)

This mode copies data at the top of frame.  Copies are done at a multiple and offset of
the Executive software frame.  During freeze mode copies are made at a multiple and offset
of the freeze frame.  With careful planning, all variables from all threads can be
guaranteed to be time homogenous. Copying data may very slightly affect the main thread
real-time performance.

To set the frame multiplier and frame offset between copies use the following commands.
The frame refers to the software frame in the Executive.  In freeze mode a different
multiplier and offset are used.

```python
trick.var_set_frame_multiplier(int mult)
trick.var_set_frame_offset(int offset)

trick.var_set_freeze_frame_multiplier(int mult)
trick.var_set_freeze_frame_offset(int offset)
```

##### Writing Data Out of Simulation.

```python
trick.var_set_write_mode(int mode)
```

There are 2 options when the variable server writes the data.

###### Asynchronous Write ( mode = trick.VS_WRITE_ASYNC or 0 )

This is the default. Values are written onto the socket asynchronously.  Writes are done
approximately at the var_cycle() rate during run and freeze mode.  A separate thread
is used to copy write data.  This mode does not affect the main thread real-time performance.

###### Write When Copied ( mode = trick.VS_WRITE_WHEN_COPIED or 1 )

Values are written onto the socket as soon as they are copied from the simulation. The
write rate depends on the copy. Writes are done in the main thread of execution.  This
can greatly affect real-tim performance if a large amount of data is requested.

##### Old Style var_sync() Command

```python
trick.var_sync(bool mode)
```

var_sync() was previously used to control the copies and writes from the simulation.
The number of options has outgrown what a single var_sync command can configure.  It
may still be used to configure a subset of the copy/write combinations.

```python
trick.var_sync(0) # asynchronous copy and asynchronous write.
trick.var_sync(1) # end of main thread copy and asynchronous write.
trick.var_sync(2) # end of main thread copy and write when copied.
```

#### Sending the Return Values Immediately

```python
trick.var_send()
```

The var_send command forces the variable server to return the list of values to the
client immediately.

#### Changing the Units

```python
trick.var_units( string var_name , string units )
```

The returned values can be converted to other units of measurments. The var_units command
tells the variable server what units to use.  If the units are changed, then the units
are included in the returned string to the client.

#### Removing a Variable

```python
trick.var_remove( string var_name )
```

Removing a variable removes the variable from the list returned to the client.

#### Clearing the List of Variables

```python
trick.var_clear()
```

To clear the whole list of variables sent to the client.

#### Exiting the Variable Server

```python
trick.var_exit()
```

Disconnects the current client from the variable server.

#### Checking for existence of a variable

```python
trick.var_exists( string var_name )
```

To test if a variable name exists.  A special response is sent to the client when
this command is processed.

In **var_binary** mode, the (4 byte) message indicator of the response will be 1,
followed by a (1 byte) value of 0 or 1 to indicate the existence of the variable.

In **var_ascii** mode: the message indicator of the response will be "1" followed
by a tab, then an ASCII "0" or "1" to indicate the existence of the variable.

#### Changing the Return Value Cycle Rate

```python
trick.var_cycle( double cycle_rate )
```

Changes the rate of the return messages to the client.  This rate is estimated and may not
perfectly match the requested rate.

#### Pause the Variable Server

```python
trick.var_pause()
```

Pauses the return values sent to the client.  Even when paused, the variable server will
accept new commands.

#### Unpause the Variable Server

```python
trick.var_unpause()
```

Resumes sending the return values to the client.

#### Setting Ascii Return Format

```python
trick.var_ascii()
```

Sets the return message format to ASCII. See below for the format of the message.

#### Setting Binary Return Format

```python
trick.var_binary()
```

Sets the return message format to Binary. See below for the format of the message.

```python
trick.var_binary_nonames()
```

This variation of the binary format reduces the amount of data that is sent to the client.
See below for the exact format.

#### Sending stdout and stderr to client

```python
trick.var_set_send_stdio(bool on_off)
```

If var_set_send_stdio is called with a true value, then all python stdout and stderr
output will be redirected to the client instead of printing to the simulation stdout/stderr
location.  Note: output from C/C++ code called from python will direct it's output to
the simulation stdout/stderr location.  See the return message format for Stdio.

This is useful to get output from the simulation such as the return values of a function.

```
# Example in a variable server client to get the Trick version used to compile a sim
# The C prototype is "const char *exec_get_current_version(void) ;"
trick.var_set_send_stdio(True)
sys.stdout.write(trick.exec_get_current_version())

# The returned text will look like this. See the return message format below
4  1       10
10.7.dev-1

# If a "print" is used instead of sys.stdout.write, a second message is sent containing
# a single newline.
print "trick.exec_get_current_version()"

4  1       10
10.7.dev-14  1        1
 <- a single newline is the second message
```

#### Debugging Variable Server Messages

```python
trick.var_debug(int level)
```

The level may range from 0-3.  The larger the number the more debugging information is printed to
the screen (for the current client only).

#### Logging Messages to file.

These commands are for toggling information messages from the variable server (for this client only).
The messages only go to a dedicated "varserver_log" file in the RUN directory.
The variable server log capability is off by default. (See the global variable server commands
@link Trick::VariableServer::set_var_server_log_on() set_var_server_log_on() @endlink and 
@link Trick::VariableServer::set_var_server_log_off() set_var_server_log_off() @endlink for toggling
the logging capability for <i>ALL</i> clients.)

```python
trick.var_server_log_on()
trick.var_server_log_off()
```

#### Setting Variable Server Client Tag

```python
trick.var_set_client_tag(string name)
```

This sets an identifying name tag to be associated with the current client that will be printed with each information message
displayed. Information messages are displayed as a result of
@link Trick::VariableServer::set_var_server_info_msg_on() set_var_server_info_msg_on() @endlink,
@c var_server_log_on() or
@c var_debug(). For instance, Trick sets a name tag for each of its variable server clients (simulation control panel is "SimControl",
TV is "TRICK_TV", etc.).

#### Byteswapping

```python
trick.var_byteswap(bool on_off)
```

### Returned Values

By default the values retrieved are sent asynchronously to the client. That is, the values
retrieved by the variable server are pulled directly from memory asynchronously and do not
guarantee synchronization from the same simulation execution frames unless the var_sync
command is used. Values will be returned to the client in the same order that they were
issued in the var_add command(s).  Typically the client receives the data from the variable
server in a buffer via the tc_read command (see TrickComm for more information).

### Ascii Format

The default format, or if var_ascii is commanded specifically, causes the variable server
to return a buffer containing a tab delimited character string in the following format:

```
0\t<variable1 value>[\t<variable2 value>. . .\t<variableN value>]
```

where N is the number of variables registered via the var_add command(s). The "\t" represents
a tab character, and the "\n" is the newline character that always ends the string.  Note
that if a value being returned is itself a character string data type, any tab (or other
unprintable character) that occurs within the character string value will appear as an
escaped character, i.e. preceded by a backslash.

The 1st value returned in the list will always be a message indicator. The possible
values of the message indicator are:
- 0 returned variable value(s) from var_add or var_send
- 1 returned value from var_exists
- 2 returned value from send_sie_resource (special command used by Trick View)
- 3 returned value from send_event_data (special command used by Events/Malfunctions Trick View) or var_send_list_size
- 4 values redirected from stdio if var_set_send_stdio is enabled

If the variable units are also specified along with the variable name in a var_add or
var_units command, then that variable will also have its units specification returned following
its associated value separated by a single blank. For example, if the 2nd of N variables was
specified with {<units>} in either a var_add or var_units command, the returned string would
be in the following format:

```
0\t<variable1 value>\t<variable2 value> {<variable2 units>}. . .\t<variableN value>
```

Note that the maximum message size that the variable server sends to the client is 8192 bytes.
If the amount of data requested is larger than that, the ASCII message will be split into
multiple messages.  The client is responsible for concatenating the multiple messages back
together.  (Hint: look for the "\n" delimter)

If a syntax error occurs when processing the variable server client command, Python will print
an error message to the screen, but nothing will be returned to the client.

If a var_add command was issued for a non-existent variable, there will be a one time Trick error
message printed to the screen, but the resulting data sent to the client is still ok. The value
returned for the non-existent variable is the string "BAD_REF".

### Binary Format

By specifying the var_binary or var_binary_nonames command, the variable server will return
values in a binary message formatted as follows:

```
<message_indicator><message_size><N>
<variable1_namelength><variable1_name><variable1_type><variable1_size><variable1_value>
<variable2_namelength><variable2_name><variable2_type><variable2_size><variable2_value>
. . .
<variableN_namelength><variableN_name><variableN_type><variableN_size><variableN_value>
```

Where the first 12 bytes are the message header:
- message_indicator is the same possible values as in var_ascii shown above : a 4 byte integer
- message_size is the total size of the message in bytes (NOT including message_indicator) : a 4 byte integer
- N is the number of variables registered via the var_add command(s) : a 4 byte integer
.
and the remaining bytes of the message contain the variable data:
- variable_namelength is the string length of the variable name : a 4 byte integer (NOT present for var_binary_nonames)
- variable_name is the ASCII variable name string : @e variable_namelength bytes of string (NOT present for var_binary_nonames)
- variable_type is Trick data type of the variable : a 4 byte integer (see Trick::MemoryManager::TRICK_TYPE)
- variable_size is number of bytes the variable occupies in memory : a 4 byte integer
- variable_value is the variable's current value : @e variable_size bytes of @e variable_type

When the client has requested a very large amount of data, it is possible that it may require
more than one message to be returned.  The maximum message size is 8192 bytes, so if the data
returned by the variable server requires more space than that (once formatted into the above
message format), then the variable server sends more than one message.  This is indicated by
the @e N field.  For example, if the client has requested 15 variables, and @e N = 15, then
everything is contained in that one message.  However if @e N < 15, then the client should
continue reading messages until all @e N received add up to 15.

If a syntax error occurs when processing the variable server client command, Python will print
an error message to the screen, but nothing will be returned to the client.

If a var_add command was issued for a non-existent variable, there will be a one time Trick error
message printed to the screen, but the resulting data sent to the client is still ok. The message 
returned for the non-existent variable will have a type of 24 and it's value will be the string "BAD_REF".

### Stdio Format

These messages are sent to the client if stdout and stderr are redirected. See "Sending stdout
and stderr to client" for more details.

```
4 <stream> <size>
<text>
```

- message_id Stdio messages are message_id = 4.
- stream is the stream the message was written to.  1 = stdout, 2 = stderr
- size is the number of bytes in the <text> section.  The newline between the <size> and <text>
is not counted in the size.
- text is the message

Only output from python is redirected, i.e. "print" or calls to "sys.stdout.write()".  C/C++ code
called from python will still direct their stdout/stderr to the simulation output location.
The "print" statement will send 2 messages, the text in the print, and an additional newline.
Calls to sys.stdout.write() only generate 1 message.

Error messages printed by python to stderr may be sent in multiple messages.

### Variable Server Broadcast Channel

To connect to the variable server for any simulation, a client needs to know the
hostname and port.  As of 10.5, the port number is determined by the OS.  For external
applications the best way to find a varible server port is to listen to the variable
server broadcast channel.  Every simulation variable server will broadcast the host and port
number to the broadcast channel.  The channel is address 224.3.14.15 port 9265.  All simulations
on your network sends it's information to this address and port so there may be multiple
messages with variable server information available here.  Here is some
C code that reads all messages on the variable server channel.

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {

    int mcast_socket ;
    char buf1[1024] ;
    ssize_t num_bytes ;
    int value = 1;
    struct sockaddr_in sockin ;
    struct ip_mreq mreq;

    if ((mcast_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("init socket");
    }

    if (setsockopt(mcast_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &value, (socklen_t) sizeof(value)) < 0) {
        perror("setsockopt: reuseaddr");
    }
#ifdef SO_REUSEPORT
    if (setsockopt(mcast_socket, SOL_SOCKET, SO_REUSEPORT, (char *) &value, sizeof(value)) < 0) {
        perror("setsockopt: reuseport");
    }
#endif

    // Use setsockopt() to request that the kernel join a multicast group
    mreq.imr_multiaddr.s_addr = inet_addr("224.3.14.15");
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(mcast_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &mreq, (socklen_t) sizeof(mreq)) < 0) {
        perror("setsockopt: ip_add_membership");
    }

    // Set up destination address
    sockin.sin_family = AF_INET;
    sockin.sin_addr.s_addr = htonl(INADDR_ANY);
    sockin.sin_port = htons(9265);

    if ( bind(mcast_socket, (struct sockaddr *) &sockin, (socklen_t) sizeof(sockin)) < 0 ) {
        perror("bind");
    }

    do {
        num_bytes = recvfrom(mcast_socket, buf1, 1024, 0 , NULL, NULL) ;
        if ( num_bytes > 0 ) {
            buf1[num_bytes] = '\0' ;
            printf("%s\n" , buf1) ;
        }
    } while ( num_bytes > 0 ) ;

    return 0 ;
}
```

The information sent by each variable server is a tab delimited list of strings
1. Hostname
2. Port
3. User
4. Process ID (PID)
5. Simulation directory
6. S_main command line name
7. Input file
8. Trick version of simulation
9. User defined tag
10. Port (duplicate field for backwards compatibility)

[Continue to Status Message System](Status-Message-System)

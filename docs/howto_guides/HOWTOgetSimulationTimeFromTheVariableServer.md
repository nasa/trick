## How to Get Simulation Time from the Variable Server

For historical reasons ```sys.exec.out.time``` is the name given to simulation time in data recording files. ```sys.exec.out.time``` isn’t a real variable in a Trick sim. So you can’t get it from the variable server directly.  
The way to get the sim time from the Trick variable server is to get the following two variables and then calculate the sim-time.
 
```trick_sys.sched.time_tic_value``` - **integer** that represents the number of tics per second.
```trick_sys.sched.time_tics``` - **integer** that represents the current simulation time in time tics.
 
To calculate the simulation time:

1. Convert the value of ```trick_sys.sched.time_tics``` to a floating point number.
2. Then divide that by the value of ```trick_sys.sched.time_tic_value```.

Below is a simple Python variable server client that gets and prints simulation time for any modern Trick based simulation.

To invoke it:
```
$ ./SimTimeExample.py <variable-server-port-number>
```

### Example (SimTimeExample.py)

```python
#!/usr/bin/python
import sys
import socket

# Process the command line arguments.
if ( len(sys.argv) == 2) :
    trick_varserver_port = int(sys.argv[1])
else :
    print( "Usage: vsclient <port_number>")
    sys.exit()

# Connect to the variable server.
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect( ("localhost", trick_varserver_port) )
insock = client_socket.makefile("r")

client_socket.send( "trick.var_pause()\n" )
client_socket.send( "trick.var_ascii()\n" )

# Get the number of tics per second (just once).
client_socket.send( "trick.var_add(\"trick_sys.sched.time_tic_value\")\n")
client_socket.send( "trick.var_send()\n" )
line = insock.readline()
field = line.split("\t")
tics_per_second = int(field[1]);
client_socket.send( "trick.var_clear()\n" )

# Get the number of time_tics, and whatever else you want to recieve periodically.
client_socket.send( "trick.var_add(\"trick_sys.sched.time_tics\") \n"
                  )

# Start the flow of data from the variable server.
client_socket.send( "trick.var_unpause()\n" )

# Repeatedly read and process the responses from the variable server.
while(True):
    line = insock.readline()
    if line == '':
        break

    field = line.split("\t")
    time_tics = int(field[1]);
    
    # Calculate sim_time 
    sim_time = float(time_tics) / tics_per_second

    print 'sim_time = {0}'.format(sim_time)
```

If you are unfamiliar or rusty on how to use the Trick variable server, please see
the [Variable Server](/trick/tutorial/TutVariableServer) section of the [Trick Tutorial](/trick/tutorial/Tutorial).

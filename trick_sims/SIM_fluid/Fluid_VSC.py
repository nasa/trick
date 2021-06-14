#!/usr/bin/python
import sys
import socket

# Process command line args
if ( len(sys.argv) == 2) :
	trick_varserver_port = int(sys.argv[1])
else :
	print("Usage: vsclient <port_number>")
	sys.exit()

# Connect to the variable server
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect( ("localhost", trick_varserver_port) )
insock = client_socket.makefile("r")

# Unit test 0: request viscosity of the fluid
client_socket.send("trick.var_pause()\n".encode())
client_socket.send("trick.var_ascii()\n".encode())
client_socket.send("trick.var_add(\"dyn.fluid.VISC\")\n".encode())
client_socket.send("trick.var_unpause()\n".encode())

# Unit test 1: request the position of a particle
client_socket.send("trick.var_pause()\n".encode())
client_socket.send("trick.var_ascii()\n".encode())
client_socket.send("trick.var_add(\"dyn.fluid.particlesArr[0].pos[0]\")\n".encode())
client_socket.send("trick.var_unpause()\n".encode())

# Repeatedly read and process the responses from the variable server.
while (True):
	line = insock.readline()
	if line == '':
		break
	print(line)
#!/usr/bin/python
import sys
import socket
from tkinter import *

# Process command line args
if ( len(sys.argv) == 2) :
	trick_varserver_port = int(sys.argv[1])
else :
	print("Usage: vsclient <port_number>")
	sys.exit()


# Create the GUI
tk = Tk()
canvas = Canvas(tk, width=200, height=200)
tk.title("SPH Display")
canvas.pack()

# Create oval objects to represent fluid particles
particle_radius = 2;
fluidParticle = canvas.create_oval(0, 0, particle_radius, particle_radius, fill="blue")

# Connect to the variable server
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect( ("localhost", trick_varserver_port) )
insock = client_socket.makefile("r")

# Unit test 0: request viscosity of the fluid
client_socket.send("trick.var_pause()\n".encode())
client_socket.send("trick.var_ascii()\n".encode())
client_socket.send("trick.var_add(\"dyn.fluid.VISC\")\n".encode())
client_socket.send("trick.var_unpause()\n".encode())

# Unit test 1: request the x position of a particle
client_socket.send("trick.var_pause()\n".encode())
client_socket.send("trick.var_ascii()\n".encode())
client_socket.send("trick.var_add(\"dyn.fluid.particlesArr[0].pos[0]\")\n".encode())
client_socket.send("trick.var_unpause()\n".encode())


client_socket.send("trick.var_pause()\n".encode())
client_socket.send("trick.var_ascii()\n".encode())
client_socket.send("trick.var_add(\"dyn.fluid.particlesArr[16].pos[0]\")\n".encode())
client_socket.send("trick.var_add(\"dyn.fluid.particlesArr[16].pos[1]\")\n".encode())
client_socket.send("trick.var_unpause()\n".encode())

# Repeatedly read and process the responses from the variable server.
while (True):
	line = insock.readline()
	if line == '':
		break
	field = line.split("\t")
	# Get position of particle and update it on the canvas
	x = float(field[1])
	y = float(field[2])
	cx = x + 100
	cy = 200 - (y + 100)
	canvas.coords(fluidParticle, cx - particle_radius, cy - particle_radius, cx + particle_radius, cy+ particle_radius)
	
	print(line)
	# Update the Tk graphics
	tk.update()
	
# Keep the window open, when the data stops.
tk.mainloop()
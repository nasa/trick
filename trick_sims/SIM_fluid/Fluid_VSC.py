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

# Connect to the variable server
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect( ("localhost", trick_varserver_port) )
insock = client_socket.makefile("r")

# Request size of BOUND in order to determine window size
# Also request the number of particles for rendering
client_socket.send("trick.var_pause()\n".encode())
client_socket.send("trick.var_ascii()\n".encode())
client_socket.send("trick.var_add(\"dyn.fluid.BOUND\")\n".encode())
client_socket.send("trick.var_add(\"dyn.fluid.NUM_PARTICLES\")\n".encode())
client_socket.send("trick.var_unpause()\n".encode())
line = insock.readline()
field = line.split("\t")
#print(field)
BOUND = int(field[1])
NUM_PARTICLES = int(field[2])
HEIGHT = 2 * BOUND
WIDTH = HEIGHT
"""
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
"""


# Create the GUI
tk = Tk()
canvas = Canvas(tk, width=WIDTH, height=HEIGHT)
tk.title("SPH Display")
canvas.pack()

# Create oval objects to represent fluid particles
particle_radius = 2;
fluidParticles = []
for i in range(NUM_PARTICLES):
	fluidParticles.append(canvas.create_oval(0, 0, particle_radius, particle_radius, fill="blue"))



client_socket.send("trick.var_pause()\n".encode())
client_socket.send("trick.var_ascii()\n".encode())
for i in range(NUM_PARTICLES):
	client_socket.send("trick.var_add(\"dyn.fluid.particlesArr[{}].pos[0]\")\n".format(i).encode())
	client_socket.send("trick.var_add(\"dyn.fluid.particlesArr[{}].pos[1]\")\n".format(i).encode())
client_socket.send("trick.var_unpause()\n".encode())

# Repeatedly read and process the responses from the variable server.
while (True):
	line = insock.readline()
	if line == '':
		break
	field = line.split("\t")
	# Get position of particle and update it on the canvas
	for i in range(NUM_PARTICLES):
		# TODO: calculate correct offset to index into field (one particle is not rendering correctly)
		y = float(field[2 * i])
		x = float(field[2 * i + 1])
		cx = x + BOUND
		cy = HEIGHT - (y + BOUND)
		canvas.coords(fluidParticles[i], cx - particle_radius, cy - particle_radius, cx + particle_radius, cy+ particle_radius)
	
	#print(line)
	# Update the Tk graphics
	tk.update()
	
# Keep the window open, when the data stops.
tk.mainloop()

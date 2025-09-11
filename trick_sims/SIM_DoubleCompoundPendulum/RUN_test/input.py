################################################################################
#     ___   ___  _   _ ___  _   ___                                            #
#    |   \ / _ \| | | | - )| | |  _|                                           #
#    | |) | (_) | |_| |--| | |_| --                                            #
#    |__ / \___/|_____|_-_)|___|___|                                           #
#                                                                              #
#     __  ___ _   ____  _   _ _   _   _ _    _                                 #
#    | _\|  _| \ | |  \| | | | | | | | | \  / |                                #
#    | _/| --|  \| ||) | |_| | |_| |_| |  \/  |                                #
#    |_| |___|_|\__|__/|_____|___|_____|_|\/|_|                                #
#                                                                              #
#     __      ___   ___ ____                                                   #
#     __| ___|   \ / _ \| __|                                                  #
#    |    ___| |) | (_) | _|                                                   #
#    |__     |___/ \___/|_|                                                    #
#                                                                              #
# This input creates a double pendulum with DOF:                             #
#		-Creates 2 bodies                      			       #
#		-Initializes 2 DOF                                             #
################################################################################

# ============================================================================ -
# This file is used to store several quality of life python functions.
# ============================================================================ -
exec(open("Modified_data/state_data.dr").read())
#exec(open("Modified_data/realtime.py").read())

################################################################################
# dyn DATA	    							       #
#	(NOTE: dyn is our instance of BodySimObject)                           #
#	This builds our objects, and defines their physical properties.        #
################################################################################

# ============================================================================ -
# Establish the physical properties of the body...CONSTANTS! 
# ============================================================================ -
gravity = 9.81
mass1 = 10
mass2 = 10

length1 = 1.0
length2 = 1.0

radius1 = length1 * 0.5
radius2 = length2 * 0.5

dyn.pendulum.mass1 = mass1
dyn.pendulum.mass2 = mass2

dyn.pendulum.radius1 = radius1
dyn.pendulum.length1 = length1

dyn.pendulum.radius2 = radius2
dyn.pendulum.length2 = length2

dyn.pendulum.gravity = gravity


dyn.pendulum.inertia1 = (1.0/3.0) * mass1 * length1 * length1
dyn.pendulum.inertia2 = (1.0/3.0) * mass2 * length2 * length2
dyn.pendulum.Ja = (0.25 * mass1 * length1 * length1) + (mass2 * length1 * length1) + ((1.0/12.0) * mass1 * length1 * length1)
dyn.pendulum.Jb = ((1.0/12) * mass2 * length2 * length2) + (0.25 * mass2 * length2 * length2)
dyn.pendulum.Jx = 0.5 * mass2 * length1 * length2
dyn.pendulum.mu1 = ((0.5 * mass1) + mass2) * gravity * length2
dyn.pendulum.mu2 = 0.5 * mass2 * gravity * length2



# ============================================================================ -
# Initialize system 
# ============================================================================ -

dyn.pendulum.ANGLE1_INIT = 0.7853981633974483
dyn.pendulum.ANGLE2_INIT = 0.7853981633974483

################################################################################
# SIMULATION SET UP                                                            #
#	This is where we set up the remaining inputs to create a working sim.  #
################################################################################

# ============================================================================ -
# getintegrator(integration method, number of variables to be integrated)
# This sets up the integration scheme we use to analyze this system.
# ============================================================================ -
dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 20)

# ============================================================================ -
# This tells the sim to stop after a few seconds
# ============================================================================ -

trick.stop(10.0)

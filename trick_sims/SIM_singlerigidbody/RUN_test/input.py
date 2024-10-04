################################################################################
#     _     ___          _                                                     #
#    / |___| _ ) ___  __| |_  _                                                #
#    | |___| _ \/ _ \/ _` | || |                                               #
#    |_|   |___/\___/\__,_|\_, |                                               #
#     __     ___   ___  __|__/                                                 # 
#    / / ___|   \ / _ \| __|                                                   #
#   / _ \___| |) | (_) | _|                                                    #
#   \___/   |___/ \___/|_|                                                     #
#                                                                              #
# This input creates a simple body with 6 DOF:                                 #
#		-Creates 1 body                      			       #
#		-Initializes 6 DOF                                             #
################################################################################

# ============================================================================ -
# This file is used to store several quality of life python functions.
# ============================================================================ -
exec(open("Modified_data/state_data.dr").read())
exec(open("Modified_data/realtime.py").read())

################################################################################
# dyn DATA	    							       #
#	(NOTE: dyn is our instance of BodySimObject)                           #
#	This builds our objects, and defines their physical properties.        #
################################################################################

# ============================================================================ -
# Establish the physical properties of the body...CONSTANTS! 
# ============================================================================ -
dyn.body.mass = 20.0
dyn.body.radius = 2.0
      
# ============================================================================ -
# Initialize system 
# ============================================================================ -
M_PI = 3.141592653589793238460

#dyn.body.POS_INIT[0] = 0.0
#dyn.body.POS_INIT[1] = 0.0
#dyn.body.POS_INIT[2] = 0.0

#dyn.body.ANGLE_INIT[0] = 0.0
#dyn.body.ANGLE_INIT[1] = 0.0
#dyn.body.ANGLE_INIT[2] = 0.0

#dyn.body.OMEGA_INIT[0] = 0.0
#dyn.body.OMEGA_INIT[1] = 0.0
#dyn.body.OMEGA_INIT[2] = 0.0

#dyn.body.FORCE_INIT[0] = 0.0
#dyn.body.FORCE_INIT[1] = 0.0
#dyn.body.FORCE_INIT[2] = 10.0

#dyn.body.TORQUE_INIT[0] = 0.0
#dyn.body.TORQUE_INIT[1] = 0.0
#dyn.body.TORQUE_INIT[2] = 0.0

#dyn.body.VEL_INIT[0] = 0.0
#dyn.body.VEL_INIT[1] = 0.0
#dyn.body.VEL_INIT[2] = 0.0

#dyn.body.POS_FORCE[0] = 2.0
#dyn.body.POS_FORCE[1] = 0.0
#dyn.body.POS_FORCE[2] = 0.0

################################################################################
# SIMULATION SET UP                                                            #
#	This is where we set up the remaining inputs to create a working sim.  #
################################################################################

# ============================================================================ -
# getintegrator(integration method, number of variables to be integrated)
# This sets up the integration scheme we use to analyze this system.
# ============================================================================ -
dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 12)

# ============================================================================ -
# This tells the sim to stop after a few second
# ============================================================================ -
trick.stop(10.0)



#==========================================
# Start the Graphics Client
#==========================================
varServerPort = trick.var_server_get_port();
SRBDisplay_path = "models/singlerigidbodygraphics/build/SingleRigidBodyDisplay.jar"

if (os.path.isfile(SRBDisplay_path)) :
    SRBDisplay_cmd = "java -jar " \
                   + SRBDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(SRBDisplay_cmd)
    os.system(SRBDisplay_cmd);
else :
    print('======================================================================================================')
    print('SRBDisplay needs to be built. Please \"cd\" into ../models/singlerigidbodygraphics and type \"make\".')
    print('======================================================================================================')


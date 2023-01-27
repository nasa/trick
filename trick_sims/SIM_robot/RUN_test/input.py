import math
exec(open("./Modified_data/realtime.py").read())

# Variable Server Data should be copied at top of frame.
trick.var_set_copy_mode(2)

dyn.arm.nlinks = 2
dyn.arm.links = trick.TMM_declare_var_1d("Link*", dyn.arm.nlinks)
#
dyn.arm.links[0] = trick.make_Link(  (math.pi), 1.0)
dyn.arm.links[1] = trick.make_Link( -(math.pi/4.0), 1.0)



dyn_integloop.getIntegrator(trick.Euler, dyn.arm.nlinks)

#==========================================
# Start the Satellite Graphics Client
#==========================================
varServerPort = trick.var_server_get_port();
RobotDisplay_path = "models/graphics/build/RobotDisplay.jar"

if (os.path.isfile(RobotDisplay_path)) :
    RobotDisplay_cmd = "java -jar " \
                   + RobotDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(RobotDisplay_cmd)
    os.system( RobotDisplay_cmd);
else :
    print('==================================================================================')
    print('RobotDisplay needs to be built. Please \"cd\" into ../models/graphics and type \"make\".')
    print('==================================================================================')

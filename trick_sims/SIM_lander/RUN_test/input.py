execfile("Modified_data/realtime.py")

dyn.lander.pos[0] = 0
dyn.lander.pos[1] = 1.8
dyn.lander.vel[0] = 0.0
dyn.lander.vel[1] = 0.0
dyn.lander.angleDot = 0.0
dyn.lander.angle = 0.0

trick.TMM_set_debug_level(1)
#==========================================
# Start the Satellite Graphics Client
#==========================================
varServerPort = trick.var_server_get_port();
LanderDisplay_path = "models/graphics/dist/LanderDisplay.jar"

if (os.path.isfile(LanderDisplay_path)) :
    LanderDisplay_cmd = "java -jar " \
                   + LanderDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(LanderDisplay_cmd)
    os.system( LanderDisplay_cmd);
else :
    print('==================================================================================')
    print('LanderDisplay needs to be built. Please \"cd\" into ../models/graphics and type \"make\".')
    print('==================================================================================')

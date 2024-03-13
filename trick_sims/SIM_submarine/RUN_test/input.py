exec(open("./Modified_data/realtime.py").read())

dyn.submarine.pos[0] =  0.0
dyn.submarine.pos[1] = -0.5
dyn.submarine.vel[0] =  0.0
dyn.submarine.vel[1] =  0.0

# ==========================================
# Start the Satellite Graphics Client
# ==========================================
varServerPort = trick.var_server_get_port();
SubmarineDisplay_path = "models/graphics/build/SubmarineDisplay.jar"

if (os.path.isfile(SubmarineDisplay_path)) :
    SubmarineDisplay_cmd = "java -jar " \
                   + SubmarineDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(SubmarineDisplay_cmd)
    os.system( SubmarineDisplay_cmd);
else :
    print('==================================================================================')
    print('SubmarineDisplay needs to be built. Please \"cd\" into ../models/graphics and type \"mvn package\".')
    print('==================================================================================')

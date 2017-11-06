execfile("Modified_data/realtime.py")

#==========================================
# Start the display VarServer Client
#==========================================
varServerPort = trick.var_server_get_port();
CannonDisplay_path = "../models/graphics/dist/CannonDisplay.jar"

if (os.path.isfile(CannonDisplay_path)) :
    CannonDisplay_cmd = "java -jar " \
                   + CannonDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(CannonDisplay_cmd)
    os.system( CannonDisplay_cmd);
else :
    print('==================================================================================')
    print('CannonDisplay needs to be built. Please \"cd\" into ../models/graphics and type \"make\".')
    print('==================================================================================')


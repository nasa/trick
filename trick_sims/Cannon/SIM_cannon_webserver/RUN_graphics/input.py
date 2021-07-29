exec(open("Modified_data/realtime.py").read())

#==========================================
# Start the Cannonball Graphics Client
#==========================================
web.server.enable = True
# web.server.debug = True
# web.server.port = 8888
# web.server.document_root = "www"

trick.var_server_set_port(5001);
varServerPort = trick.var_server_get_port();
CannonDisplay_path = "../models/graphics/dist/CannonDisplay.jar"

print("pos: ", dyn.cannon.pos[0])

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


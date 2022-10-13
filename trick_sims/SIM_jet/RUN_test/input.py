# ==========================================
# Start the Jet Graphics Client
# ==========================================
varServerPort = trick.var_server_get_port();
JetDisplay_path = "models/graphics/build/JetDisplay.jar";

if (os.path.isfile(JetDisplay_path)) :
    JetDisplay_cmd = "java -jar " \
                   + JetDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(JetDisplay_cmd)
    os.system( JetDisplay_cmd);
else :
    print('==================================================================================')
    print('JetDisplay needs to be built. Please \"cd\" into ../models/graphics and type \"mvn package\".')
    print('==================================================================================')
#==========================================
# Start the display VarServer Client
#==========================================
varServerPort = trick.var_server_get_port();
BallDisplay_path = "../models/graphics/dist/BallDisplay.jar"

if (os.path.isfile(BallDisplay_path)) :
    BallDisplay_cmd = "java -jar " \
                   + BallDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(BallDisplay_cmd)
    os.system( BallDisplay_cmd);
else :
    print('==================================================================================')
    print('BallDisplay needs to be built. Please \"cd\" into models/Graphics and type \"make\".')
    print('==================================================================================')

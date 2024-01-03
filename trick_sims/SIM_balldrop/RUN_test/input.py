exec(open("Modified_data/realtime.py").read())

trick.stop(60.0)

# ==========================================
# Start the Satellite Graphics Client
# ==========================================
varServerPort = trick.var_server_get_port();
BallDisplay_path = "models/graphics/build/BallDisplay.jar"

if (os.path.isfile(BallDisplay_path)) :
    BallDisplay_cmd = "java -jar " \
                   + BallDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(BallDisplay_cmd)
    os.system( BallDisplay_cmd)
else :
    print('==================================================================================')
    print('BallDisplay needs to be built. Please \"cd\" into ../models/graphics and type \"mvn package\".')
    print('==================================================================================')
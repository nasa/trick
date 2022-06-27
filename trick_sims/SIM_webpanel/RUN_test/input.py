exec(open("Modified_data/realtime.py").read())
web.server.enable = True
web.server.port = 8888
web.server.document_root = "www"
web.server.debug = False
web.server.ssl_enable = False
web.server.error_log_file = "civet_server_error.log"


dyn.balloon.pos[0] = 0
dyn.balloon.pos[1] = 0
dyn.balloon.vel[0] = 0.0
dyn.balloon.vel[1] = 0.0
dyn.balloon.envelope_air_temperature = 92.0
dyn.balloon.wind_speed = 0.0

# ==========================================
# Start the Satellite Graphics Client
# ==========================================
varServerPort = trick.var_server_get_port();
BalloonDisplay_path = "models/graphics/dist/BalloonDisplay.jar"

if (os.path.isfile(BalloonDisplay_path)) :
    BalloonDisplay_cmd = "java -jar " \
                   + BalloonDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(BalloonDisplay_cmd)
    os.system( BalloonDisplay_cmd);
else :
    print('==================================================================================')
    print('BalloonDisplay needs to be built. Please \"cd\" into ../models/graphics and type \"make\".')
    print('==================================================================================')

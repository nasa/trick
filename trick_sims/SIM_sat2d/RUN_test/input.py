exec(open("Modified_data/realtime.py").read())
exec(open("Modified_data/satellite.dr").read())

dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 6)

#==========================================
# Start the Satellite Graphics Client
#==========================================
varServerPort = trick.var_server_get_port();
SatDisplay_path = "models/graphics/dist/SatDisplay.jar"

if (os.path.isfile(SatDisplay_path)) :
    SatDisplay_cmd = "java -jar " \
                   + SatDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(SatDisplay_cmd)
    os.system( SatDisplay_cmd);
else :
    print('==================================================================================')
    print('SatDisplay needs to be built. Please \"cd\" into ../models/graphics and type \"make\".')
    print('==================================================================================')

trick.stop(5400);




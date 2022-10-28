exec(open("Modified_data/realtime.py").read())

dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 6)
dyn_integloop.set_integ_cycle(0.01)

dyn.aircraft.set_desired_compass_heading(45.0);
dyn.aircraft.autoPilot = True;
dyn.aircraft.desired_speed = 200 # meters per second

#==========================================
# Start the Satellite Graphics Client
#==========================================
varServerPort = trick.var_server_get_port();
AircraftDisplay_path = "models/graphics/dist/AircraftDisplay.jar"

if (os.path.isfile(AircraftDisplay_path)) :
    AircraftDisplay_cmd = "java -jar " \
                   + AircraftDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(AircraftDisplay_cmd)
    os.system( AircraftDisplay_cmd);
else :
    print('==================================================================================')
    print('AircraftDisplay needs to be built. Please \"cd\" into ../models/graphics and type \"make\".')
    print('==================================================================================')

trick.stop(5400);

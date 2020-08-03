exec(open("Modified_data/realtime.py").read())
exec(open("Modified_data/vehicleState.dr").read())

trick.TMM_reduced_checkpoint(0)

veh_integloop.getIntegrator(trick.Runge_Kutta_4, 8)

#==========================================
# Configure the Vehicle.
#==========================================
veh.vehicle.position = [0.0, 0.0]
veh.vehicle.heading = 0.0

veh.vehicle.distanceBetweenWheels = 0.183
veh.vehicle.wheelRadius = 0.045
veh.vehicle.wheelSpeedLimit = 8.880
veh.vehicle.headingRateLimit = 3.14159/4

veh.vehicle.slowDownDistance = 0.5
veh.vehicle.arrivalDistance  = 0.1

#==========================================
# Add the waypoints to the SIM.
# Set a home point by adding it as the last waypoint.
#==========================================
waypoints_path = "Modified_data/cross.waypoints"
fp = open(waypoints_path, "r")
for line in fp:
    fields = line.split(",")
    veh.vehicle.add_waypoint( float(fields[0]), float(fields[1]))

#==========================================
# Start the display VarServer Client
#==========================================
varServerPort = trick.var_server_get_port();
EVDisplay_path = "models/Graphics/dist/EVDisplay.jar"

if (os.path.isfile(EVDisplay_path)) :
    EVDisplay_cmd = "java -jar " \
                  + EVDisplay_path \
                  + " -v images/twoWheelRover.png" \
                  + " -w " + waypoints_path \
                  + " " + str(varServerPort) + " &" ;
    print(EVDisplay_cmd)
    os.system( EVDisplay_cmd);
else :
    print('==================================================================================')
    print('EVDisplay needs to be built. Please \"cd\" into models/Graphics and type \"make\".')
    print('==================================================================================')

#==========================================
# Start the display VarServer Client
#==========================================
varServerPort = trick.var_server_get_port();
HomeDisplay_path = "models/GUIControl1/dist/HomeDisplay.jar"

if (os.path.isfile(HomeDisplay_path)) :
    HomeDisplay_cmd = "java -jar " \
                  + HomeDisplay_path \
                  + " " + str(varServerPort) + " &" ;
    print(HomeDisplay_cmd)
    os.system( HomeDisplay_cmd);
else :
    print('==================================================================================')
    print('HomeDisplay needs to be built. Please \"cd\" into models/GUIControl1 and type \"make\".')
    print('==================================================================================')

trick.stop(100)

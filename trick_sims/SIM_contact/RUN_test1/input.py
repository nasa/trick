execfile("Modified_data/realtime.py")

# Variable Server Data should be copied at top of frame.
trick.var_set_copy_mode(2)

dyn.contact.nballs = 2
dyn.contact.balls = trick.TMM_declare_var_1d("Ball*", dyn.contact.nballs)

dyn.contact.balls[0] = trick.make_Ball(0.0, 0.0,  0.5, 0.0, 0.5, 1.0)
dyn.contact.balls[1] = trick.make_Ball(3.0, 0.0, -0.5, 0.0, 0.5, 1.0)

dyn_integloop.getIntegrator(trick.Euler, 2*dyn.contact.nballs)


#==========================================
# Start the Satellite Graphics Client
#==========================================
varServerPort = trick.var_server_get_port();
BallDisplay_path = "models/graphics/dist/BallDisplay.jar"

if (os.path.isfile(BallDisplay_path)) :
    BallDisplay_cmd = "java -jar " \
                   + BallDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(BallDisplay_cmd)
    os.system( BallDisplay_cmd);
else :
    print('==================================================================================')
    print('BallDisplay needs to be built. Please \"cd\" into ../models/graphics and type \"make\".')
    print('==================================================================================')

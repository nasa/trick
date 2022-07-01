

exec(open("./Modified_data/realtime.py").read())

dyn.table.numBalls = 2
dyn.table.balls = trick.TMM_declare_var_1d("Ball*", dyn.table.numBalls)

id1 = dyn.table.addBall(-2, .5, 1, 1, False);
id2 = dyn.table.addBall(1, .5, 1, 1, False);

dyn.table.setBallVel(id1, 0.5, 0);

dyn_integloop.getIntegrator(trick.Euler, 6*dyn.table.numBalls)

#==========================================
# Start the Graphics Client
#==========================================
varServerPort = trick.var_server_get_port();
PoolTableDisplay_path = "models/graphics/dist/PoolTableDisplay.jar"

if (os.path.isfile(PoolTableDisplay_path)) :
    PoolTableDisplay_cmd = "java -jar " \
                   + PoolTableDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(PoolTableDisplay_cmd)
    os.system( PoolTableDisplay_cmd);
else :
    print('============================================================================================')
    print('PoolTableDisplay needs to be built. Please \"cd\" into ../models/graphics and type \"make\".')
    print('============================================================================================')

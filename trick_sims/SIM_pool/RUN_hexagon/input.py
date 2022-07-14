import math

exec(open("./Modified_data/realtime.py").read())

dyn.table.numBalls = 4
dyn.table.balls = trick.TMM_declare_var_1d("Ball*", dyn.table.numBalls)

ballRadius = 0.02
ballMass = 1

unit_pos = [math.sqrt(3)/2, 0.5]
unit_neg = [unit_pos[0], -unit_pos[1]]
center_x = 0.2;
center_y = 0;
tol = 1e-4;

dyn.table.addBall(-.3, 0, ballMass, ballRadius, False)

dyn.table.addBall(center_x, center_y, ballMass, ballRadius, False)

dyn.table.addBall(center_x+unit_neg[0]*2*(ballRadius+tol), center_y+unit_neg[1]*2*(ballRadius+tol), ballMass, ballRadius, False)
dyn.table.addBall(center_x+unit_pos[0]*2*(ballRadius+tol), center_y+unit_pos[1]*2*(ballRadius+tol), ballMass, ballRadius, False)

# dyn.table.addBall(center_x+unit_neg[0]*4*(ballRadius+tol), center_y+unit_neg[1]*4*(ballRadius+tol), ballMass, ballRadius, False)
# dyn.table.addBall(center_x+unit_pos[0]*4*(ballRadius+tol), center_y+unit_pos[1]*4*(ballRadius+tol), ballMass, ballRadius, False)
# dyn.table.addBall(center_x+unit_pos[0]*4*(ballRadius+tol), center_y, ballMass, ballRadius, False)

# dyn.table.addBall(center_x+unit_neg[0]*6*(ballRadius+tol), center_y+unit_neg[1]*6*(ballRadius+tol), ballMass, ballRadius, False)
# dyn.table.addBall(center_x+unit_pos[0]*6*(ballRadius+tol), center_y+unit_pos[1]*6*(ballRadius+tol), ballMass, ballRadius, False)
# dyn.table.addBall(center_x+unit_neg[0]*6*(ballRadius+tol), center_y+unit_neg[1]*2*(ballRadius+tol), ballMass, ballRadius, False)
# dyn.table.addBall(center_x+unit_pos[0]*6*(ballRadius+tol), center_y+unit_pos[1]*2*(ballRadius+tol), ballMass, ballRadius, False)






# Make a Hexagonal table
corners = [ [1, 0],
            [1/2, math.sqrt(3)/2], 
            [-1/2, math.sqrt(3)/2],
            [-1, 0],
            [-1/2, -math.sqrt(3)/2],
            [1/2, -math.sqrt(3)/2]]

scale = 0.5

for i in range(len(corners)):
    corners[i][0] *= scale
    corners[i][1] *= scale

pocketRadius = 0.04
bumperWidth = 0.03


dyn.table.numTablePoints = 6
dyn.table.tableShape = trick.TMM_declare_var_1d("Point*", dyn.table.numTablePoints)
dyn.table.tableShapeType = 0  # generic
for corner in corners:
    dyn.table.addPointToTable(corner[0], corner[1])

# Pockets - Just put 1 in the center
dyn.table.numPockets = 1
dyn.table.pockets = trick.TMM_declare_var_1d("Pocket*", dyn.table.numPockets)
dyn.table.addPocket(0, 0, pocketRadius)
# for coord in corners:
#     dyn.table.addPocket(coord[0], coord[1], pocketRadius)

# dyn.table.numBumpers = 0
dyn.table.numBumpers = len(corners)
dyn.table.bumpers = trick.TMM_declare_var_1d("Bumper*", dyn.table.numBumpers)

# Put bumpers along each edge
for i in range(len(corners)):
    p1 = [corners[i][0], corners[i][1]]
    p2 = [corners[(i+1) % 6][0], corners[(i+1) % 6][1]]
    id = dyn.table.addBumper(3, p1[0], p1[1], p2[0], p2[1])
    dyn.table.bumpers[id][0].shapeType = 2 # Triangle i guess?
    dyn.table.bumpers[id][0].numPoints = 3
    dyn.table.bumpers[id][0].renderedShape = trick.TMM_declare_var_1d("Point*", dyn.table.bumpers[id].numPoints)
    dyn.table.addPointToBumper(id, p1[0], p1[1])
    dyn.table.addPointToBumper(id, p2[0], p2[1])
    p3 = [(p1[0] + p2[0])/2,(p1[1] + p2[1])/2]
    dyn.table.addPointToBumper(id, p3[0], p3[1])
    
dyn_integloop.getIntegrator(trick.Euler, 6*dyn.table.numBalls)

#==========================================
# Start the Graphics Client
#==========================================
varServerPort = trick.var_server_get_port();

# This will definitely change to something else
PoolTableDisplay_path = "models/graphics/libigl-example-project/build/example"


if (os.path.isfile(PoolTableDisplay_path)) :
    PoolTableDisplay_cmd = PoolTableDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(PoolTableDisplay_cmd)
    os.system( PoolTableDisplay_cmd);
else :
    print('=================================================================================================')
    print('PoolTableDisplay needs to be built. Please \"cd\" into ../models/graphics/java and type \"make\".')
    print('=================================================================================================')


# PoolTableDisplay_path = "models/graphics/java/dist/PoolTableDisplay.jar"

# if (os.path.isfile(PoolTableDisplay_path)) :
#     PoolTableDisplay_cmd = "java -jar " \
#                    + PoolTableDisplay_path \
#                    + " " + str(varServerPort) + " &" ;
#     print(PoolTableDisplay_cmd)
#     os.system( PoolTableDisplay_cmd);
# else :
#     print('=================================================================================================')
#     print('PoolTableDisplay needs to be built. Please \"cd\" into ../models/graphics/java and type \"make\".')
#     print('=================================================================================================')

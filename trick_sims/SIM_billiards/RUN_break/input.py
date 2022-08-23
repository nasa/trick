import math

exec(open("./Modified_data/realtime.py").read())

dyn.table.numBalls = 16
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

dyn.table.addBall(center_x+unit_neg[0]*4*(ballRadius+tol), center_y+unit_neg[1]*4*(ballRadius+tol), ballMass, ballRadius, False)
dyn.table.addBall(center_x+unit_pos[0]*4*(ballRadius+tol), center_y+unit_pos[1]*4*(ballRadius+tol), ballMass, ballRadius, False)
dyn.table.addBall(center_x+unit_pos[0]*4*(ballRadius+tol), center_y, ballMass, ballRadius, False)

dyn.table.addBall(center_x+unit_neg[0]*6*(ballRadius+tol), center_y+unit_neg[1]*6*(ballRadius+tol), ballMass, ballRadius, False)
dyn.table.addBall(center_x+unit_pos[0]*6*(ballRadius+tol), center_y+unit_pos[1]*6*(ballRadius+tol), ballMass, ballRadius, False)
dyn.table.addBall(center_x+unit_neg[0]*6*(ballRadius+tol), center_y+unit_neg[1]*2*(ballRadius+tol), ballMass, ballRadius, False)
dyn.table.addBall(center_x+unit_pos[0]*6*(ballRadius+tol), center_y+unit_pos[1]*2*(ballRadius+tol), ballMass, ballRadius, False)

dyn.table.addBall(center_x+unit_neg[0]*8*(ballRadius+tol), center_y+unit_neg[1]*8*(ballRadius+tol), ballMass, ballRadius, False)
dyn.table.addBall(center_x+unit_pos[0]*8*(ballRadius+tol), center_y+unit_pos[1]*8*(ballRadius+tol), ballMass, ballRadius, False)
dyn.table.addBall(center_x+unit_neg[0]*8*(ballRadius+tol), center_y+unit_neg[1]*4*(ballRadius+tol), ballMass, ballRadius, False)
dyn.table.addBall(center_x+unit_pos[0]*8*(ballRadius+tol), center_y+unit_pos[1]*4*(ballRadius+tol), ballMass, ballRadius, False)
dyn.table.addBall(center_x+unit_pos[0]*8*(ballRadius+tol), center_y, ballMass, ballRadius, False)

corners = [-.5, -.25, .5, .25]



# Make a normal pool table
pocketRadius = 0.04
bumperWidth = 0.03


dyn.table.numTablePoints = 2
dyn.table.tableShape = trick.TMM_declare_var_1d("Vec*", dyn.table.numTablePoints)
dyn.table.tableShapeType = 3  # rectangle
dyn.table.addPointToTable(corners[0], corners[1])
dyn.table.addPointToTable(corners[2], corners[3])

# Pockets
dyn.table.numPockets = 6
dyn.table.pockets = trick.TMM_declare_var_1d("Pocket*", dyn.table.numPockets)
pocketCenters = [[0, corners[1]],
                 [0, corners[3]],
                 [corners[0],corners[1]],
                 [corners[0],corners[3]],
                 [corners[2],corners[1]],
                 [corners[2],corners[3]] ]

for coord in pocketCenters:
    dyn.table.addPocket(coord[0], coord[1], pocketRadius)


bumperBorders =[[corners[0]+pocketRadius+bumperWidth, corners[1]+bumperWidth, 0.0-pocketRadius-bumperWidth,corners[1]+bumperWidth],         # Bottom left
                [0.0+pocketRadius+bumperWidth, corners[1]+bumperWidth, corners[2]-pocketRadius-bumperWidth,corners[1]+bumperWidth],         # Bottom Right
                [corners[0]+pocketRadius+bumperWidth, corners[3]-bumperWidth, 0.0-pocketRadius-bumperWidth,corners[3]-bumperWidth],         # Top left
                [0.0+pocketRadius+bumperWidth, corners[3]-bumperWidth, corners[2]-pocketRadius-bumperWidth,corners[3]-bumperWidth],         # Top right
                [corners[0]+bumperWidth, corners[1]+pocketRadius+bumperWidth, corners[0]+bumperWidth, corners[3]-pocketRadius-bumperWidth], # Left
                [corners[2]-bumperWidth, corners[1]+pocketRadius+bumperWidth, corners[2]-bumperWidth, corners[3]-pocketRadius-bumperWidth]] # Right

# triangles
bumperBorders.extend([[bumperBorders[0][0]-bumperWidth, bumperBorders[0][1]-bumperWidth, bumperBorders[0][0], bumperBorders[0][1]],         # Bottom left
                      [bumperBorders[0][2], bumperBorders[0][1], bumperBorders[0][2]+bumperWidth, bumperBorders[0][1]-bumperWidth],
                      [bumperBorders[1][0]-bumperWidth, bumperBorders[1][1]-bumperWidth, bumperBorders[1][0], bumperBorders[1][1]],         # Bottom Right
                      [bumperBorders[1][2], bumperBorders[1][1], bumperBorders[1][2]+bumperWidth, bumperBorders[1][1]-bumperWidth],
                      [bumperBorders[2][0]-bumperWidth, bumperBorders[2][1]+bumperWidth, bumperBorders[2][0], bumperBorders[2][1]],         # Top left
                      [bumperBorders[2][2], bumperBorders[2][1], bumperBorders[2][2]+bumperWidth, bumperBorders[2][1]+bumperWidth],
                      [bumperBorders[3][0]-bumperWidth, bumperBorders[3][1]+bumperWidth, bumperBorders[3][0], bumperBorders[3][1]],         # Top right
                      [bumperBorders[3][2], bumperBorders[3][1], bumperBorders[3][2]+bumperWidth, bumperBorders[3][1]+bumperWidth],
                      [bumperBorders[4][0], bumperBorders[4][1], bumperBorders[4][2]-bumperWidth, bumperBorders[4][1]-bumperWidth],         # Left
                      [bumperBorders[4][0]-bumperWidth, bumperBorders[4][3]+bumperWidth, bumperBorders[4][2], bumperBorders[4][3]],
                      [bumperBorders[5][0], bumperBorders[5][1], bumperBorders[5][2]+bumperWidth, bumperBorders[5][1]-bumperWidth],         # Left
                      [bumperBorders[5][0]+bumperWidth, bumperBorders[5][3]+bumperWidth, bumperBorders[5][2], bumperBorders[5][3]],
                      ])

                

bumperShapes = [    [bumperBorders[0][0], bumperBorders[0][1]-bumperWidth, bumperBorders[0][2], bumperBorders[0][3]],
                    [bumperBorders[1][0], bumperBorders[1][1]-bumperWidth, bumperBorders[1][2], bumperBorders[1][3]],
                    [bumperBorders[2][0], bumperBorders[2][1]+bumperWidth, bumperBorders[2][2], bumperBorders[2][3]],
                    [bumperBorders[3][0], bumperBorders[3][1]+bumperWidth, bumperBorders[3][2], bumperBorders[3][3]],
                    [bumperBorders[4][0], bumperBorders[4][1], bumperBorders[4][2]-bumperWidth, bumperBorders[4][3],],
                    [bumperBorders[5][0]+bumperWidth, bumperBorders[5][1], bumperBorders[5][2], bumperBorders[5][3],],
                    [bumperBorders[6][0], bumperBorders[6][1], bumperBorders[6][2], bumperBorders[6][3], bumperBorders[6][2], bumperBorders[6][1]],  
                    [bumperBorders[7][0], bumperBorders[7][1], bumperBorders[7][2], bumperBorders[7][3], bumperBorders[7][0], bumperBorders[7][3]],
                    [bumperBorders[8][0], bumperBorders[8][1], bumperBorders[8][2], bumperBorders[8][3], bumperBorders[8][2], bumperBorders[8][1]],
                    [bumperBorders[9][0], bumperBorders[9][1], bumperBorders[9][2], bumperBorders[9][3], bumperBorders[9][0], bumperBorders[9][3]],
                    [bumperBorders[10][0], bumperBorders[10][1], bumperBorders[10][2], bumperBorders[10][3], bumperBorders[10][2], bumperBorders[10][1]],
                    [bumperBorders[11][0], bumperBorders[11][1], bumperBorders[11][2], bumperBorders[11][3], bumperBorders[11][0], bumperBorders[11][3]],
                    [bumperBorders[12][0], bumperBorders[12][1], bumperBorders[12][2], bumperBorders[12][3], bumperBorders[12][2], bumperBorders[12][1]],
                    [bumperBorders[13][0], bumperBorders[13][1], bumperBorders[13][2], bumperBorders[13][3], bumperBorders[13][0], bumperBorders[13][3]],
                    [bumperBorders[14][0], bumperBorders[14][1], bumperBorders[14][2], bumperBorders[14][3], bumperBorders[14][2], bumperBorders[14][1]],
                    [bumperBorders[15][0], bumperBorders[15][1], bumperBorders[15][2], bumperBorders[15][3], bumperBorders[15][0], bumperBorders[15][3]],
                    [bumperBorders[16][0], bumperBorders[16][1], bumperBorders[16][2], bumperBorders[16][3], bumperBorders[16][2], bumperBorders[16][1]],
                    [bumperBorders[17][0], bumperBorders[17][1], bumperBorders[17][2], bumperBorders[17][3], bumperBorders[17][0], bumperBorders[17][3]]]

bumperShapeTypes = [3, 3, 3, 3, 3, 3, 2, 2,2,2,2, 2, 2, 2,2,2, 2, 2]


bumperNum = len(bumperBorders)
print("Num bumpers: ", len(bumperBorders))
dyn.table.numBumpers = bumperNum
dyn.table.bumpers = trick.TMM_declare_var_1d("Bumper*", dyn.table.numBumpers)

for i in range(bumperNum):
    numPoints = len(bumperShapes[i])/2
    id = dyn.table.addBumper(numPoints, bumperBorders[i][0], bumperBorders[i][1], bumperBorders[i][2],bumperBorders[i][3])
    dyn.table.bumpers[id][0].numPoints = numPoints
    dyn.table.bumpers[id][0].renderedShape = trick.TMM_declare_var_1d("Vec*", numPoints)

    dyn.table.bumpers[id][0].shapeType = bumperShapeTypes[i]
    for j in range(0, len(bumperShapes[i]), 2):
        dyn.table.addPointToBumper(id, bumperShapes[i][j],bumperShapes[i][j+1])


dyn_integloop.getIntegrator(trick.Euler, 6*dyn.table.numBalls)

#==========================================
# Start the Graphics Client
#==========================================
varServerPort = trick.var_server_get_port();

PoolTableDisplay_path = "models/graphics/cpp/build/PoolTableDisplay"


if (os.path.isfile(PoolTableDisplay_path)) :
    PoolTableDisplay_cmd = PoolTableDisplay_path \
                   + " " + str(varServerPort) + " &" ;
    print(PoolTableDisplay_cmd)
    os.system( PoolTableDisplay_cmd);
else :
    print('===================================')
    print('PoolTableDisplay needs to be built.')
    print('===================================')

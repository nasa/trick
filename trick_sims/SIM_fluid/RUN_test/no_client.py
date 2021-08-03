import math
exec(open("Modified_data/realtime.py").read())
dyn.fluid.BOUND = 380
dyn.fluid.ISO_RADIUS = 32

for i in range(1600):
    tx = i / 40 - 20
    tz = i % 40 - 20
    dyn.fluid.particlesArr[i].pos[0] = tx * 4
    dyn.fluid.particlesArr[i].pos[2] = tz * 4 
    dyn.fluid.particlesArr[i].pos[1] = tx * tx + tz * tz

"""
for i in range(800):
    t = i / 2
    dyn.fluid.particlesArr[i].pos[1] = 15 * math.cos(math.radians(t))
"""
"""
for i in range(800):
    theta = (360 / 400) * i
    print(theta)
    dyn.fluid.particlesArr[i].pos[0] = 100 * math.cos(math.radians(theta))
    dyn.fluid.particlesArr[i].pos[1] = 100 * math.sin(math.radians(theta))
    if i > 400 and i <= 700:
        theta = (360 / 300) * i
        dyn.fluid.particlesArr[i].pos[0] = 70 * math.cos(math.radians(theta))
        dyn.fluid.particlesArr[i].pos[1] = 70 * math.sin(math.radians(theta))
    if i > 700:
        theta = (360 / 100) * i
        dyn.fluid.particlesArr[i].pos[0] = 40 * math.cos(math.radians(theta))
        dyn.fluid.particlesArr[i].pos[1] = 40 * math.sin(math.radians(theta))
"""
import os

exec(open("Modified_data/realtime.py").read())
#exec(open("Modified_data/Satellite.dr").read())

dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 18)

scene_app_path = 'models/Satellite/graphics/Scene'
if os.path.exists(scene_app_path):
    dyn.sat_graph_comm.syscmd = scene_app_path + ' &'
else:
    dyn.sat_graph_comm.syscmd = '';
    sys.stderr.write(scene_app_path + ' not found. It probably needs to be complied.\n')
    sys.exit(1)

# Fire Thrusters at the given times.
read = 10
trick.add_read(read,"""dyn.satellite.thruster_T1.on = True""")

read = 20
trick.add_read(read,"""dyn.satellite.thruster_T1.on = False""")
trick.add_read(read,"""dyn.satellite.thruster_T3.on = True""")

read = 30
trick.add_read(read,"""dyn.satellite.thruster_T3.on = False""")

read = 50
trick.add_read(read,"""dyn.satellite.thruster_T3.on = True""")

read = 60
trick.add_read(read,"""dyn.satellite.thruster_T3.on = False""")
trick.add_read(read,"""dyn.satellite.thruster_T1.on = True""")

read = 70
trick.add_read(read,"""dyn.satellite.thruster_T1.on = False""")

trick.stop(5400)

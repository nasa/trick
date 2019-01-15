#!/usr/bin/env python
import matplotlib.pyplot as plt
import numpy as np

#
data = np.genfromtxt('../RUN_test/log_Rocket.csv',
                     delimiter=',',
                     skip_header=1,
                     skip_footer=1,
                     names=['t',
                            'vux', 'vuy',
                            'Fthrustx', 'Fthrusty',
                            'Fdragx', 'Fdragy',
                            'Fgravx','Fgravy',
                            'Ftotalx','Ftotaly',
                            'Mt',
                            'MassTotal', 'MassRate',
                            'Posx','Posy',
                            'velx','vely',
                            'Cd', 'MassEmpty' ],
                     dtype=(float, float)
                    )

curve1 = plt.plot(data['t'], data['Fthrusty'], 'C1-')
curve2 = plt.plot(data['t'], data['Fdragy'], 'C2-')
curve3 = plt.plot(data['t'], data['Fgravy'], 'C3-')
curve4 = plt.plot(data['t'], data['Ftotaly'], 'C4-')
plt.legend(('F_thrust', 'F_drag', 'F_grav', 'F_total'))
plt.title('Rockets Forces Over Time')
plt.xlabel('time (s)')
plt.ylabel('force (N)')
plt.grid(True)
plt.show()

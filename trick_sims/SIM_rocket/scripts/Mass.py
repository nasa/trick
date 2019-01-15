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

plt.plot(data['t'], data['MassTotal'], 'r-')
plt.title('Rocket Mass')
plt.xlabel('time (s)')
plt.ylabel('Mass (kg)')
plt.grid(True)
plt.show()

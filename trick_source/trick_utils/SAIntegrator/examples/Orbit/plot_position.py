#!/usr/bin/env python

import matplotlib.pyplot as plt
import numpy as np
data = np.genfromtxt('orbit.csv',
                     delimiter=',',
                     skip_header=1,
                     skip_footer=1,
                     names=['t', 'posx','posy','velx','vely'],
                     dtype=(float, float, float, float, float)
                    )

curve1 = plt.plot(data['posx'], data['posy'], 'C1-')
plt.title('Orbit')
plt.xlabel('position')
plt.ylabel('position')
plt.grid(True)
plt.show()

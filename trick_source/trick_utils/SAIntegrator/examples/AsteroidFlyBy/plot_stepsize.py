#!/usr/bin/env python

import matplotlib.pyplot as plt
import numpy as np
data = np.genfromtxt('flyby.csv',
                     delimiter=',',
                     skip_header=1,
                     skip_footer=1,
                     names=['t', 'dt', 'posx','posy','velx','vely'],
                     dtype=(float, float, float, float, float, float)
                    )

curve1 = plt.plot(data['t'], data['dt'], 'C1-')
plt.title('Time-step Adaptation')
plt.xlabel('t')
plt.ylabel('dt')
plt.grid(True)
plt.show()

#!/usr/bin/env python

import matplotlib.pyplot as plt
import numpy as np
data = np.genfromtxt('msd.csv',
                     delimiter=',',
                     skip_header=1,
                     skip_footer=1,
                     names=['t', 'pos', 'vel'],
                     dtype=(float, float, float)
                    )

curve1 = plt.plot(data['t'], data['pos'], 'C1-')
plt.title('MSD Position')
plt.xlabel('position')
plt.ylabel('time')
plt.grid(True)
plt.show()

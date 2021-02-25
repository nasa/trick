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

curve1 = plt.plot(data['posx'], data['posy'], 'C1-')
plt.title('Flyby')
plt.xlabel('position')
plt.ylabel('position')
plt.grid(True)
plt.show()

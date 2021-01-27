#!/usr/bin/env python
  
import matplotlib.pyplot as plt
import numpy as np
data = np.genfromtxt('cannon.csv',
                     delimiter=',',
                     skip_header=1,
                     skip_footer=1,
                     names=['t', 'px', 'py', 'vx', 'vx'],
                     dtype=(float, float, float, float, float)
                    )

curve1 = plt.plot(data['px'], data['py'], 'C1-')
plt.title('Cannonball Trajectory')
plt.xlabel('pos-x')
plt.ylabel('pos-y')
plt.grid(True)
plt.show()

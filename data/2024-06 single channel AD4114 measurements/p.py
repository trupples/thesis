import matplotlib
matplotlib.use('TkAgg')

import matplotlib.pyplot as plt
from scipy import signal
import numpy as np
from meas7 import v

fs = 1007
fn = 50.35
Q = 5
b,a = signal.iircomb(fn / fs * 2, Q)

n = len(v)
t = np.array(range(0, n)) / fs
vv = signal.filtfilt(b,a,v)

plt.plot(t, vv, linewidth=0.5)
plt.grid()
plt.show()


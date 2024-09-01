#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
from adi.attribute import attribute
from adi.context_manager import context_manager
from adi.rx_tx import rx
from scipy import signal, io
from datetime import datetime

class ad4114exg(rx, context_manager):
    _complex_data = False
    channel = []
    _device_name = ""

    def __init__(self, uri="", device_name=""):
        context_manager.__init__(self, uri, self._device_name)

        compatible_part = "ad4114-exg"
        self._ctrl = None

        if not device_name:
            device_name = compatible_part
        else:
            if device_name != compatible_part:
                raise Exception(f"Not a compatible device: {device_name}")

        # Select the device matching device_name as working device
        for device in self._ctx.devices:
            if device.name == device_name:
                self._ctrl = device
                self._rxadc = device
                break

        if not self._ctrl:
            raise Exception("Error in selecting matching device")

        if not self._rxadc:
            raise Exception("Error in selecting matching device")

        for ch in self._ctrl.channels:
            name = ch._id
            self._rx_channel_names.append(name)
            self.channel.append(self._channel(self._ctrl, name))

        rx.__init__(self)

    class _channel(attribute):
        def __init__(self, ctrl, channel_name):
            self.name = channel_name
            self._ctrl = ctrl

        @property
        def raw(self):
            return self._get_iio_attr(self.name, "raw", False)

        @property
        def scale(self):
            return float(self._get_iio_attr(self.name, "scale", False))

        @property
        def offset(self):
            return float(self._get_iio_attr(self.name, "offset", False))

d = ad4114exg('serial:/dev/ttyACM0,500000,8n1')

print(d._set_iio_attr("voltage0", "input", False, "VIN0-VIN1"))
print(d._get_iio_attr("voltage0", "input", False))

print(d._set_iio_attr("voltage5", "input", False, "VIN3-VIN2"))
print(d._get_iio_attr("voltage5", "input", False))

scale = d.channel[0].scale
offset = d.channel[0].offset
print(scale, offset, type(scale), type(offset))

d.rx_enabled_channels = [0, 5]
n = 128 * 8
d.rx_buffer_size = n


fs = 1007
fn = 50.35
Q = 5
b,a = signal.iircomb(fn / fs * 2, Q)

ch0_z = signal.lfilter_zi(b, a)
ch1_z = signal.lfilter_zi(b, a)

t = np.array(range(0, n)) / fs

ch0_all = []
ch1_all = []

line0 = None

for i in range(5):
    [ch0, ch1] = d.rx()
    ch0 = (ch0 - offset) * scale
    ch1 = (ch1 - offset) * scale
    #ch0 = (d.rx() - offset) * scale
    #ch1 = ch0

    #[ch0, ch0_z] = signal.lfilter(b,a,ch0, zi=ch0_z)
    #[ch1, ch1_z] = signal.lfilter(b,a,ch1, zi=ch1_z)

    print(ch0, ch1)
    # print("-#"[ra[i]] * 2000)

    ch0_all.extend(ch0)
    ch1_all.extend(ch1)

ch0_filt = signal.lfilter(b,a,ch0_all)
ch1_filt = signal.lfilter(b,a,ch1_all)

#lpf = signal.bessel(6, 150 / fs * 2, norm='delay', output='sos')
#ch0_filt = signal.sosfilt(lpf, ch0_filt)

#io.savemat(f"{datetime.now()}.mat", {
#    'i': ra,
#    'v0': ch0_all,
#    'f0': ch0_filt,
#    'v1': ch1_all,
#    'f1': ch1_filt
#})

line0, = plt.plot(ch0_filt, 'r-')
line1, = plt.plot(ch1_filt, 'b-')

plt.show()


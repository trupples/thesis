from adi.attribute import attribute
from adi.context_manager import context_manager
from adi.rx_tx import rx

class ad4114exg_iio(rx, context_manager):
    _complex_data = False
    channel = []
    _device_name = ""

    def __init__(self, uri="", device_name="ad4114-exg"):
        context_manager.__init__(self, uri, self._device_name)

        compatible_part = "ad4114-exg"
        self._ctrl = None

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

    def start(self, inputs):
        for i, inp in enumerate(inputs):
            self._set_iio_attr(f"voltage{i}", "input", False, inp)

        nchan = len(inputs)
        self.rx_buffer_size = nchan * 16
        self.rx_enabled_channels = list(range(nchan))

    def stop(self):
        raise NotImplementedError()

    @property
    def sample_rate(self):
        return self._ctrl.attrs['sampling_frequency'].value

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



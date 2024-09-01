import rclpy
from rclpy.node import Node
from rclpy.time import Time
from ad4114exg_interfaces.msg import ExgSample
from threading import Thread
from queue import Queue
from time import time
from ad4114exg_iio import ad4114exg_iio

class AD4114ExgPublisher(Node):
    def __init__(self, uri, channels):
        super().__init__('ad4114exg')

        self.publisherz = [
            self.create_publisher(ExgSample, f'ch{i}', 10)
            for i in range(16)
        ]

        self.iiodev = ad4114exg_iio(uri)
        self.buffer = Queue()
        self.timer = self.create_timer(1e-3, self.publish_buffered_samples)

        self.running = True
        thr = Thread(target=self.receive_thread, args=[channels])
        thr.daemon = True
        thr.start()

    def publish_buffered_samples(self):
        while not self.buffer.empty():
            (timestamp, channel, value) = self.buffer.get()

            msg = ExgSample()
            msg.header.stamp = Time(seconds=timestamp).to_msg()
            msg.value = value
            self.publisherz[channel].publish(msg)

    def receive_thread(self, channels):
        self.iiodev.start(channels)

        t0 = time()
        while self.running:
            data = self.iiodev.rx()
            t1 = time()
            if len(channels) == 1:
                data = [data]

            N = data[0].size
            print(t1, t1-t0, N)
            for i, chan in enumerate(channels):
                for j, value in enumerate(data[i]):
                    value = (value - 2**23) * 3e-6
                    t = t0 + (t1 - t0) * j / N
                    self.buffer.put((t, i, value))

            t0 = t1

def main(args=None):
    print(args)

    rclpy.init(args=args)

    uri = 'serial:/dev/ttyACM0,500000,8n1'
    uri = 'ip:172.17.0.1'
    channels = [f"VIN0-VINCOM" for i in range(16)]
    node = AD4114ExgPublisher(uri, channels)
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()

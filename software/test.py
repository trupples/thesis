from ad4114exg_iio import ad4114exg_iio

# Connect to libIIO device based on URI
uri = 'serial:/dev/ttyACM0,500000'
dev = ad4114exg_iio(uri)

# Start acquisition of the following inputs
dev.start(['VIN13-VINCOM', 'VIN2-VIN3', 'VIN3-VIN2'])

# Query sample rate, determined based on the number of inputs
fs = dev.sample_rate
print(fs)

# Receive 100 windows of measured data and print them in real time
for i in range(100):
	window = dev.rx()

	print(window)

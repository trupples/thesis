from ad4114exg_iio import ad4114exg_iio

uri = 'serial:/dev/ttyACM0,500000,8n1'
iiodev = ad4114exg_iio(uri)

iiodev.start(['VIN13-VINCOM', 'VIN2-VIN3', 'VIN3-VIN2'])

fs = iiodev.sample_rate
print(fs)

for i in range(100):
	data = iiodev.rx()

	print(data)

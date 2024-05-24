#include "parameters.h"
#include "no_os_uart.h"
#include "maxim_uart.h"
#include "no_os_spi.h"
#include "maxim_spi.h"
#include "ad7124.h"
#include "ad7124_regs.h"

#ifdef IIO_SUPPORT
#include "iio_ad7124_exg.h"
#include "iio_app.h"
#endif

char _iio_ad7124_read_buf[1024];

int main()
{
    int status;

    // Set up UART
    struct max_uart_init_param uart_extra_init = {
        .flow = UART_FLOW_DIS
    };

	struct no_os_uart_init_param uart_init = {
        .device_id = UART_DEVICE_ID,
        .baud_rate = UART_BAUDRATE,
        .size = NO_OS_UART_CS_8,
        .platform_ops = UART_OPS,
        .parity = NO_OS_UART_PAR_NO,
        .stop = NO_OS_UART_STOP_1_BIT,
        .extra = &uart_extra_init,
	};

    // Set up UART stdio
    struct no_os_uart_desc *uart;
	status = no_os_uart_init(&uart, &uart_init);
    if(status != 0)
    {
        printf("no_os_uart_init failed with status %d\r\n", status);
        return status;
    }

	no_os_uart_stdio(uart);

    printf("Hello, world!\r\n");

    // Set up SPI
    struct max_spi_init_param maxim_spi_init = {
        .num_slaves = 1,
        .polarity = SPI_SS_POL_LOW,
        .vssel = MXC_GPIO_VSSEL_VDDIOH,
    };

	struct no_os_spi_init_param spi_init = {
		.chip_select = 1, // SS1 pin P0_11, incorrectly labelled as SS0 on the datasheet
		.max_speed_hz = 1000000,
		.mode = NO_OS_SPI_MODE_3,
		.device_id = 1, // MXC_SPI_GET_SPI(1) = MXC_SPI0
		.platform_ops = &max_spi_ops,
		.extra = &maxim_spi_init
	};

    // Set up AD7124
	struct ad7124_init_param ad7124_init = {
		.spi_init = &spi_init,
		.regs = ad7124_init_regs_default,
		.spi_rdy_poll_cnt = 1000
	};

    struct ad7124_dev *ad7124;
	status = ad7124_setup(&ad7124, &ad7124_init);
    if(status != 0)
    {
        printf("ad7124_setup failed with status %d\r\n", status);
        return status;
    }

#ifdef IIO_SUPPORT
    // Tear down UART stdio
    fflush(stdout);
    no_os_uart_remove(uart);

    // Set up IIO
    struct iio_data_buffer iio_ad7124_read_buf = {
		.buff = _iio_ad7124_read_buf,
		.size = sizeof(_iio_ad7124_read_buf),
	};

    struct iio_app_device devices[] = {
        IIO_APP_DEVICE("ad7124-8-exg", ad7124, &iio_ad7124_exg, &iio_ad7124_read_buf, NULL, NULL)
    };

    struct iio_app_desc *app;
	struct iio_app_init_param app_init = { 0 };

    app_init.devices = devices;
	app_init.nb_devices = 1;
	app_init.uart_init_params = uart_init;

    status = iio_app_init(&app, app_init);
    if (status != 0)
    {
        return status;
    }

    return iio_app_run(app);

#else

    // Console only logic
    printf("iio disabled, running in console-only mode\r\n");

#endif // IIO_SUPPORT
}
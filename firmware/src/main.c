#undef IIO_SUPPORT

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

    for(int i = 0; i < 100; i++)
    {
        printf(".");
        no_os_mdelay(50);
        fflush(stdout);
    }
    printf("\n");
    
    struct ad7124_dev *ad7124;
	status = ad7124_setup(&ad7124, &ad7124_init);
    if(status != 0)
    {
        printf("ad7124_setup failed with status %d\r\n", status);
        return status;
    }

    ad7124_set_power_mode(ad7124, AD7124_HIGH_POWER);
    ad7124_reg_write_msk(ad7124, AD7124_ADC_CTRL_REG, AD7124_ADC_CTRL_REG_DATA_STATUS, 0);

    struct ad7124_analog_inputs ain = { .ainp = AD7124_AIN0, .ainm = AD7124_AIN1 };
    ad7124_connect_analog_input(ad7124, 0, ain);
    ad7124_assign_setup(ad7124, 0, 0); // setup 0 for channel 0
    ad7124_set_polarity(ad7124, true, 0); // bipolar
    ad7124_set_reference_source(ad7124, INTERNAL_REF, 0, true); // use internal ref with setup 0
    ad7124_enable_buffers(ad7124, true, true, 0); // Enable both input and reference buffer for setup 0
    
    // Set PGA0 to 128x
    ad7124_reg_write_msk(ad7124, AD7124_CFG0_REG, AD7124_CFG_REG_PGA(7), 0);

    ad7124_set_channel_status(ad7124, 0, true); // enable channel 0

    while(true)
    {
        fflush(stdout);
        int32_t val;
        status = ad7124_read_data(ad7124, &val);
        if(status)
        {
            printf("Error: %d\n");
        }
        int32_t statusreg = ad7124->regs[AD7124_Status].value;
        printf("%2d %9d ", statusreg&0xf, val);

        printf("\n");
        fflush(stdout);
    }

#endif // IIO_SUPPORT
}
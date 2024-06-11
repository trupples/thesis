#undef IIO_SUPPORT

#include "parameters.h"
#include "no_os_uart.h"
#include "maxim_uart.h"
#include "no_os_spi.h"
#include "maxim_spi.h"
#include "no_os_delay.h"
// #include "ad4114.h"
// #include "ad4114_regs.h"


/* Communication Register bits */
#define AD4114_COMM_REG_WEN    (0 << 7)
#define AD4114_COMM_REG_WR     (0 << 6)
#define AD4114_COMM_REG_RD     (1 << 6)
#define AD4114_COMM_REG_RA(x)  ((x) & 0x3F)



#ifdef IIO_SUPPORT
#include "iio_ad4114_exg.h"
#include "iio_app.h"
#endif

char _iio_ad4114_read_buf[1024];

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

    // Set up AD4114
	// struct ad4114_init_param ad4114_init = {
	// 	.spi_init = &spi_init,
	// 	.regs = ad4114_init_regs_default,
	// 	.spi_rdy_poll_cnt = 1000
	// };

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

    for(int i = 0; i < 10; i++)
    {
        printf(".");
        no_os_mdelay(50);
        fflush(stdout);
    }
    printf("\n");
    
    printf("The AD4114 doesn't yet have no-OS support so we're doing this from scratch!\r\n");

    struct no_os_spi_desc *spi;
    int ret = no_os_spi_init(&spi, &spi_init);
    if(ret < 0)
    {
        printf("Error in no_os_spi_init!\r\n");
        return 1;
    }
    printf("no_os_spi_init!\r\n");

    uint8_t buf[8] = { 0 };

    // Read ID

    buf[0] = AD4114_COMM_REG_WEN | AD4114_COMM_REG_RD | AD4114_COMM_REG_RA(7);

    ret = no_os_spi_write_and_read(spi, buf, 4);
    if (ret < 0)
    {
        printf("Error in no_os_spi_write_and_read!\r\n");
        return 1;
    }
    printf("no_os_spi_write_and_read!\r\n");

    printf("Read from REG_ID: %02x %02x %02x %02x\r\n", buf[0], buf[1], buf[2], buf[3]);

    // adc mode
    buf[0] = AD4114_COMM_REG_WEN | AD4114_COMM_REG_WR | AD4114_COMM_REG_RA(0x01);
    buf[1] = 0b10100000; // internal reference, sincly_cyc, 0 delay
    buf[2] = 0b00000000; // continuous conversion, internal oscillator
    ret = no_os_spi_write_and_read(spi, buf, 3);
    if (ret < 0)
    {
        printf("Error in no_os_spi_write_and_read!\r\n");
        return 1;
    }
    
    // setup 0
    buf[0] = AD4114_COMM_REG_WEN | AD4114_COMM_REG_WR | AD4114_COMM_REG_RA(0x20);
    buf[1] = 0b00011111; // bipolar, buffer everything
    buf[2] = 0b00100000; // internal reference
    ret = no_os_spi_write_and_read(spi, buf, 3);
    if (ret < 0)
    {
        printf("Error in no_os_spi_write_and_read!\r\n");
        return 1;
    }

    // filter 0
    buf[0] = AD4114_COMM_REG_WEN | AD4114_COMM_REG_WR | AD4114_COMM_REG_RA(0x28);
    buf[1] = 0b00000000; 
    buf[2] = 0b00001010; // 1007 SPS
    ret = no_os_spi_write_and_read(spi, buf, 3);
    if (ret < 0)
    {
        printf("Error in no_os_spi_write_and_read!\r\n");
        return 1;
    }

    // channel 0
    buf[0] = AD4114_COMM_REG_WEN | AD4114_COMM_REG_WR | AD4114_COMM_REG_RA(16);
    buf[1] = 0b10000000; // enable, setup 0
    //buf[2] = 0b00100000; // VIN1 - VIN0
    buf[2] = 0b00010000; // VIN0 - VINCOM
    
    ret = no_os_spi_write_and_read(spi, buf, 3);
    if (ret < 0)
    {
        printf("Error in no_os_spi_write_and_read!\r\n");
        return 1;
    }

    while(true)
    {
        buf[0] = AD4114_COMM_REG_WEN | AD4114_COMM_REG_RD | AD4114_COMM_REG_RA(0);
        buf[1] = 0;
        ret = no_os_spi_write_and_read(spi, buf, 2);
        if (ret < 0)
        {
            printf("Error in no_os_spi_write_and_read!\r\n");
            return 1;
        }

        if(buf[1] & 0x80)
        {
            // ~RDY
        }
        else
        {
            buf[0] = AD4114_COMM_REG_WEN | AD4114_COMM_REG_RD | AD4114_COMM_REG_RA(4);
            buf[1] = 0;
            buf[2] = 0;
            buf[3] = 0;
            ret = no_os_spi_write_and_read(spi, buf, 4);
            if (ret < 0)
            {
                printf("Error in no_os_spi_write_and_read!\r\n");
                return 1;
            }

            printf("0x%02x%02x%02x,\r\n", buf[1], buf[2], buf[3]);
        }
    }

    ret = no_os_spi_remove(spi);
    if(ret < 0)
    {
        printf("Error in no_os_spi_remove!\r\n");
        return 1;
    }

    printf("Done!\r\n");

#endif // IIO_SUPPORT
}
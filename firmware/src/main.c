//#undef IIO_SUPPORT

#include "parameters.h"
#include "no_os_uart.h"
#include "maxim_uart.h"
#include "no_os_spi.h"
#include "maxim_spi.h"
#include "no_os_delay.h"
#include "ad717x.h"
#include "ad411x_regs.h"

#ifdef IIO_SUPPORT
#include "iio_ad4114_exg.h"
#include "iio_app.h"
#endif

char _iio_ad4114_samples_buf[16 * 4 * 1024]; // 16 channels, 1024 samples, 4 bytes/sample

int main()
{
    int ret;

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
	ret = no_os_uart_init(&uart, &uart_init);
    if(ret != 0)
    {
        printf("no_os_uart_init failed with ret %d\r\n", ret);
        return ret;
    }

	no_os_uart_stdio(uart);

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
	ad717x_init_param ad4114_init = {
        .spi_init = spi_init,
        .regs = ad4111_regs,
        .num_regs = sizeof(ad4111_regs) / sizeof(ad4111_regs[0]),
        .active_device = ID_AD4114,
        .num_channels = 16,
        .num_setups = 8,
        .mode = CONTINUOUS
    };
    
    ad4114_init.chan_map[0].analog_inputs.analog_input_pairs = VIN0_VINCOM;
    ad4114_init.chan_map[1].analog_inputs.analog_input_pairs = VIN1_VINCOM;
    ad4114_init.chan_map[2].analog_inputs.analog_input_pairs = VIN1_VIN0;

    /*
    ad4114_init.chan_map[3].analog_inputs.analog_input_pairs = VIN3_VINCOM;
    ad4114_init.chan_map[4].analog_inputs.analog_input_pairs = VIN4_VINCOM;
    ad4114_init.chan_map[5].analog_inputs.analog_input_pairs = VIN5_VINCOM;
    ad4114_init.chan_map[6].analog_inputs.analog_input_pairs = VIN6_VINCOM;
    ad4114_init.chan_map[7].analog_inputs.analog_input_pairs = VIN7_VINCOM;
    ad4114_init.chan_map[8].analog_inputs.analog_input_pairs = VIN8_VINCOM;
    ad4114_init.chan_map[9].analog_inputs.analog_input_pairs = VIN9_VINCOM;
    ad4114_init.chan_map[10].analog_inputs.analog_input_pairs = VIN10_VINCOM;
    ad4114_init.chan_map[11].analog_inputs.analog_input_pairs = VIN11_VINCOM;
    ad4114_init.chan_map[12].analog_inputs.analog_input_pairs = VIN12_VINCOM;
    ad4114_init.chan_map[13].analog_inputs.analog_input_pairs = VIN13_VINCOM;
    ad4114_init.chan_map[14].analog_inputs.analog_input_pairs = VIN14_VINCOM;
    ad4114_init.chan_map[15].analog_inputs.analog_input_pairs = VIN15_VINCOM;
    */

    for(int i = 0; i < 16; i++)
    {
        ad4114_init.chan_map[i].channel_enable = 0;
        ad4114_init.chan_map[i].setup_sel = i % 8;
    }

    for(int i = 0; i < 8; i++)
    {
        ad4114_init.setups[i].bi_unipolar = 0; // unused anyway?
        ad4114_init.setups[i].input_buff = 1;
        ad4114_init.setups[i].ref_buff = 1;
        ad4114_init.setups[i].ref_source = INTERNAL_REF;

        ad4114_init.filter_configuration[i].odr = 0b01010; // => 1007/1008 Hz
    }

    ad717x_dev *ad4114 = NULL;
    ret = AD717X_Init(&ad4114, ad4114_init);
    if(ret)
    {
        return ret;
    }


#ifdef IIO_SUPPORT
    // Tear down UART stdio
    fflush(stdout);
    no_os_uart_remove(uart);

    // Set up IIO
    struct iio_data_buffer iio_ad4114_read_buf = {
		.buff = _iio_ad4114_samples_buf,
		.size = sizeof(_iio_ad4114_samples_buf),
	};

    struct iio_app_device devices[] = {
        IIO_APP_DEVICE("ad4114-exg", ad4114, &iio_ad4114_exg, &iio_ad4114_read_buf, NULL, NULL)
    };

    struct iio_app_desc *app;
	struct iio_app_init_param app_init = { 0 };

    app_init.devices = devices;
	app_init.nb_devices = 1;
	app_init.uart_init_params = uart_init;

    ret = iio_app_init(&app, app_init);
    if (ret != 0)
    {
        return ret;
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
    
    

#endif // IIO_SUPPORT
}
//#undef IIO_SUPPORT

#include "parameters.h"
#include "no_os_uart.h"
#include "maxim_uart.h"
#include "no_os_spi.h"
#include "maxim_spi.h"
#include "no_os_irq.h"
#include "maxim_gpio_irq.h"
#include "no_os_delay.h"
#include "ad717x.h"
#include "ad411x_regs.h"

#ifdef IIO_SUPPORT
#include "iio_trigger.h"
#include "iio_ad4114_exg.h"
#include "iio_app.h"
#endif

char _iio_ad4114_samples_buf[16 * 4 * 1024]; // 16 channels, 1024 samples, 4 bytes/sample

extern void handle_sample_ready_irq();

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
		.max_speed_hz = 20000000,
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

    for(int i = 0; i < 16; i++)
    {
        ad4114_init.chan_map[i].channel_enable = 0;
        ad4114_init.chan_map[i].setup_sel = i / 2;
    }

    for(int i = 0; i < 8; i++)
    {
        ad4114_init.setups[i].bi_unipolar = 1; // bipolar coding, offset binary. code = 2^{n-1} * (Vin * 0.1 / Vref + 1). Vin = (code / 2^{n-1} - 1) * Vref * 10
        ad4114_init.setups[i].input_buff = 1;
        ad4114_init.setups[i].ref_buff = 1;
        ad4114_init.setups[i].ref_source = INTERNAL_REF;

        ad4114_init.filter_configuration[i].odr = sps_1007; // => 1007/1008 Hz
    }

    ad717x_dev *ad4114 = NULL;
    ret = AD717X_Init(&ad4114, ad4114_init);
    if(ret)
    {
        return ret;
    }

#ifdef IIO_SUPPORT
    // Set up hardware trigger
    struct no_os_irq_init_param irq_crtl_init = {
        .irq_ctrl_id = 0,
        .platform_ops = &max_gpio_irq_ops,
        .extra = NULL
    };

    struct no_os_irq_ctrl_desc *gpio0_irq_ctrl;
    ret = no_os_irq_ctrl_init(&gpio0_irq_ctrl, &irq_crtl_init);
	if (ret)
		return ret;

    struct iio_hw_trig *ad4114_trig;

    struct iio_hw_trig_cb_info ad4114_trig_callback = {
        .event = NO_OS_EVT_GPIO,
        .peripheral = NO_OS_GPIO_IRQ,
        .handle = handle_sample_ready_irq
    };

    struct iio_hw_trig_init_param ad4114_trig_init = {
        .irq_ctrl = gpio0_irq_ctrl,
        .irq_id = 6, // DOUT/~DRDY on pin P0_6
        .irq_trig_lvl = NO_OS_IRQ_EDGE_FALLING,
        .cb_info = ad4114_trig_callback,
        .name = "sample-ready"
    };

    ret = iio_hw_trig_init(&ad4114_trig, &ad4114_trig_init); // This registers the callback and everything
	if (ret)
		return ret;

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

    ad4114_trig->iio_desc = app->iio_desc;

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
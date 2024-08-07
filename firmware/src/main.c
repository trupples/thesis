// #undef IIO_SUPPORT

#include "common/common_data.h" // Will bring in platform-specific definitions as well

#include "no_os_uart.h"
#include "no_os_alloc.h"

#ifdef IIO_SUPPORT
#include "iio_ad4114_exg.h"
#include "iio_app.h"
#else
#include "no_os_delay.h"
#endif

void please(void *ctx) {
    printf("P");
}

char _iio_ad4114_samples_buf[16 * 4 * 1024];

int main()
{
    int ret;

    // Set up UART stdio
    struct no_os_uart_desc *uart;
	ret = no_os_uart_init(&uart, &uart_init);
    if(ret)
    {
        return ret;
    }

	no_os_uart_stdio(uart);

    for(int i = 0; i < 10; i++)
    {
        printf("Heyo! ");
        fflush(stdout);
    }

#ifdef IIO_SUPPORT
    struct iio_data_buffer iio_ad4114_read_buf = {
		.buff = _iio_ad4114_samples_buf,
		.size = sizeof(_iio_ad4114_samples_buf)
	};

    iio_ad4114_exg_dev *dev;
    struct iio_ad4114_exg_init_param iio_ad4114_exg_ip = {
        .spi_init = &spi_ad4114_init,
        .samplerdy_timer_init = &samplerdy_timer_init,
        .irq_ctrl_init = &irq_ctrl_init,
        .trig_init = &ad4114_trig_init,
    };
    ret = iio_ad4114_exg_init(&dev, iio_ad4114_exg_ip);
    if(ret)
    {
        return ret;
    }

    struct iio_trigger_init trigs[] = {
        { // trigger0
            .name = "sample-ready",
            .trig = dev->trig,
            .descriptor = &dev->trig_desc
        }
    };

    struct iio_app_device devices[] = {
        { // iio:device0
            .name = "ad4114-exg",
            .read_buff = &iio_ad4114_read_buf,
            .write_buff = NULL,
            .dev = dev,
            .dev_descriptor = &dev->iio_dev,
            .default_trigger_id = "trigger0"
        }
    };

    struct iio_app_desc *app;
	struct iio_app_init_param app_init = {
        .devices = devices,
        .nb_devices = NO_OS_ARRAY_SIZE(devices),
        .uart_init_params = uart_init,
        .trigs = trigs,
        .nb_trigs = NO_OS_ARRAY_SIZE(trigs),
        .irq_desc = NULL, //dev->trig->irq_ctrl
    };

    // Tear down UART stdio
    printf("Gata distractia!\r\n");
    fflush(stdout);
    no_os_uart_remove(uart);

    ret = iio_app_init(&app, app_init);
    if (ret)
    {
        return ret;
    }

    dev->trig->iio_desc = app->iio_desc;

    // Run IIO app
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

    struct no_os_timer_desc *samplerdy_timer;
    samplerdy_timer_init.ticks_count *= 1000;
    ret = no_os_timer_init(&samplerdy_timer, &samplerdy_timer_init);
    if(ret)
    {
        printf("%s %d\r\n", __LINE__, ret);
        return ret;
    }

    struct no_os_irq_ctrl_desc *irq_ctrl;
    ret = no_os_irq_ctrl_init(&irq_ctrl, &irq_ctrl_init);
    if(ret)
    {
        printf("%s %d\r\n", __LINE__, ret);
        return ret;
    }

    ret = no_os_irq_set_priority(irq_ctrl, SAMPLERDY_TIMER_IRQ_ID, 1);
    if(ret)
    {
        printf("%s %d\r\n", __LINE__, ret);
        return ret;
    }
    
    struct no_os_callback_desc irq_cb = {
		.callback = please,
		.ctx = NULL,
		.event = NO_OS_EVT_TIM_ELAPSED,
		.handle = SAMPLERDY_TIMER_HANDLE,
		.peripheral = NO_OS_TIM_IRQ
	};

	ret = no_os_irq_register_callback(irq_ctrl, TMR0_IRQn, &irq_cb);
	if (ret)
    {
        printf("%s : %d\r\n", __LINE__, ret);
        return ret;
    }

	ret = no_os_timer_start(samplerdy_timer);
	if (ret)
    {
        printf("%s : %d\r\n", __LINE__, ret);
        return ret;
    }
    
    for(int i = 0; i < 10; i++)
    {
        printf(".");
        no_os_mdelay(50);
        fflush(stdout);
    }
    
	ret = no_os_irq_enable(irq_ctrl, TMR0_IRQn);
	if (ret)
    {
        printf("%s : %d\r\n", __LINE__, ret);
        return ret;
    }
    
    for(int i = 0; i < 100; i++)
    {
        printf(".");
        no_os_mdelay(50);
        fflush(stdout);
    }

#endif // IIO_SUPPORT
}
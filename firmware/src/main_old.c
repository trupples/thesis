#include "maxim_uart.h"
#include "maxim_irq.h"
#include "maxim_spi.h"

#include "no_os_error.h"
#include "iio.h"
#include "no_os_irq.h"
#include "no_os_uart.h"
#include "iio_ad7124.h"
#include "ad7124_regs.h"
#include "iio_app.h"
#include "parameters.h"

#include "maxim_uart_stdio.h"

#define MAX_SIZE_BASE_ADDR		1024

static uint8_t in_buff[MAX_SIZE_BASE_ADDR];

#define ADC_DDR_BASEADDR	((void *)in_buff)
#define NUMBER_OF_DEVICES	1

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main_old(void)
{
	int32_t status;

	/* IIO application data. */
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

    struct max_uart_init_param uart_extra_ip = {
        .flow = UART_FLOW_DIS
    };
	struct no_os_uart_init_param uart_ip = {
        .device_id = UART_DEVICE_ID,
        .baud_rate = UART_BAUDRATE,
        .size = NO_OS_UART_CS_8,
        .platform_ops = UART_OPS,
        .parity = NO_OS_UART_PAR_NO,
        .stop = NO_OS_UART_STOP_1_BIT,
        .extra = &uart_extra_ip,
	};



	struct no_os_uart_desc *ud;
	status = no_os_uart_init(&ud, &uart_ip);
	no_os_uart_stdio(ud);


	printf("HELLO!\r\n\r\n");

	struct ad7124_dev *ad7124_device;

    struct max_spi_init_param maxim_spi_ini = {
        .num_slaves = 1,
        .polarity = SPI_SS_POL_LOW,
        .vssel = MXC_GPIO_VSSEL_VDDIOH,
    };

	struct no_os_spi_init_param spi_initial = {
		.chip_select = 1, // SS1 pin P0_11, incorrectly labelled as SS0 on the datasheet
		.max_speed_hz = 1000000,
		.mode = NO_OS_SPI_MODE_3,
		.device_id = 1, // MXC_SPI_GET_SPI(1) = MXC_SPI0
		.platform_ops = &max_spi_ops,
		.extra = &maxim_spi_ini
	};
	struct ad7124_init_param ad7124_initial = {
		.spi_init = &spi_initial,
		.regs = ad7124_init_regs_default,
		.spi_rdy_poll_cnt = 1000
	};
	struct iio_data_buffer iio_ad7124_read_buff = {
		.buff = ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR,
	};

	// Page 74, example setup procedure
	status = ad7124_setup(&ad7124_device, &ad7124_initial);
	printf("ad7124_setup: >>>>    %04d    <<<<\r\n\r\n", status);

	ad7124_write_register2(ad7124_device, AD7124_Error_En, AD7124_ERREN_REG_MCLK_CNT_EN | AD7124_ERREN_REG_SPI_IGNORE_ERR_EN);


	uint32_t x;
	
	// x = 0x6969;
	// status = ad7124_read_register2(ad7124_device, AD7124_ADC_Control, &x);
	// printf("ad7124_read_register2: AD7124_ADC_Control >>>>    %04d    <<<<    >>>>    %04x    <<<<\r\n\r\n", status, x);

	for(int i = 0; i < 1000; i++) {
		printf(".");
		fflush(stdout);
	}
	
	x = 0x6969;
	status = ad7124_read_register2(ad7124_device, AD7124_Mclk_Count, &x);
	printf("ad7124_read_register2: AD7124_Mclk_Count >>>>    %04d    <<<<    >>>>    %04x    <<<<\r\n\r\n", status, x);

	for(int i = 0; i < 1000; i++) {
		printf(".");
		fflush(stdout);
	}

	x = 0x6969;
	status = ad7124_read_register2(ad7124_device, AD7124_Error, &x);
	printf("ad7124_read_register2: AD7124_Error >>>>    %04d    <<<<    >>>>    %04x    <<<<\r\n\r\n", status, x);

	for(int i = 0; i < 1000; i++) {
		printf(".");
		fflush(stdout);
	}

	x = 0x6969;
	status = ad7124_read_register2(ad7124_device, AD7124_ID, &x);
	printf("ad7124_read_register2: AD7124_ID >>>>    %04d    <<<<    >>>>    %04x    <<<<\r\n\r\n", status, x);

	for(int i = 0; i < 1000; i++) {
		printf(".");
		fflush(stdout);
	}

	x = 0x6969;
	status = ad7124_read_register2(ad7124_device, AD7124_Mclk_Count, &x);
	printf("ad7124_read_register2: AD7124_Mclk_Count >>>>    %04d    <<<<    >>>>    %04x    <<<<\r\n\r\n", status, x);

	// x = AD7124_ADC_CTRL_REG_CONT_READ | AD7124_ADC_CTRL_REG_REF_EN | (0b11<<6) | 0b00;
	// // printf("               setting AD7124_ADC_Control >>>>    %04d    <<<<    >>>>    %04x    <<<<\r\n\r\n", status, x);
	// status = ad7124_write_register2(ad7124_device, AD7124_ADC_Control, x);

	// Set up temperature measurements. Setup 1.

	ad7124_write_register2(ad7124_device, AD7124_Config_0,  0b0000100001110000); // Setup 0: bipolar, internal reference, 1x
	ad7124_write_register2(ad7124_device, AD7124_Channel_0, 0b1000000000000001); // Channel 0 uses setup 0 and measures IN0 - IN1

	ad7124_write_register2(ad7124_device, AD7124_Config_1,  0b0000100001110000); // Setup 1: bipolar, internal reference, 1x
	ad7124_write_register2(ad7124_device, AD7124_Channel_1, 0b1001001000010001); // Channel 1 uses setup 1 and measures temperature - AVss

	{
		int32_t			value;
		uint32_t reg_temp;
		int32_t ret;

		for(int i = 0; i < 100; i++)
		{
			ret = ad7124_wait_for_conv_ready(ad7124_device, 1000000);
			if (ret != 0)
				return ret;
			ret = ad7124_read_data(ad7124_device, &value);
			if (ret != 0)
				return ret;
			printf("%d %04x %d\r\n", i, (uint32_t)value, ret);
		}

	}
	
	x = 0x6969;
	status = ad7124_read_register2(ad7124_device, AD7124_Mclk_Count, &x);
	printf("ad7124_read_register2: AD7124_Mclk_Count >>>>    %04d    <<<<    >>>>    %04x    <<<<\r\n\r\n", status, x);

	fflush(stdout);
	return 0;

	status = no_os_uart_remove(ud);

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad7124-8", ad7124_device, &iio_ad7124_device,
			       &iio_ad7124_read_buff, NULL, NULL)
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NUMBER_OF_DEVICES;
	app_init_param.uart_init_params = uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
    {
		return status;
    }

    status = iio_app_run(app);
	return status;
}

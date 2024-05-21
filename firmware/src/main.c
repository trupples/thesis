/***************************************************************************//**
 *   @file   ad7124-8pmdz/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author Drimbarean Andrei (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

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
int main(void)
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

	status = ad7124_setup(&ad7124_device, &ad7124_initial);
	printf("ad7124_setup: >>>>    %04d    <<<<\r\n\r\n", status);


	uint32_t x = 0x6969;
	status = ad7124_read_register2(ad7124_device, AD7124_Data, &x);
	printf("ad7124_read_register2: >>>>    %04d    <<<<    >>>>    %04d    <<<<\r\n\r\n", status, x);



	fflush(stdout);
	if (status < 0)
    {
		return status;
    }

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

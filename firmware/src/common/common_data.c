#include "common_data.h"

struct no_os_uart_init_param uart_init = {
    .device_id = UART_DEVICE_ID,
    .baud_rate = UART_BAUDRATE,
    .size = NO_OS_UART_CS_8,
    .platform_ops = UART_OPS,
    .parity = NO_OS_UART_PAR_NO,
    .stop = NO_OS_UART_STOP_1_BIT,
    .extra = UART_EXTRA,
    .asynchronous_rx = true, // hai sa te vad acum
};

struct no_os_spi_init_param spi_ad4114_init = {
    .chip_select = SPI_CS,
    .max_speed_hz = 20000000,
    .mode = NO_OS_SPI_MODE_3,
    .device_id = SPI_DEVICE_ID,
    .platform_ops = SPI_OPS,
    .extra = SPI_EXTRA
};

#define TICKS_COUNT 5

struct no_os_timer_init_param samplerdy_timer_init = {
    .id = SAMPLERDY_TIMER_ID, 
    .freq_hz = 320000 * TICKS_COUNT, // 20000Hz / channel -> sufficient even with the 1733sps ODR when using sinc3
    .ticks_count = TICKS_COUNT,
    .platform_ops = SAMPLERDY_TIMER_OPS,
    .extra = SAMPLERDY_TIMER_EXTRA
};

struct no_os_irq_init_param irq_ctrl_init = {
    .irq_ctrl_id = IRQC_DEVICE_ID,
    .platform_ops = IRQC_OPS,
    .extra = IRQC_EXTRA
};

#ifdef IIO_SUPPORT

struct iio_hw_trig_init_param ad4114_trig_init = {
    .irq_id = SAMPLERDY_TIMER_IRQ_ID,
    .cb_info = {
        .event = NO_OS_EVT_TIM_ELAPSED,
        .peripheral = NO_OS_TIM_IRQ,
        .handle = SAMPLERDY_TIMER_HANDLE
    },
    .name = "sample-ready",
    .irq_ctrl = NULL, // Must be set when initializing the trigger
};

#endif

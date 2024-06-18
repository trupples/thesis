#include "parameters.h"

struct max_uart_init_param max_uart_extra_init = {
    .flow = UART_FLOW_DIS
};

struct max_spi_init_param max_spi_ad4114_init = {
    .num_slaves = 1,
    .polarity = SPI_SS_POL_LOW,
    .vssel = MXC_GPIO_VSSEL_VDDIOH,
};

#include "platform/platform_includes.h"

#ifdef IIO_SUPPORT
#include "iio_ad4114_exg.h"
#endif

extern struct no_os_uart_init_param uart_init;
extern struct no_os_spi_init_param spi_ad4114_init;
extern struct no_os_timer_init_param samplerdy_timer_init;
extern struct no_os_irq_init_param irq_ctrl_init;

#ifdef IIO_SUPPORT

extern struct iio_hw_trig_init_param ad4114_trig_init;

#endif

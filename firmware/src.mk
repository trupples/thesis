SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(INCLUDE)

# Platform drivers
INCS += $(PLATFORM_DRIVERS)/maxim_uart.h \
        $(PLATFORM_DRIVERS)/maxim_gpio_irq.h \
        $(PLATFORM_DRIVERS)/maxim_irq.h \
        $(PLATFORM_DRIVERS)/maxim_spi.h \
        $(PLATFORM_DRIVERS)/maxim_uart_stdio.h \
        $(PLATFORM_DRIVERS)/../common/maxim_dma.h \
        $(DRIVERS)/adc/ad717x/ad717x.h \
        $(DRIVERS)/adc/ad717x/ad411x_regs.h \
        $(NO-OS)/iio/iio_trigger.h

SRCS += $(PLATFORM_DRIVERS)/maxim_uart.c \
        $(PLATFORM_DRIVERS)/maxim_gpio_irq.c \
        $(PLATFORM_DRIVERS)/maxim_irq.c \
        $(PLATFORM_DRIVERS)/maxim_spi.c \
        $(PLATFORM_DRIVERS)/maxim_delay.c \
        $(PLATFORM_DRIVERS)/maxim_uart_stdio.c \
        $(PLATFORM_DRIVERS)/../common/maxim_dma.c \
        $(DRIVERS)/api/no_os_uart.c \
        $(DRIVERS)/api/no_os_spi.c \
        $(DRIVERS)/api/no_os_irq.c \
        $(DRIVERS)/api/no_os_dma.c \
        $(NO-OS)/util/no_os_util.c \
        $(NO-OS)/util/no_os_alloc.c \
        $(NO-OS)/util/no_os_list.c \
        $(NO-OS)/util/no_os_mutex.c \
        $(NO-OS)/util/no_os_lf256fifo.c \
        $(DRIVERS)/adc/ad717x/ad717x.c \
        $(NO-OS)/iio/iio_trigger.c

# Bodge to inject `-lm` in the `ld` call because `no-OS/tools/scripts/maxim.mk` resets `LDFLAGS`
# EXTRA_LIBS += m

# IIO
IIOD = y
SRC_DIRS += $(NO-OS)/iio/iio_app

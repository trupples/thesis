# Use a larger heap than the default 0xC00 (see startup_max78000.S)
HEAP_SIZE = 0x2000

SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(INCLUDE)

# Platform specific files
include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c

# AD4114
INCS += $(DRIVERS)/adc/ad717x/ad717x.h \
        $(DRIVERS)/adc/ad717x/ad411x_regs.h
SRCS += $(DRIVERS)/adc/ad717x/ad717x.c

# No-OS & other stuff
INCS += $(NO-OS)/iio/iio_trigger.h
SRCS += $(DRIVERS)/api/no_os_uart.c \
        $(DRIVERS)/api/no_os_spi.c \
        $(DRIVERS)/api/no_os_irq.c \
        $(DRIVERS)/api/no_os_dma.c \
        $(DRIVERS)/api/no_os_timer.c \
        $(NO-OS)/util/no_os_util.c \
        $(NO-OS)/util/no_os_alloc.c \
        $(NO-OS)/util/no_os_list.c \
        $(NO-OS)/util/no_os_mutex.c \
        $(NO-OS)/util/no_os_lf256fifo.c \
        $(NO-OS)/iio/iio_trigger.c

INCS += $(PROJECT)/src/debugbreak.h


# Bodge to inject `-lm` in the `ld` call because `no-OS/tools/scripts/maxim.mk` resets `LDFLAGS`
# EXTRA_LIBS += m

# IIO
IIOD = y
SRC_DIRS += $(NO-OS)/iio/iio_app

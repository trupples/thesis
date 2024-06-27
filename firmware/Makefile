PLATFORM ?= maxim
TARGET ?= MAX78000
BOARD ?= FTHR_RevA

ifeq ($(strip $(PLATFORM)),maxim)
  ifeq ($(strip $(MAXIM_PATH)),)
    $(error MAXIM_PATH not set)
  endif

  MAXIM_LIBRARIES ?= $(MAXIM_PATH)/Libraries
endif

NO-OS = $(realpath ./lib/no-OS)
include $(NO-OS)/tools/scripts/generic_variables.mk

include src.mk

include $(NO-OS)/tools/scripts/generic.mk

CFLAGS += -DUART_BLANA

# Create build/firmware.elf.openocd required by debug plugin
include ./debug.mk

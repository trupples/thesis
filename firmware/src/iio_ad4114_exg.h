#pragma once

#include "iio_types.h"
#include "iio_trigger.h"
#include "ad717x.h"

typedef struct {
    struct iio_device iio_dev;
    ad717x_dev *dev; // Underlying AD4114 device

    struct no_os_timer_desc *samplerdy_timer;
    struct iio_hw_trig *trig; // sample-ready trigger
    struct iio_trigger trig_desc;

    uint32_t sample_buf[16]; // Incomplete sample of all enabled channels
    uint8_t channel_offset[16]; // After converting channel X, store it in the buffer at channel_offset[X]
    uint8_t last_enabled_channel; // Used to detect a full sample buffer
}  iio_ad4114_exg_dev;

struct iio_ad4114_exg_init_param {
    struct no_os_spi_init_param *spi_init;
    struct no_os_timer_init_param *samplerdy_timer_init;
    struct no_os_irq_init_param *irq_ctrl_init;
    struct iio_hw_trig_init_param *trig_init;
};

int iio_ad4114_exg_init(iio_ad4114_exg_dev **dev, struct iio_ad4114_exg_init_param init);
int iio_ad4114_exg_remove(iio_ad4114_exg_dev *dev);

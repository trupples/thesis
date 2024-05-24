/*
TODO IMPLEMENT THIS:

iio:device0: ad7128-8-exg (buffer capable)
    16 channels found:
        voltage0..15:  (input, index: 0..15, format: le:u24/32>>0)
        channel-specific attributes:
            attr  0: ainp (rw)
            attr  1: ainp_available (r): a0 a1 ... a15 zero temp
            attr  2: ainm (rw)
            attr  3: ainm_available (r): a0 a1 ... a15 zero=singleended
            attr  4: pga (rw)
            attr  5: pga_available (r): 1 2 4 8 16 32 64 128
            attr  6: raw (r)
            attr  7: scale (r)
    
    device-specific attributes:
        attr  0: clocksource
        attr  1: clocksource_available: internal external
        attr  2: powerdown
        attr  3: sampling_frequency
        attr  4: sampling_frequency_available: 2048 1024 512 idk??
    
    debug attributes:
        debug attr  0: direct_reg_access

config registers will be automatically assigned based on requested channel
attributes. if more than 8 configurations are needed by active channels,
an error will occur when activating a channel. inactive channels' configs
may be changed without any errors.

*/

#include <stdio.h>
#include <string.h>
#include "iio_types.h"
#include "ad7124.h"
#include "errno.h"

// Attribute getters and setters

#define IIO_ATTR_HANDLER(name) static int32_t name (void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
#define NOT_IMPLEMENTED() return snprintf(buf, len, "UNIMPLEMENTED")

IIO_ATTR_HANDLER(iio_ad7124_exg_channel_get_ainp)
{
    NOT_IMPLEMENTED();
}

IIO_ATTR_HANDLER(iio_ad7124_exg_channel_set_ainp)
{
    NOT_IMPLEMENTED();
}

IIO_ATTR_HANDLER(iio_ad7124_exg_channel_get_ainp_available)
{
    return snprintf(buf, len, "AIN0 AIN1 AIN2 AIN3 AIN4 AIN5 AIN6 AIN7 AIN8 AIN9 AIN10 AIN11 AIN12 AIN13 AIN14 AIN15");
}

IIO_ATTR_HANDLER(iio_ad7124_exg_channel_get_ainm)
{
    NOT_IMPLEMENTED();
}

IIO_ATTR_HANDLER(iio_ad7124_exg_channel_set_ainm)
{
    NOT_IMPLEMENTED();
}

IIO_ATTR_HANDLER(iio_ad7124_exg_channel_get_ainm_available)
{
    return snprintf(buf, len, "AIN0 AIN1 AIN2 AIN3 AIN4 AIN5 AIN6 AIN7 AIN8 AIN9 AIN10 AIN11 AIN12 AIN13 AIN14 AIN15 REF");
}

IIO_ATTR_HANDLER(iio_ad7124_exg_channel_get_pga)
{
	NOT_IMPLEMENTED();
}

IIO_ATTR_HANDLER(iio_ad7124_exg_channel_set_pga)
{
	NOT_IMPLEMENTED();
}

IIO_ATTR_HANDLER(iio_ad7124_exg_channel_get_pga_available)
{
    return snprintf(buf, len, "1 2 4 8 16 32 64 128");
}

IIO_ATTR_HANDLER(iio_ad7124_exg_channel_get_raw)
{
	NOT_IMPLEMENTED();
}

IIO_ATTR_HANDLER(iio_ad7124_exg_channel_get_scale)
{
	NOT_IMPLEMENTED();
}


IIO_ATTR_HANDLER(iio_ad7124_exg_get_clocksource)
{
	NOT_IMPLEMENTED();
}

IIO_ATTR_HANDLER(iio_ad7124_exg_set_clocksource)
{
	NOT_IMPLEMENTED();
}

IIO_ATTR_HANDLER(iio_ad7124_exg_get_clocksource_available)
{
    return snprintf(buf, len, "internal external");
}

IIO_ATTR_HANDLER(iio_ad7124_exg_get_sampling_frequency)
{
	NOT_IMPLEMENTED();
}

IIO_ATTR_HANDLER(iio_ad7124_exg_set_sampling_frequency)
{
	NOT_IMPLEMENTED();
}

IIO_ATTR_HANDLER(iio_ad7124_exg_get_sampling_frequency_available)
{
    return snprintf(buf, len, "6 1024");
}

IIO_ATTR_HANDLER(iio_ad7124_exg_get_powerdown)
{
	NOT_IMPLEMENTED();
}

IIO_ATTR_HANDLER(iio_ad7124_exg_set_powerdown)
{
	NOT_IMPLEMENTED();
}

#undef IIO_ATTR_HANDLER

// Channel definition

struct scan_type iio_ad7124_exg_scan_type = {
    .sign = 's',
    .realbits = 24,
    .storagebits = 32,
    .shift = 0,
    .is_big_endian = false
};

typedef int attr_handler(void *, char *, uint32_t, const struct iio_ch_info *, intptr_t);

struct iio_attribute iio_ad7124_exg_channel_attributes[] = {
    { .name = "ainp",           .priv = 0, .shared = IIO_SEPARATE,      .show = (attr_handler*) iio_ad7124_exg_channel_get_ainp,           .store = (attr_handler*) iio_ad7124_exg_channel_set_ainp },
    { .name = "ainp_available", .priv = 0, .shared = IIO_SHARED_BY_ALL, .show = (attr_handler*) iio_ad7124_exg_channel_get_ainp_available, .store = 0 },
    { .name = "ainm",           .priv = 0, .shared = IIO_SEPARATE,      .show = (attr_handler*) iio_ad7124_exg_channel_get_ainm,           .store = (attr_handler*) iio_ad7124_exg_channel_set_ainm },
    { .name = "ainm_available", .priv = 0, .shared = IIO_SHARED_BY_ALL, .show = (attr_handler*) iio_ad7124_exg_channel_get_ainm_available, .store = 0 },
    { .name = "pga",            .priv = 0, .shared = IIO_SEPARATE,      .show = (attr_handler*) iio_ad7124_exg_channel_get_pga,            .store = (attr_handler*) iio_ad7124_exg_channel_set_pga },
    { .name = "pga_available",  .priv = 0, .shared = IIO_SHARED_BY_ALL, .show = (attr_handler*) iio_ad7124_exg_channel_get_pga_available,  .store = 0 },
    { .name = "raw",            .priv = 0, .shared = IIO_SEPARATE,      .show = (attr_handler*) iio_ad7124_exg_channel_get_raw,            .store = 0 },
    { .name = "scale",          .priv = 0, .shared = IIO_SEPARATE,      .show = (attr_handler*) iio_ad7124_exg_channel_get_scale,          .store = 0 },
    { 0 } // Terminates the list
};

#define IIO_AD7124_EXG_CHAN_DEF(idx) \
{ \
    .name = "ch" #idx, \
    .ch_type = IIO_VOLTAGE, \
    .channel = idx, \
    .channel2 = idx, \
    .address = idx, \
    .scan_index = idx, \
    .scan_type = &iio_ad7124_exg_scan_type, \
    .attributes = iio_ad7124_exg_channel_attributes, \
    .ch_out = false, \
    .modified = false, \
    .indexed = true, \
    .diferential = false /* Although any of these channel may be configured as differential, we don't want them to show up as "voltageX-voltageX" */ \
}

struct iio_channel iio_ad7124_exg_channels[] = {
    IIO_AD7124_EXG_CHAN_DEF(0),
    IIO_AD7124_EXG_CHAN_DEF(1),
    IIO_AD7124_EXG_CHAN_DEF(2),
    IIO_AD7124_EXG_CHAN_DEF(3),
    IIO_AD7124_EXG_CHAN_DEF(4),
    IIO_AD7124_EXG_CHAN_DEF(5),
    IIO_AD7124_EXG_CHAN_DEF(6),
    IIO_AD7124_EXG_CHAN_DEF(7),
    IIO_AD7124_EXG_CHAN_DEF(8),
    IIO_AD7124_EXG_CHAN_DEF(9),
    IIO_AD7124_EXG_CHAN_DEF(10),
    IIO_AD7124_EXG_CHAN_DEF(11),
    IIO_AD7124_EXG_CHAN_DEF(12),
    IIO_AD7124_EXG_CHAN_DEF(13),
    IIO_AD7124_EXG_CHAN_DEF(14),
    IIO_AD7124_EXG_CHAN_DEF(15)
};

#undef IIO_AD7124_EXG_CHAN_DEF

// Device methods

static int32_t iio_ad7124_exg_pre_enable(void *dev, uint32_t mask)
{
    return -ENOSYS;
}

static int32_t iio_ad7124_exg_post_disable(void *dev)
{
    return -ENOSYS;
}

static int32_t iio_ad7124_exg_submit(struct iio_device_data *dev)
{
    return -ENOSYS;
}

static int32_t iio_ad7124_exg_trigger_handler(struct iio_device_data *dev)
{
    return -ENOSYS;
}


// Device definition

struct iio_attribute iio_ad7124_exg_attributes[] = {
    { .name = "clocksource",                  .priv = 0, .shared = IIO_SEPARATE,      .show = (attr_handler*) iio_ad7124_exg_get_clocksource,                  .store = (attr_handler*) iio_ad7124_exg_set_clocksource },
    { .name = "clocksource_available",        .priv = 0, .shared = IIO_SHARED_BY_ALL, .show = (attr_handler*) iio_ad7124_exg_get_clocksource_available,        .store = 0 },
    { .name = "sampling_frequency",           .priv = 0, .shared = IIO_SEPARATE,      .show = (attr_handler*) iio_ad7124_exg_get_sampling_frequency,           .store = (attr_handler*) iio_ad7124_exg_set_sampling_frequency },
    { .name = "sampling_frequency_available", .priv = 0, .shared = IIO_SHARED_BY_ALL, .show = (attr_handler*) iio_ad7124_exg_get_sampling_frequency_available, .store = 0 },
    { .name = "powerdown",                    .priv = 0, .shared = IIO_SEPARATE,      .show = (attr_handler*) iio_ad7124_exg_get_powerdown,                    .store = (attr_handler*) iio_ad7124_exg_set_powerdown },
    { 0 } // Terminates the list
}; 

struct iio_device iio_ad7124_exg = {
    .num_ch = 16,
    .channels = iio_ad7124_exg_channels,
    .attributes = iio_ad7124_exg_attributes,
    .debug_attributes = 0,
    .buffer_attributes = 0,

    .pre_enable = (int32_t (*)())iio_ad7124_exg_pre_enable,
    .post_disable = (int32_t (*)())iio_ad7124_exg_post_disable,
    .submit = (int32_t (*)())iio_ad7124_exg_submit,
    .trigger_handler = (int32_t (*)())iio_ad7124_exg_trigger_handler,

	.debug_reg_read = (int32_t (*)())ad7124_read_register2,
	.debug_reg_write = (int32_t (*)())ad7124_write_register2
};

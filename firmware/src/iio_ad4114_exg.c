/*
TODO IMPLEMENT THIS:

iio:device0: ad4114-8-exg (buffer capable)
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
#include "iio.h"
#include "iio_types.h"
#include "ad717x.h" // Contains AD4114 driver, despite the filename
#include "errno.h"

// Attribute getters and setters
struct iio_ad4114_exg_pair_string {
    enum ad717x_analog_input_pairs pair;
    const char string[13]; // Longest is "VINxx-VINCOM"
};
static struct iio_ad4114_exg_pair_string iio_ad4114_exg_pair_strings[] = {
    {VIN0_VIN1, "VIN0-VIN1"},
	{VIN0_VINCOM, "VIN0-VINCOM"},
	{VIN1_VIN0, "VIN1-VIN0"},
	{VIN1_VINCOM, "VIN1-VINCOM"},
	{VIN2_VIN3, "VIN2-VIN3"},
	{VIN2_VINCOM, "VIN2-VINCOM"},
	{VIN3_VIN2, "VIN3-VIN2"},
	{VIN3_VINCOM, "VIN3-VINCOM"},
	{VIN4_VIN5, "VIN4-VIN5"},
	{VIN4_VINCOM, "VIN4-VINCOM"},
	{VIN5_VIN4, "VIN5-VIN4"},
	{VIN5_VINCOM, "VIN5-VINCOM"},
	{VIN6_VIN7, "VIN6-VIN7"},
	{VIN6_VINCOM, "VIN6-VINCOM"},
	{VIN7_VIN6, "VIN7-VIN6"},
	{VIN7_VINCOM, "VIN7-VINCOM"},
    {VIN0_VIN1, "VIN8-VIN9"},
	{VIN0_VINCOM, "VIN8-VINCOM"},
	{VIN1_VIN0, "VIN9-VIN8"},
	{VIN1_VINCOM, "VIN9-VINCOM"},
    {VIN0_VIN1, "VIN10-VIN11"},
	{VIN0_VINCOM, "VIN10-VINCOM"},
	{VIN1_VIN0, "VIN11-VIN10"},
	{VIN1_VINCOM, "VIN11-VINCOM"},
    {VIN0_VIN1, "VIN12-VIN13"},
	{VIN0_VINCOM, "VIN12-VINCOM"},
	{VIN1_VIN0, "VIN13-VIN12"},
	{VIN1_VINCOM, "VIN13-VINCOM"},
    {VIN0_VIN1, "VIN14-VIN15"},
	{VIN0_VINCOM, "VIN14-VINCOM"},
	{VIN1_VIN0, "VIN15-VIN14"},
	{VIN1_VINCOM, "VIN15-VINCOM"},
	{TEMPERATURE_SENSOR, "Temperature"},
	{REFERENCE, "Reference"}
};
static const int iio_ad4114_exg_num_pair_strings = sizeof(iio_ad4114_exg_pair_strings) / sizeof(iio_ad4114_exg_pair_strings[0]);


int num;
void handle_sample_ready_irq()
{
    num++;
}

static int32_t iio_ad4114_exg_channel_get_input(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    ad717x_dev *dev = (ad717x_dev *)device;
    int ret = AD717X_ReadRegister(dev, AD717X_CHMAP0_REG + channel->ch_num);
    if(ret)
    {
        return ret;
    }

    ad717x_st_reg *chreg = AD717X_GetReg(dev, AD717X_CHMAP0_REG + channel->ch_num);
    int input = (chreg->value & AD4111_CHMAP_REG_INPUT(0x3ff)) >> 0;

    for(int i = 0; i < iio_ad4114_exg_num_pair_strings; i++)
    {
        if(iio_ad4114_exg_pair_strings[i].pair == input)
        {
            return snprintf(buf, len, "%s", iio_ad4114_exg_pair_strings[i].string);
        }
    }

    // Value read from register does not match any known input: device is in an undocumented state
    // Not 100% sure EIO is the correct error for this.
    return -EIO;
    // return snprintf(buf, len, "? %03x", input);
}

static int32_t iio_ad4114_exg_channel_set_input(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    enum ad717x_analog_input_pairs pair = 0; // 0 is an invalid value, can be used as a sentry

    for(int i = 0; i < iio_ad4114_exg_num_pair_strings; i++)
    {
        if(!strcmp(buf, iio_ad4114_exg_pair_strings[i].string))
        {
            pair = iio_ad4114_exg_pair_strings[i].pair;
            break;
        }
    }

    if(pair == 0)
    {
        return -EINVAL;
    }

    ad717x_dev *dev = (ad717x_dev *) device;
    int ret = ad717x_connect_analog_input(dev, channel->ch_num, (union ad717x_analog_inputs) {.analog_input_pairs = pair});
    if(ret)
    {
        return ret;
    }

    return 0;
}

static int32_t iio_ad4114_exg_channel_get_input_available(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    int num = 0; // Total number of bytes written

    for(int i = 0; i < iio_ad4114_exg_num_pair_strings; i++)
    {
        num += snprintf(buf + num, len - num, "%s%s", iio_ad4114_exg_pair_strings[i].string, (i == iio_ad4114_exg_num_pair_strings-1) ? "": " ");
    }

    return num;
}

static int32_t iio_ad4114_exg_channel_get_raw(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{

    ad717x_dev *dev = (ad717x_dev *) device;
    int32_t raw = 0;
    int ret = ad717x_single_read(dev, channel->ch_num, &raw);
    if(ret)
    {
        return ret;
    }

    return snprintf(buf, len, "%d", raw);
}

static int32_t iio_ad4114_exg_channel_get_scale(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    return snprintf(buf, len, "0.0000014901161193847656");
}

static int32_t iio_ad4114_exg_get_sampling_frequency(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    return snprintf(buf, len, "1007");
}

static int32_t iio_ad4114_exg_set_sampling_frequency(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    return -ENOSYS;
}

static int32_t iio_ad4114_exg_get_sampling_frequency_available(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    // 1007: internal clock
    // 1024: external clock, generate a 2*1024/1007 Mhz PWM signal. If done by the microcontroller, the jitter seems sort of alright (~20dB higher noise floor)
    return snprintf(buf, len, "1007 1024");
}

static int32_t iio_ad4114_exg_get_powerdown(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    return snprintf(buf, len, "0");
}

static int32_t iio_ad4114_exg_set_powerdown(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    return -ENOSYS;
}

// Channel definition

struct scan_type iio_ad4114_exg_scan_type = {
    .sign = 's',
    .realbits = 24,
    .storagebits = 32,
    .shift = 0,
    .is_big_endian = false
};

typedef int attr_handler(void *, char *, uint32_t, const struct iio_ch_info *, intptr_t);

struct iio_attribute iio_ad4114_exg_channel_attributes[] = {
    { .name = "input",           .priv = 0, .shared = IIO_SEPARATE,      .show = (attr_handler*) iio_ad4114_exg_channel_get_input,           .store = (attr_handler*) iio_ad4114_exg_channel_set_input },
    { .name = "input_available", .priv = 0, .shared = IIO_SHARED_BY_ALL, .show = (attr_handler*) iio_ad4114_exg_channel_get_input_available, .store = 0 },
    { .name = "raw",             .priv = 0, .shared = IIO_SEPARATE,      .show = (attr_handler*) iio_ad4114_exg_channel_get_raw,             .store = 0 },
    { .name = "scale",           .priv = 0, .shared = IIO_SHARED_BY_ALL, .show = (attr_handler*) iio_ad4114_exg_channel_get_scale,           .store = 0 },
    { 0 } // Terminates the list
};

#define IIO_AD4114_EXG_CHAN_DEF(idx) \
{ \
    .name = "ch" #idx, \
    .ch_type = IIO_VOLTAGE, \
    .channel = idx, \
    .channel2 = idx, \
    .address = idx, \
    .scan_index = idx, \
    .scan_type = &iio_ad4114_exg_scan_type, \
    .attributes = iio_ad4114_exg_channel_attributes, \
    .ch_out = false, \
    .modified = false, \
    .indexed = true, \
    .diferential = false /* Although any of these channel may be configured as differential, we don't want them to show up as "voltageX-voltageX" */ \
}

struct iio_channel iio_ad4114_exg_channels[] = {
    IIO_AD4114_EXG_CHAN_DEF(0),
    IIO_AD4114_EXG_CHAN_DEF(1),
    IIO_AD4114_EXG_CHAN_DEF(2),
    IIO_AD4114_EXG_CHAN_DEF(3),
    IIO_AD4114_EXG_CHAN_DEF(4),
    IIO_AD4114_EXG_CHAN_DEF(5),
    IIO_AD4114_EXG_CHAN_DEF(6),
    IIO_AD4114_EXG_CHAN_DEF(7),
    IIO_AD4114_EXG_CHAN_DEF(8),
    IIO_AD4114_EXG_CHAN_DEF(9),
    IIO_AD4114_EXG_CHAN_DEF(10),
    IIO_AD4114_EXG_CHAN_DEF(11),
    IIO_AD4114_EXG_CHAN_DEF(12),
    IIO_AD4114_EXG_CHAN_DEF(13),
    IIO_AD4114_EXG_CHAN_DEF(14),
    IIO_AD4114_EXG_CHAN_DEF(15)
};

#undef IIO_AD4114_EXG_CHAN_DEF

// Device methods

static int32_t iio_ad4114_exg_pre_enable(void *device, uint32_t mask)
{
    // TODO assign setups?

    ad717x_dev *dev = (ad717x_dev *) device;
    for(int i = 0; i < 16; i++)
    {
        int ret = ad717x_set_channel_status(dev, i, (mask >> i) & 1);
        if(ret)
        {
            return ret;
        }
    }

    return 0;
}

static int32_t iio_ad4114_exg_post_disable(void *device)
{
    ad717x_dev *dev = (ad717x_dev *) device;

    // Disable all channels
    for(int i = 0; i < 16; i++)
    {
        int ret = ad717x_set_channel_status(dev, i, 0);
        if(ret)
        {
            return ret;
        }
    }
}

static int32_t iio_ad4114_exg_submit(struct iio_device_data *dev_data)
{
    int ret;
    ad717x_dev *dev = dev_data->dev;

    int channel_offset[16] = { -1 };
    int num_channels = 0;
    for(int i = 0; i < 16; i++)
    {
        if((dev_data->buffer->active_mask >> i) & 1)
        {
            channel_offset[i] = num_channels++;
        }
    }

    // One "sample" = measurements of all channels
    int32_t sample[16] = { 0 };

    for(int i = 0; i < dev_data->buffer->samples; i++)
    {
        for(int j = 0; j < num_channels; j++)
        {
            ret = AD717X_WaitForReady(dev, AD717X_CONV_TIMEOUT);
            if (ret < 0)
                return ret;

            int32_t data;
            ret = AD717X_ReadData(dev, &data);
            if(ret)
            {
                return ret;
            }

            uint8_t status = data & 0xff;
            int channel = status & AD717X_STATUS_REG_CH(7);
            sample[channel_offset[channel]] = data >> 8; // most significant 3 bytes are the data
        }

        ret = iio_buffer_push_scan(dev_data->buffer, sample);
        if(ret)
        {
            return ret;
        }
    }

    return 0;
}

static int32_t iio_ad4114_exg_trigger_handler(struct iio_device_data *dev_data)
{
    return -ENOSYS;
}

// Device definition

struct iio_attribute iio_ad4114_exg_attributes[] = {
    { .name = "sampling_frequency",           .priv = 0, .shared = IIO_SEPARATE,      .show = (attr_handler*) iio_ad4114_exg_get_sampling_frequency,           .store = (attr_handler*) iio_ad4114_exg_set_sampling_frequency },
    { .name = "sampling_frequency_available", .priv = 0, .shared = IIO_SHARED_BY_ALL, .show = (attr_handler*) iio_ad4114_exg_get_sampling_frequency_available, .store = 0 },
    { .name = "powerdown",                    .priv = 0, .shared = IIO_SEPARATE,      .show = (attr_handler*) iio_ad4114_exg_get_powerdown,                    .store = (attr_handler*) iio_ad4114_exg_set_powerdown },
    { 0 } // Terminates the list
}; 

static int32_t debug_get_num(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    return snprintf(buf, len, "%d", num);
}

struct iio_attribute iio_ad4114_debug_attributes[] = {
    { .name = "num_irq_triggers", .priv = 0, .shared = IIO_SEPARATE, .show = (attr_handler*) debug_get_num, .store = 0 },
    { 0 }
};

// Adapt IIO debug read function signature to AD717X_ReadRegister's
int32_t ad4114_debug_read(ad717x_dev *dev, uint32_t reg, uint32_t *readval)
{
    int ret = AD717X_ReadRegister(dev, reg);
    if(ret)
    {
        return ret;
    }

    *readval = dev->regs[reg].value;
    return 0;
}

struct iio_device iio_ad4114_exg = {
    .num_ch = 16,
    .channels = iio_ad4114_exg_channels,
    .attributes = iio_ad4114_exg_attributes,
    .debug_attributes = iio_ad4114_debug_attributes,
    .buffer_attributes = 0,

    .pre_enable = (int32_t (*)())iio_ad4114_exg_pre_enable,
    .post_disable = (int32_t (*)())iio_ad4114_exg_post_disable,
    .submit = (int32_t (*)())iio_ad4114_exg_submit,
    .trigger_handler = (int32_t (*)())iio_ad4114_exg_trigger_handler,

	.debug_reg_read = (int32_t (*)())ad4114_debug_read,
	.debug_reg_write = (int32_t (*)())AD717X_WriteRegister
};

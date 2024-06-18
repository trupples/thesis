#include "iio_ad4114_exg.h"

#include <stdio.h>
#include <string.h>
#include "iio.h"
#include "iio_trigger.h"
#include "ad717x.h"
#include "ad411x_regs.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_timer.h"

// Attribute getters and setters
static struct {
    enum ad717x_analog_input_pairs pair;
    const char *string;
} iio_ad4114_exg_pair_strings[] = {
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

static int32_t iio_ad4114_exg_channel_get_input(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    iio_ad4114_exg_dev *iio_dev = device;
    ad717x_dev *dev = iio_dev->dev;

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
}

static int32_t iio_ad4114_exg_channel_set_input(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    iio_ad4114_exg_dev *iio_dev = device;
    ad717x_dev *dev = iio_dev->dev;
    
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
    iio_ad4114_exg_dev *iio_dev = device;
    ad717x_dev *dev = iio_dev->dev;
    
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
    return snprintf(buf, len, "1.4901161193847656"); // uV per LSB
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
    iio_ad4114_exg_dev *iio_dev = device;
    ad717x_dev *dev = iio_dev->dev;
    
    for(int i = 0; i < 16; i++)
    {
        int ret = ad717x_set_channel_status(dev, i, (mask >> i) & 1);
        if(ret)
        {
            return ret;
        }
    }

    iio_dev->last_enabled_channel = 0;
    for(int i = 0; i < 16; i++)
    {
        if(mask & NO_OS_BIT(i))
        {
            iio_dev->channel_offset[i] = iio_dev->last_enabled_channel++;
        }
    }

    return 0;
}

static int32_t iio_ad4114_exg_post_disable(void *device)
{
    iio_ad4114_exg_dev *iio_dev = device;
    ad717x_dev *dev = iio_dev->dev;

    // Disable all channels
    for(int i = 0; i < 16; i++)
    {
        int ret = ad717x_set_channel_status(dev, i, 0);
        if(ret)
        {
            return ret;
        }
    }
    
    return 0;
}

static int32_t iio_ad4114_exg_submit(struct iio_device_data *dev_data)
{
    return 0;
}

static int32_t iio_ad4114_exg_trigger_handler(struct iio_device_data *dev_data)
{
    iio_ad4114_exg_dev *iio_dev = dev_data->dev;
    ad717x_dev *dev = iio_dev->dev;

    // Got a trigger -> check if any of the channels are readable

    // Check if RDY
	ad717x_st_reg *statusReg = AD717X_GetReg(dev, AD717X_STATUS_REG);
	if(!statusReg)
		return -EINVAL;

    int ret = AD717X_ReadRegister(dev, AD717X_STATUS_REG);
    if(ret < 0)
    {
        return ret;
    }
    
    bool nready = (statusReg->value & AD717X_STATUS_REG_RDY) == 0;
    if(nready)
    {
        return 0;
    }

    // Read out the last conversion
    int32_t data;
    ret = AD717X_ReadData(dev, &data);
    if(ret)
    {
        return ret;
    }

    uint8_t status = data & 0xff;
    int channel = status & AD717X_STATUS_REG_CH(7);
    iio_dev->sample_buf[iio_dev->channel_offset[channel]] = data >> 8; // most significant 3 bytes are the data

    // Got a full sample of all channels, push it to the buffer!
    if(channel == iio_dev->last_enabled_channel)
    {
        ret = iio_buffer_push_scan(dev_data->buffer, iio_dev->sample_buf);
        if(ret)
        {
            return ret;
        }
    }

    return 0;
}

// Device definition

struct iio_attribute iio_ad4114_exg_attributes[] = {
    { .name = "sampling_frequency",           .priv = 0, .shared = IIO_SEPARATE,      .show = (attr_handler*) iio_ad4114_exg_get_sampling_frequency,           .store = (attr_handler*) iio_ad4114_exg_set_sampling_frequency },
    { .name = "sampling_frequency_available", .priv = 0, .shared = IIO_SHARED_BY_ALL, .show = (attr_handler*) iio_ad4114_exg_get_sampling_frequency_available, .store = 0 },
    { .name = "powerdown",                    .priv = 0, .shared = IIO_SEPARATE,      .show = (attr_handler*) iio_ad4114_exg_get_powerdown,                    .store = (attr_handler*) iio_ad4114_exg_set_powerdown },
    { 0 } // Terminates the list
}; 

int num;

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


int ad4114_trig_enable(void *trig)
{
    if(!trig)
		return -EINVAL;

	struct iio_hw_trig *desc = trig;

	return no_os_irq_enable(desc->irq_ctrl, desc->irq_id);
}

int ad4114_trig_disable(void *trig)
{
    if(!trig)
		return -EINVAL;

	struct iio_hw_trig *desc = trig;

	return no_os_irq_disable(desc->irq_ctrl, desc->irq_id);
}

int iio_ad4114_exg_init(iio_ad4114_exg_dev **iio_dev, struct iio_ad4114_exg_init_param init)
{
    int ret;

    iio_ad4114_exg_dev *dev = no_os_calloc(1, sizeof(iio_ad4114_exg_dev));

    if(!dev)
        return -ENOMEM;

    *iio_dev = dev;

    // Set up AD4114
    ad717x_init_param ad4114_init = {
        .spi_init = *init.spi_init,
        .regs = ad4111_regs,
        .num_regs = sizeof(ad4111_regs) / sizeof(ad4111_regs[0]),
        .active_device = ID_AD4114,
        .num_channels = 16,
        .num_setups = 8,
        .mode = CONTINUOUS
    };

    for(int i = 0; i < 16; i++)
    {
        ad4114_init.chan_map[i].channel_enable = 0;
        ad4114_init.chan_map[i].setup_sel = i / 2;
    }

    for(int i = 0; i < 8; i++)
    {
        ad4114_init.setups[i].bi_unipolar = 1; // bipolar coding, offset binary. code = 2^{n-1} * (Vin * 0.1 / Vref + 1). Vin = (code / 2^{n-1} - 1) * Vref * 10
        ad4114_init.setups[i].input_buff = 1;
        ad4114_init.setups[i].ref_buff = 1;
        ad4114_init.setups[i].ref_source = INTERNAL_REF;

        ad4114_init.filter_configuration[i].odr = sps_1007; // => 1007/1008 Hz
    }

    ret = AD717X_Init(&dev->dev, ad4114_init);
    if(ret)
        goto error_alloc;

    // Set up timer
    ret = no_os_timer_init(&dev->samplerdy_timer, init.samplerdy_timer_init);
	if (ret)
        goto error_device;

    init.irq_ctrl_init->extra = dev->samplerdy_timer->extra;

    // Set up timer interrupt
    struct no_os_irq_ctrl_desc *irq_ctrl;    
    ret = no_os_irq_ctrl_init(&irq_ctrl, init.irq_ctrl_init);
	if (ret)
        goto error_timer;

	ret = no_os_irq_set_priority(irq_ctrl, init.trig_init->irq_id, 1);
	if (ret)
        goto error_timer;

    init.trig_init->irq_ctrl = irq_ctrl;
    init.trig_init->cb_info.handle = dev->samplerdy_timer; // ???

    // Set up trigger
    ret = iio_hw_trig_init(&dev->trig, init.trig_init); // This registers the callback and everything
	if (ret)
        goto error_trig;
    
    ret = no_os_timer_start(dev->samplerdy_timer);
	if (ret)
        goto error_trig;

    dev->trig_desc = (struct iio_trigger) {
        .is_synchronous = true,
        .enable = iio_trig_enable,
        .disable = iio_trig_disable,
    };

    dev->iio_dev = (struct iio_device) {
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

    return 0;

error_trig:
    iio_hw_trig_remove(dev->trig);
error_timer:
    no_os_timer_remove(dev->samplerdy_timer);
error_device:
    AD717X_remove(dev->dev);
error_alloc:
    no_os_free(dev);

    return ret;
}

int iio_ad4114_exg_remove(iio_ad4114_exg_dev *iio_dev)
{
    if(!iio_dev)
        return -ENODEV;

    iio_hw_trig_remove(iio_dev->trig);
    no_os_timer_remove(iio_dev->samplerdy_timer);
    AD717X_remove(iio_dev->dev);
    no_os_free(iio_dev);
    return 0;
}

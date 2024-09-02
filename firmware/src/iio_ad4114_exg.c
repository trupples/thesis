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

uint32_t num = 0;

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
    {VIN8_VIN9, "VIN8-VIN9"},
	{VIN8_VINCOM, "VIN8-VINCOM"},
	{VIN9_VIN8, "VIN9-VIN8"},
	{VIN9_VINCOM, "VIN9-VINCOM"},
    {VIN10_VIN11, "VIN10-VIN11"},
	{VIN10_VINCOM, "VIN10-VINCOM"},
	{VIN11_VIN10, "VIN11-VIN10"},
	{VIN11_VINCOM, "VIN11-VINCOM"},
    {VIN12_VIN13, "VIN12-VIN13"},
	{VIN12_VINCOM, "VIN12-VINCOM"},
	{VIN13_VIN12, "VIN13-VIN12"},
	{VIN13_VINCOM, "VIN13-VINCOM"},
    {VIN14_VIN15, "VIN14-VIN15"},
	{VIN14_VINCOM, "VIN14-VINCOM"},
	{VIN15_VIN14, "VIN15-VIN14"},
	{VIN15_VINCOM, "VIN15-VINCOM"},
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

    return snprintf(buf, len, "%04x", chreg->value);
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
    
    uint32_t raw = 0;
    int ret = ad717x_single_read(dev, channel->ch_num, &raw);
    if(ret)
    {
        return ret;
    }
    raw = (raw >> 8) & 0xffffff; // Discard status byte

    return snprintf(buf, len, "%lu", raw);
}

// code = 2**23 * ((V * 0.1 / Vref) + 1)
// V = (code - 2**23) * 2**-23 * Vref * 10
// offset = -2**23 = 
// scale = 2**-23 * Vref * 10 = 0.0029802322387 mV

static int32_t iio_ad4114_exg_channel_get_scale(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    return snprintf(buf, len, "0.0029802322387"); // mV / LSB
}

static int32_t iio_ad4114_exg_channel_get_offset(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    return snprintf(buf, len, "-8388608"); // -2**23 = -8388608
}

struct ad4114_channel_config ad4114_channel_configs[] = {
    {},
    {sps_1007, sinc5_sinc1, 1007},
    {sps_2957, sinc5_sinc1, 1298.5},
    {sps_5208, sinc5_sinc1, 1038.33},
    {sps_10417, sinc5_sinc1, 1111},
    {sps_15625, sinc5_sinc1, 1036.33},
    {sps_31250_a, sinc5_sinc1, 1035.17},
    {sps_31250_a, sinc3, 10309 / 7},
    {sps_31250_a, sinc3, 10309 / 8},
    {sps_31250_a, sinc3, 10309 / 9},
    {sps_31250_a, sinc3, 10309 / 10},
    {sps_31250_a, sinc3, 10309 / 11},
    {sps_31250_a, sinc3, 10309 / 12},
    {sps_31250_a, sinc3, 10309 / 13},
    {sps_31250_a, sinc3, 10309 / 14},
    {sps_31250_a, sinc3, 10309 / 15},
    {sps_31250_a, sinc3, 10309 / 16}
};

static int32_t iio_ad4114_exg_get_sampling_frequency(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    return snprintf(buf, len, "%.2f", ((iio_ad4114_exg_dev *)device)->current_config.channel_odr);
}

// Channel definition

struct scan_type iio_ad4114_exg_scan_type = {
    .sign = 'u',
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
    { .name = "offset",          .priv = 0, .shared = IIO_SHARED_BY_ALL, .show = (attr_handler*) iio_ad4114_exg_channel_get_offset,          .store = 0 },
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

/* Sets sinc order and ODR for filter with given ID. */
int32_t ad4114_set_filtcon(ad717x_dev *dev, uint8_t filtcon_id, enum ad717x_order sinc_order, enum ad717x_odr odr) {
    ad717x_st_reg *filtcon_reg;
	int32_t ret;

	/* Retrieve the FILTCON register */
	filtcon_reg = AD717X_GetReg(dev,
				    AD717X_FILTCON0_REG + filtcon_id);
	if (!filtcon_reg) {
		return -EINVAL;
	}

	filtcon_reg->value &= ~(0x7f);
    filtcon_reg->value |= (sinc_order << 5) | odr;

	ret = AD717X_WriteRegister(dev, AD717X_FILTCON0_REG + filtcon_id);
	if (ret) {
		return ret;
	}

	return 0;
}

// Device methods
static int32_t iio_ad4114_exg_pre_enable(void *device, uint32_t mask)
{
    iio_ad4114_exg_dev *iio_dev = device;
    ad717x_dev *dev = iio_dev->dev;
    
    int num_channels = 0;
    for(int i = 0; i < 16; i++) num_channels += (mask >> i) & 1;

    // Set channel configurations based on the number of channels
    iio_dev->current_config = ad4114_channel_configs[num_channels];
    for(int i = 0; i < 8; i++) {
        int ret = ad4114_set_filtcon(dev, i, iio_dev->current_config.sinc_order, iio_dev->current_config.odr_setting);
        if(ret)
        {
            return ret;
        }
    }

    iio_dev->last_enabled_channel = -1;
    int k = 0;
    for(int i = 0; i < 16; i++)
    {
        if(mask & NO_OS_BIT(i))
        {
            int ret = ad717x_assign_setup(dev, i, k%8);
            if(ret)
            {
                return ret;
            }

            iio_dev->channel_offset[i] = k++;
            iio_dev->last_enabled_channel = i;
        }
        else
        {
            iio_dev->channel_offset[i] = -1;
        }
    }

    for(int i = 0; i < 16; i++)
    {
        int ret = ad717x_set_channel_status(dev, i, mask & NO_OS_BIT(i));
        if(ret)
        {
            return ret;
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

// Return 0 if ready, -EAGAIN if noy yet ready, other for actual errors
int check_ad4114_ready(iio_ad4114_exg_dev *device)
{
    iio_ad4114_exg_dev *iio_dev = device;
    ad717x_dev *dev = iio_dev->dev;

    int ret = AD717X_ReadRegister(dev, AD717X_STATUS_REG);
    if(ret)
    {
        return ret;
    }

    if(dev->regs[0].value & AD717X_STATUS_REG_RDY)
    {
        return -EAGAIN;
    }

    return 0;
}

#if false

char irq_log[1025];
volatile int irq_log_idx;
#define IRQLOG(x) do { irq_log[irq_log_idx++] = (x); if(irq_log_idx >= 1024) {irq_log_idx = 0; /*debug_break();*/ }} while(0)

#else

#define IRQLOG(x) do {} while(0);

#endif

static int32_t iio_ad4114_exg_trigger_handler(struct iio_device_data *dev_data)
{
    iio_ad4114_exg_dev *iio_dev = dev_data->dev;
    ad717x_dev *dev = iio_dev->dev;

    int ret = check_ad4114_ready(iio_dev);
    if(ret == -EAGAIN)
    {
        IRQLOG(' ');
        return 0;
    }

    if(ret)
    {
        IRQLOG('E');
        return ret;
    }

    int8_t status, channel;
    uint32_t data;
    ret = AD717X_ReadData(dev, &data);
    if(ret)
    {
        IRQLOG('E');
        return ret;
    }

    status = data & 0xff;
    channel = status & 0xf;
    data = data >> 8; // actual measurement is only the first 3 bytes

    iio_dev->sample_buf[iio_dev->channel_offset[channel]] = data;
    
    IRQLOG('0' + channel);
    
    // Got a full sample of all channels, push it to the buffer!
    if(channel == iio_dev->last_enabled_channel)
    {
        IRQLOG('!');
        no_os_cb_size(dev_data->buffer->buf, &num);
        IRQLOG('0' + (num / 1000 % 10));
        IRQLOG('0' + (num / 100 % 10));
        IRQLOG('0' + (num / 10 % 10));
        IRQLOG('0' + (num % 10));
        IRQLOG('<');

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
    { .name = "sampling_frequency",           .priv = 0, .shared = IIO_SEPARATE,      .show = (attr_handler*) iio_ad4114_exg_get_sampling_frequency, .store = 0 },
    { 0 } // Terminates the list
}; 

static int32_t debug_get_num(void *device, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
    return snprintf(buf, len, "%u", num);
}

struct iio_attribute iio_ad4114_debug_attributes[] = {
    { .name = "buffer_usage", .priv = 0, .shared = IIO_SEPARATE, .show = (attr_handler*) debug_get_num, .store = 0 },
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

    enum ad717x_analog_input_pairs default_pairs[] = {
        VIN0_VINCOM,
        VIN1_VINCOM,
        VIN2_VINCOM,
        VIN3_VINCOM,
        VIN4_VINCOM,
        VIN5_VINCOM,
        VIN6_VINCOM,
        VIN7_VINCOM,
        VIN8_VINCOM,
        VIN9_VINCOM,
        VIN10_VINCOM,
        VIN11_VINCOM,
        VIN12_VINCOM,
        VIN13_VINCOM,
        VIN14_VINCOM,
        VIN15_VINCOM,
    };

    for(int i = 0; i < 16; i++)
    {
        ad4114_init.chan_map[i].channel_enable = 0;
        ad4114_init.chan_map[i].setup_sel = i / 2;
        ad4114_init.chan_map[i].analog_inputs.analog_input_pairs = default_pairs[i];
    }

    for(int i = 0; i < 8; i++)
    {
        ad4114_init.setups[i].bi_unipolar = 1; // 1 = bipolar coded ~ offset binary
        ad4114_init.setups[i].input_buff = 1;
        ad4114_init.setups[i].ref_buff = 1;
        ad4114_init.setups[i].ref_source = INTERNAL_REF; // AD717X_Init will ultimately activate the internal reference

        ad4114_init.filter_configuration[i].oder = sinc5_sinc1;
        ad4114_init.filter_configuration[i].odr = sps_1007; // => 1007/1008 Hz
    }

    ret = AD717X_Init(&(dev->dev), ad4114_init);
    if(ret)
        goto error_alloc;

    // Activate status readout
    {
        ad717x_st_reg *interfaceReg;

        interfaceReg = AD717X_GetReg(dev->dev, AD717X_IFMODE_REG);
        //interfaceReg->value |= AD717X_IFMODE_REG_CRC_EN;
        interfaceReg->value |= AD717X_IFMODE_REG_DATA_STAT;
        AD717X_WriteRegister(dev->dev, AD717X_IFMODE_REG);
        //AD717X_UpdateCRCSetting(dev->dev);
    }

    // Set up timer
    ret = no_os_timer_init(&(dev->samplerdy_timer), init.samplerdy_timer_init);
	if (ret)
        goto error_device;

    init.irq_ctrl_init->extra = dev->samplerdy_timer->extra;

    // Set up timer interrupt
    struct no_os_irq_ctrl_desc *irq_ctrl;    
    ret = no_os_irq_ctrl_init(&irq_ctrl, init.irq_ctrl_init);
	if (ret)
        goto error_timer;

    init.trig_init->irq_ctrl = irq_ctrl;

    // Set up IRQ priorities so that the samplerdy timer doesn't "eat" incoming UART data
	ret = no_os_irq_set_priority(irq_ctrl, init.trig_init->irq_id, 10);
	if (ret)
        goto error_timer;
        
	ret = no_os_irq_set_priority(irq_ctrl, 14, 1); // UART0_IRQn = 14 ... or 30?
	if (ret)
        goto error_timer;

    // Set up trigger
    ret = iio_hw_trig_init(&(dev->trig), init.trig_init); // This registers the callback and everything
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
        .irq_desc = irq_ctrl, // Make IIO use the already initialized irq controller
        .read_dev = NULL,
        .write_dev = NULL,

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

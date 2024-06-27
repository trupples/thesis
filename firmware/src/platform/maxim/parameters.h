#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_spi.h"
#include "maxim_uart.h"
#include "maxim_timer.h"
#include "maxim_irq.h"

// AD4114 SPI
#define SPI_CS         1 // SS1 pin P0_11, incorrectly labelled as SS0 on the FTHR datasheet
#define SPI_DEVICE_ID  1 // MXC_SPI_GET_SPI(1) = MXC_SPI0
#define SPI_OPS        &max_spi_ops 
#define SPI_EXTRA      &max_spi_ad4114_init

extern struct max_spi_init_param max_spi_ad4114_init;

// USB UART
#define UART_DEVICE_ID  0
#define UART_BAUDRATE   500000 // 115200
#define UART_OPS        &max_uart_ops
#define UART_EXTRA      &max_uart_extra_init

#define INTC_DEVICE_ID  0 // Used by iio_app for UART
#define UART_IRQ UART0_IRQn

extern struct max_uart_init_param max_uart_extra_init;

// "Check RDY" timer and timer interrupts
#define SAMPLERDY_TIMER_ID     0 // Use TMR0_IRQn for this timer's IRQ
#define SAMPLERDY_TIMER_OPS    &max_timer_ops // Use TMR0_IRQn for this timer's IRQ
#define SAMPLERDY_TIMER_EXTRA  NULL

#define IRQC_DEVICE_ID  0
#define IRQC_OPS        &max_irq_ops
#define IRQC_EXTRA      NULL

#define SAMPLERDY_TIMER_IRQ_ID TMR0_IRQn
#define SAMPLERDY_TIMER_HANDLE MXC_TMR0


#endif // __PARAMETERS_H__

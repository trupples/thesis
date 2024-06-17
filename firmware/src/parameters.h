#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#ifdef MAXIM_PLATFORM

#define INTC_DEVICE_ID  0
#define UART_IRQ_ID     UART0_IRQn

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_OPS        &max_uart_ops

#endif // MAXIM_PLATFORM

#endif // __PARAMETERS_H__

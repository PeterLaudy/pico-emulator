#ifndef _IRQ_H
#define _IRQ_H

#include "sync.h"

#define TIMER_IRQ_0 0
#define TIMER_IRQ_1 1
#define TIMER_IRQ_2 2
#define TIMER_IRQ_3 3
#define PWM_IRQ_WRAP 4
#define USBCTRL_IRQ 5
#define XIP_IRQ 6
#define PIO0_IRQ_0 7
#define PIO0_IRQ_1 8
#define PIO1_IRQ_0 9
#define PIO1_IRQ_1 10
#define DMA_IRQ_0 11
#define DMA_IRQ_1 12
#define IO_IRQ_BANK0 13
#define IO_IRQ_QSPI 14
#define SIO_IRQ_PROC0 15
#define SIO_IRQ_PROC1 16
#define CLOCKS_IRQ 17
#define SPI0_IRQ 18
#define SPI1_IRQ 19
#define UART0_IRQ 20
#define UART1_IRQ 21
#define ADC_IRQ_FIFO 22
#define I2C0_IRQ 23
#define I2C1_IRQ 24
#define RTC_IRQ 25

typedef void (*irq_handler_t)(void);

// IRQ priority is not supported for now.
static inline void irq_set_priority(uint num, uint8_t hardware_priority) { };

// Not all interrupts are supported.
void irq_set_exclusive_handler(uint num, irq_handler_t handler);
void irq_set_enabled(uint num, bool enabled);

#endif
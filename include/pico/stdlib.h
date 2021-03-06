#ifndef _STDLIB_H
#define _STDLIB_H

#define GPIO_OUT 1
#define GPIO_IN 0

enum gpio_function {
    GPIO_FUNC_XIP = 0,
    GPIO_FUNC_SPI = 1,
    GPIO_FUNC_UART = 2,
    GPIO_FUNC_I2C = 3,
    GPIO_FUNC_PWM = 4,
    GPIO_FUNC_SIO = 5,
    GPIO_FUNC_PIO0 = 6,
    GPIO_FUNC_PIO1 = 7,
    GPIO_FUNC_GPCK = 8,
    GPIO_FUNC_USB = 9,
    GPIO_FUNC_NULL = 0xf,
};

static inline void gpio_init(uint gpio) { }
static inline void gpio_set_dir(uint gpio, bool out) { }
static inline void gpio_set_function(uint gpio, enum gpio_function fn) { }
static inline void gpio_put(uint gpio, bool out) { }
static inline void gpio_set_mask(uint32_t mask) { }
static inline void gpio_clr_mask(uint32_t mask) { }

static inline void tight_loop_contents() { }

#endif
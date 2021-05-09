#ifndef _PWM_H
#define _PWM_H

#include "../rp2040.h"

static inline uint pwm_gpio_to_slice_num(uint gpio) { return 0; }
static inline uint pwm_gpio_to_channel(uint gpio) { return 0; }
static inline void pwm_set_clkdiv(uint slice_num,  float divider) { }
static inline void pwm_set_chan_level(uint slice_num, uint chan, uint16_t level) { }
static inline void pwm_set_wrap(uint slice_num, uint16_t wrap) { }
static inline void pwm_set_output_polarity(uint slice_num, bool a, bool b) { }

static inline uint32_t pwm_get_irq_status_mask() { return 0; }
static inline void pwm_set_irq_mask_enabled(uint32_t slice_mask, bool enabled) { }
static inline void pwm_set_mask_enabled(uint32_t mask) { }

class pwm_slice_hw_t {
    public:
        uint32_t csr;
        uint32_t div;
        uint32_t ctr;
        uint32_t cc;
        uint32_t top;
};

class pwm_hw_t {
    public:
        pwm_slice_hw_t *slice[NUM_PWM_SLICES];
        uint32_t en;
        uint32_t intr;
        uint32_t inte;
        uint32_t intf;
        uint32_t ints;
};

extern pwm_hw_t * pwm_hw;

#endif
#ifndef _PWM_H
#define _PWM_H

#include "../rp2040.h"
#include <ioregister.h>
#include <hardware/irq.h>

uint pwm_gpio_to_slice_num(uint gpio);
uint pwm_gpio_to_channel(uint gpio);
void pwm_set_clkdiv(uint slice_num, float divider);
void pwm_set_chan_level(uint slice_num, uint chan, uint16_t level);
void pwm_set_wrap(uint slice_num, uint16_t wrap);
void pwm_set_output_polarity(uint slice_num, bool a, bool b);

uint32_t pwm_get_irq_status_mask();
void pwm_set_irq_mask_enabled(uint32_t slice_mask, bool enabled);
void pwm_set_mask_enabled(uint32_t slice_mask);

class pwm_slice_hw_t {
    public:
        pwm_slice_hw_t();
        // Control and status register.
        IoRegister &csr;
        // Bits 11-4: Integer part of the 8.4 divider. Bits 0-3: Fractiontal part of the 8.4 divider.
        IoRegister &div;
        // Lower 16 bits: Direct access to PWM counter.
        IoRegister &ctr;
        // Upper 16 bits: Counter compare channel B. Lower 16 bits: Counter compare channel A.
        IoRegister &cc;
        // Lower 16 bits: Counter wrap value.
        IoRegister &top;
        
        uint16_t counterCompareA();
        uint16_t counterCompareB();
};

class pwm_hw_t {
    private:
        irq_handler_t irq_handler = NULL;
        bool irq_enabled = false;
        uint base_counter = 0;
    public:
        pwm_hw_t();
        pwm_slice_hw_t *slice[NUM_PWM_SLICES];
        // PWM slice enabled.
        IoRegister &en;
        // Raw interrupt status.
        IoRegister &intr;
        // Interrupt enabled.
        IoRegister &inte;
        // Interrupt force.
        IoRegister &intf;
        // Interrupt status and reset.
        IoRegister &ints;

        void set_irq_handler(irq_handler_t handler);
        void set_irq_enabled(bool enabled);
        void increase_counters();
        bool call_next_irq();
};

extern pwm_hw_t * pwm_hw;

#endif
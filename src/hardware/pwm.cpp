#include <hardware/pwm.h>

#include <msg_assert.h>
#include <algorithm>

pwm_hw_t* pwm_hw = new pwm_hw_t;

#define DIVIDER_RES 16
#define PWM_BASE_FREQ 125000000

uint pwm_gpio_to_slice_num(uint gpio) {
    assert(30u > gpio, "Invalid GPIO pin number: " << gpio);
    return (gpio >> 1u) % NUM_PWM_SLICES;
}

uint pwm_gpio_to_channel(uint gpio) {
    assert(30u > gpio, "Invalid GPIO pin number: " << gpio);
    return gpio & 1u;
}

/**
 * The divider is a 8.4 fractional divider. We multiply the divider by 16
 * and with the calculations also multiply the base frequency by 16.
 */
void pwm_set_clkdiv(uint slice_num, float divider) {
    assert(NUM_PWM_SLICES > slice_num, "Invalid PWM slice number: " << slice_num);
    uint div = (uint)round(divider * 16);
    assert(0x0010 <= div, "Illegal value for PWM fractional divider: " << div * 16);
    assert(0x0FFF > div, "Illegal value for PWM fractional divider: " << div * 16);
    pwm_hw->slice[slice_num]->div = div;
}

void pwm_set_chan_level(uint slice_num, uint chan, uint16_t level) {
    assert(NUM_PWM_SLICES > slice_num, "Invalid PWM slice number: " << slice_num);
    assert(2u > chan, "Invalid PWM channel number: " << chan);
    if (chan) {
        pwm_hw->slice[slice_num]->cc = (pwm_hw->slice[slice_num]->cc & 0xFFFF) | (level << 16);
    } else {
        pwm_hw->slice[slice_num]->cc = (pwm_hw->slice[slice_num]->cc & 0xFFFF0000) | level;
    }
}

void pwm_set_wrap(uint slice_num, uint16_t wrap) {
    assert(NUM_PWM_SLICES > slice_num, "Invalid PWM slice number: " << slice_num);
    pwm_hw->slice[slice_num]->top = wrap;
}

void pwm_set_output_polarity(uint slice_num, bool a, bool b) {
    assert(NUM_PWM_SLICES > slice_num, "Invalid PWM slice number: " << slice_num);
    pwm_hw->slice[slice_num]->csr = (pwm_hw->slice[slice_num]->csr & ~0x04) | (a ? 0x04 : 0x00);
    pwm_hw->slice[slice_num]->csr = (pwm_hw->slice[slice_num]->csr & ~0x08) | (b ? 0x08 : 0x00);
}

/**
 * The mask indicating which PWM slices raised an interrupt.
 */
uint32_t pwm_get_irq_status_mask() {
    return pwm_hw->ints;
}

/**
 * Enable PWM slice interrupts for the given mask.
 */
void pwm_set_irq_mask_enabled(uint32_t slice_mask, bool enabled) {
    assert(256 > slice_mask, "Invalid PWM slice mask: " << slice_mask);
    if (enabled) {
        pwm_hw->inte |= slice_mask;
    }
    else {
        pwm_hw->inte &= ~slice_mask;
    }
}

/**
 * Enable PWM slices counter for the given mask.
 */
void pwm_set_mask_enabled(uint32_t slice_mask) {
    assert(256 > slice_mask, "Invalid PWM slice mask: " << slice_mask);
    pwm_hw->en = slice_mask;
}

pwm_slice_hw_t::pwm_slice_hw_t() :
    csr(*(new IoRegister())),
    div(*(new IoRegister(0x010))),
    ctr(*(new IoRegister())),
    cc(*(new IoRegister())),
    top(*(new IoRegister(0xFFFF))) {
};

uint16_t pwm_slice_hw_t::counterCompareA() {
    return cc & 0xFFFF;
}

uint16_t pwm_slice_hw_t::counterCompareB() {
    return (cc >> 16) & 0xFFFF;
}


pwm_hw_t::pwm_hw_t() :
    en(*(new IoRegister())),
    intr(*(new IoRegister())),
    inte(*(new IoRegister())),
    intf(*(new IoRegister())),
    ints(*(new IoRegister())) {
    for (auto i = 0; i < NUM_PWM_SLICES; i++) {
        slice[i] = new pwm_slice_hw_t();

        // If the slice is disabled/enabled by writing the bit in the CSR register,
        // we set the EN register.
        slice[i]->csr.callback = [](uint32_t oldValue, uint32_t newValue) {
            if ((0x01 & oldValue) != (0x01 & newValue)) {
                uint32_t mask = 0;
                for (auto j = 0; j < NUM_PWM_SLICES; j++) {
                    if (0x01 & pwm_hw->slice[j]->csr) {
                        mask |= 1u << j;
                    }
                }

                if (mask != pwm_hw->en) {
                    pwm_hw->en = mask;
                }
            }
        };
    }

    // Reset the interrupt flags if they are cleared by writing the PWM intr register.
    // This callback is called whenever the PWM intr register is written.
    intr.callback = [](uint32_t oldValue, uint32_t newValue) {
        pwm_hw->ints &= ~newValue;
    };

    // If the EN register changes, we set the corresponding bit in the CSR register of the corresponding slice.
    en.callback = [](uint32_t oldValue, uint32_t newValue) {
        for (auto i = 0; i < NUM_PWM_SLICES; i++) {
            if (((1u << i) & oldValue) != ((1u << i) & newValue)) {
                if (newValue & (1u << i)) {
                    if (!(0x01 & pwm_hw->slice[i]->csr)) {
                        pwm_hw->slice[i]->csr |= 0x01;
                    }
                }
                else {
                    if (0x01 & pwm_hw->slice[i]->csr) {
                        pwm_hw->slice[i]->csr &= ~0x01;
                    }
                }
            }
        }
    };
};


void pwm_hw_t::set_irq_handler(irq_handler_t handler) {
    irq_handler = handler;
}

void pwm_hw_t::set_irq_enabled(bool enabled) {
    irq_enabled = enabled;
}

/**
 * Update the counters for all PWM slices to the value they would have
 * whenever the next slice would wrapor would change its output.
 */
void pwm_hw_t::increase_counters() {
    // The interval after which the next counter expires.
    uint32_t next = 0xFFFFFFFFu;

    // For all PWM slices...
    for (auto i = 0; i < NUM_PWM_SLICES; i++) {
        auto &slice = *(pwm_hw->slice[i]);
        // ...check if they are enabled.
        if (0x01 & slice.csr) {
            // If one is enabled, check how long it takes before a counter expires.
            // Take the minimum value of all slices.
            next = std::min(next, (slice.top + 1 - slice.ctr) * slice.div);
            if (slice.counterCompareA() > slice.ctr) {
                next = std::min(next, (slice.counterCompareA() - slice.ctr) * slice.div);
            }
            if (slice.counterCompareB() > slice.ctr) {
                next = std::min(next, (slice.counterCompareB() - slice.ctr) * slice.div);
            }
        }
    }

    // Now increase the PWM base counter.
    base_counter += next;

    // Check if the counter wraps.
    while (base_counter > PWM_BASE_FREQ * 16) {
        base_counter -= PWM_BASE_FREQ * 16;
    }

    // Increase the counters for all enabled slices.
    for (auto i = 0; i < NUM_PWM_SLICES; i++) {
        auto &slice = *(pwm_hw->slice[i]);
        if (0x01 & slice.csr) {
            slice.ctr += next / slice.div;
            if ((uint32_t)slice.ctr == slice.top + 1) {
                slice.ctr = 0;
            }
        }
    }
}

/**
 * Update the counters for all PWM slices to the value they would have
 * whenever the next interrupt would be generated by one of the slices.
 */
bool pwm_hw_t::call_next_irq() {
    // An interrupt is generated only if the PWM interrupt is enabled and
    // if for any PWM slice the interrupt is enabled as is the slice itself.
    if (!irq_enabled || !(en && inte)) {
        return false;
    }

    uint32_t irq_mask = 0;
    while (0 == irq_mask) {
        increase_counters();
        for (auto i = 0; i < NUM_PWM_SLICES; i++) {
            auto& slice = *(pwm_hw->slice[i]);
            // If the counter is enabled, the interrupt for the slice is enabled
            // and the counter for the slice is 0, we need to generate an interrupt.
            if ((0x01 & slice.csr) && (pwm_hw->inte & (1u << i)) && ((uint32_t)slice.ctr == 0)) {
                irq_mask |= 1u << i;
            }
        }
    }

    ints |= irq_mask;

    // If the interrupt handler is set, call it.
    if (irq_handler) {
        irq_handler();
    }

    return true;
}
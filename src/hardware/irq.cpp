#include <msg_assert.h>
#include <hardware/irq.h>
#include <hardware/pwm.h>

void irq_set_exclusive_handler(uint num, irq_handler_t handler) {
    switch (num) {
        case PWM_IRQ_WRAP:
            pwm_hw->set_irq_handler(handler);
            break;
        default:
            assert(false, "Unsupported interrupt number: " << num);
            break;
    }
}

void irq_set_enabled(uint num, bool enabled) {
    switch (num) {
        case PWM_IRQ_WRAP:
            pwm_hw->set_irq_enabled(enabled);
            break;
        default:
            assert(false, "Unsupported interrupt number: " << num);
            break;
    }
}

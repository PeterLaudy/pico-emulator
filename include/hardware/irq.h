#ifndef _IRQ_H
#define _IRQ_H

#include "sync.h"

#define PWM_IRQ_WRAP 4

typedef void (*irq_handler_t)(void);

static inline void irq_set_priority(uint num, uint8_t hardware_priority) { }
static inline void irq_set_exclusive_handler(uint num, irq_handler_t handler) { }
static inline void irq_set_enabled(uint num, bool enabled) { }

#endif
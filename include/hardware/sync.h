#ifndef _SYNC_H
#define _SYNC_H

#define __no_inline_not_in_flash_func(F) F

static inline uint32_t save_and_disable_interrupts() {
    return 0;
}

static inline void restore_interrupts(uint32_t status) { }

#endif
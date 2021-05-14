#ifndef _IOREGISTER_H
#define _IOREGISTER_H

#include <functional>

typedef void (*callback_t)(uint32_t, uint32_t);

class IoRegister {
    private:
        uint32_t ioRegister;
    public:
        IoRegister() { ioRegister = 0; }
        IoRegister(uint32_t value) { ioRegister = value; }
        callback_t callback = NULL;
        uint32_t operator=(const uint32_t& other) { uint32_t oldValue = ioRegister; ioRegister = other; if (callback) { (*callback)(oldValue, ioRegister); }; return ioRegister; }
        operator uint32_t() const { return ioRegister; }
        uint32_t operator|=(const uint32_t& mask) { uint32_t oldValue = ioRegister; ioRegister |= mask; if (callback) { (*callback)(oldValue, ioRegister); }; return ioRegister; }
        uint32_t operator&=(const uint32_t& mask) { uint32_t oldValue = ioRegister; ioRegister &= mask; if (callback) { (*callback)(oldValue, ioRegister); }; return ioRegister; }
        uint32_t operator+=(const uint32_t& val) { uint32_t oldValue = ioRegister; ioRegister += val; if (callback) { (*callback)(oldValue, ioRegister); }; return ioRegister; }
        uint32_t operator-=(const uint32_t& val) { uint32_t oldValue = ioRegister; ioRegister -= val; if (callback) { (*callback)(oldValue, ioRegister); }; return ioRegister; }

        friend bool operator==(const IoRegister& self, const IoRegister& other);
};

inline bool operator==(const IoRegister& self, const IoRegister& other) { return self.ioRegister == other.ioRegister; }

#endif
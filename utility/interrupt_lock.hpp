#pragma once

#include <cassert>

#include <main.h>

namespace utility {

class InterruptLock {
public:
    InterruptLock(const InterruptLock&)            = delete;
    InterruptLock& operator=(const InterruptLock&) = delete;
    InterruptLock(InterruptLock&&)                 = delete;
    InterruptLock& operator=(InterruptLock&&)      = delete;

    InterruptLock() {
        __disable_irq();
        ++lock_count_;
    }

    ~InterruptLock() {
        if (--lock_count_ == 0) {
            __enable_irq();
        } else {
            assert(lock_count_ > 0);
        }
    }

private:
    static inline int lock_count_;
};

} // namespace utility

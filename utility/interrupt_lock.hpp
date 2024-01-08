#pragma once

#include <cassert>

#include <main.h>

#include "utility/immovable.hpp"

namespace utility {

class InterruptMutex : Immovable {
public:
    static void lock() {
        __disable_irq();
        ++lock_count_;
    }

    static void unlock() {
        if (--lock_count_ == 0) {
            __enable_irq();
        }
    }

private:
    static inline int lock_count_;
};

class InterruptLockGuard : Immovable {
public:
    InterruptLockGuard() { InterruptMutex::lock(); }
    ~InterruptLockGuard() { InterruptMutex::unlock(); }
};

} // namespace utility

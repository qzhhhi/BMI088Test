#pragma once

#include <cstdint>

#include <chrono>
#include <ratio>

#include "module/timer/timer.hpp"

namespace module {
namespace timer {

inline void us_delay(std::chrono::duration<uint32_t, std::micro> delay) {
    auto& timer = *timer2.get();

    uint32_t start = timer.get_tick();
    uint32_t end   = start + delay.count();

    if (end < start) { // Overflow.
        while (timer.get_tick() >= start)
            ;
    }

    while (timer.get_tick() < end)
        ;
}

} // namespace timer
} // namespace module
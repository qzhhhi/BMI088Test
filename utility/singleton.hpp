#pragma once

#include <cstdint>

#include <new>
#include <type_traits>

#include <cmsis_gcc.h>

#include "utility/interrupt_lock.hpp"

namespace utility {

template <typename T>
class Singleton {
public:
    Singleton()                            = delete;
    Singleton(const Singleton&)            = delete;
    Singleton& operator=(const Singleton&) = delete;

    static_assert(
        !std::is_copy_constructible_v<T> && !std::is_copy_assignable_v<T>
        && !std::is_move_constructible_v<T> && !std::is_move_assignable_v<T>);

    static T& get_instance() {
        if (!initialized_) {
            InterruptLock lock;
            new (class_data_) T{};
            initialized_ = true;
        }
        return *reinterpret_cast<T*>(class_data_);
    }

    template <typename CallableT>
    static void weak_instance(const CallableT& callable) {
        if (initialized_) {
            callable(reinterpret_cast<T*>(class_data_));
        }
    }

    alignas(T) inline static uint8_t class_data_[sizeof(T)];
    inline static bool initialized_ = false;
};

} // namespace utility
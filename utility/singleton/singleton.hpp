#pragma once

#include <cstdint>

#include <new>
#include <type_traits>

#include <cmsis_gcc.h>

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
            __disable_irq();
            new (class_data_) T{};
            initialized_ = true;
            __enable_irq();
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

class Immovable {
public:
    Immovable()                            = default;
    Immovable(const Immovable&)            = delete;
    Immovable& operator=(const Immovable&) = delete;
    Immovable(Immovable&&)                 = delete;
    Immovable& operator=(Immovable&&)      = delete;
};

} // namespace util
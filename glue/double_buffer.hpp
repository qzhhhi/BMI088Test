/**
 * @author Qzh (zihanqin2048@gmail.com)
 * @author Thy (sy2330@qq.com)
 * @brief An optimized interrupt-safe & wait-free double buffer.
 * @warning To ensure the safety of the buffer, the push() function must be atomic, meaning it
 * cannot be interrupted. The pop() function, on the other hand, can only be interrupted by the
 * push() function. Otherwise, the buffer is left in an unsafe state.
 * @copyright Copyright (c) 2024 by Alliance, All Rights Reserved.
 */

#pragma once

#include <atomic>
#include <cstdint>
#include <new>
#include <type_traits>
#include <utility>

namespace glue {

template <typename T>
class DoubleBuffer {
public:
    static_assert(
        std::is_trivially_destructible_v<T>,
        "To meet wait-free and performance requirements, T must be trivially destructible. This "
        "implies that objects of the type can be destructed without doing anything.");

    DoubleBuffer()
        : writing_(0)
        , readable_(false){};

    template <typename... Args>
    void construct_each(Args... args) {
        new (&data_[0]) T{std::forward<Args>(args)...};
        new (&data_[1]) T{std::forward<Args>(args)...};
    }

    T& start_writing() noexcept {
        uint8_t writing = writing_.load(std::memory_order_relaxed);
        return *reinterpret_cast<T*>(&data_[writing]);
    }

    void finish_writing() noexcept { readable_.store(true, std::memory_order_relaxed); }

    bool readable() const noexcept { return readable_.load(std::memory_order_relaxed); }

    T& read() noexcept {
        readable_.store(false, std::memory_order_relaxed);
        uint8_t writing = writing_.load(std::memory_order_relaxed);
        writing_.store(!writing, std::memory_order_release);
        return *reinterpret_cast<T*>(&data_[writing]);
    }

private:
    struct alignas(T) TypeErasedT {
        uint8_t byte_array[sizeof(T)];
    } data_[2];
    std::atomic<uint8_t> writing_;
    std::atomic<bool> readable_;
};

} // namespace glue
#pragma once
/**
 * @author Qzh (zihanqin2048@gmail.com)
 * @brief A simple stack (FILO) structure implementation.
 * @warning This class does not have any bounds checking! Please check it yourself before operating.
 * @warning It is not allowed to throw exceptions in any constructor or destructor functions of T.
 * Any exception thrown will turn to std::terminate due to the noexcept flag.
 * @copyright Copyright (c) 2023 by Alliance, All Rights Reserved.
 */

#include <cstddef>
#include <cstdint>
#include <utility>

#include <new>

#include "utility/immovable.hpp"

namespace utility {
namespace data_struct {

template <typename T, size_t max_size>
class Stack : Immovable {
public:
    Stack() = default;
    ~Stack() noexcept {
        while (stack_top_ != stack_) {
            reinterpret_cast<T*>(stack_top_--)->~T();
        }
    };

    template <typename... Args>
    void push(Args&&... args) noexcept {
        new (stack_top_++) T{std::forward<Args>(args)...};
    }

    T pop() noexcept {
        auto& data = *reinterpret_cast<T*>(--stack_top_);
        T moved_data(std::move(data));
        data.~T();
        return moved_data;
    }

    T& peek() noexcept { return *stack_top_; }

    size_t size() const noexcept { return stack_top_ - stack_; }

    bool is_empty() const noexcept { return stack_top_ == stack_; }

private:
    struct alignas(T) TypeErasedDataT {
        uint8_t data[sizeof(T)];
    } stack_[max_size];
    TypeErasedDataT* stack_top_ = stack_;
};

} // namespace data_struct
} // namespace utility
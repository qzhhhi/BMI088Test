#pragma once
/**
 * @author Qzh (zihanqin2048@gmail.com)
 * @brief A simple circular queue (FIFO) structure implementation.
 * @note It is a well-defined behavior that pushing elements into the full queue, the queue will
 * release the first element to free up space.
 * @warning However, calling function pop() when the queue is empty is undefined behavior! Always
 * check empty before getting any element.
 * @warning It is not allowed to throw exceptions in any constructor or destructor functions of T.
 * Any exception thrown will turn to std::terminate due to the noexcept flag.
 * @copyright Copyright (c) 2023 by Alliance, All Rights Reserved.
 */

#include <atomic>
#include <cstddef>
#include <cstdint>

#include <new>
#include <utility>

#include "utility/immovable.hpp"

namespace utility {
namespace data_struct {

template <typename T, size_t max_size>
class CircularQueue : Immovable {
public:
    CircularQueue() noexcept
        : front_(nullptr)
        , rear_(&queue_[0]) {}

    ~CircularQueue() noexcept {
        while (!empty()) {
            pop();
        }
    }

    template <typename... Args>
    void push(Args&&... args) noexcept {
        if (full()) {
            // This operation is equivalent to calling the pop() function once.
            // But NEVER do this unless you change the 'else if' expression below to 'if', which
            // will bring some performance loss.
            TypeErasedDataT* front = front_.load(std::memory_order_relaxed);
            reinterpret_cast<T*>(front)->~T();
            front_.store(next(front), std::memory_order_relaxed);
        } else if (empty()) {
            front_.store(rear_.load(std::memory_order_relaxed), std::memory_order_relaxed);
        }

        new (rear_.load(std::memory_order_relaxed)) T{std::forward<Args>(args)...};
        rear_.store(next(rear_.load(std::memory_order_relaxed)), std::memory_order_relaxed);
    }

    T pop() noexcept {
        TypeErasedDataT* front = front_.load(std::memory_order_relaxed);
        T& element             = *reinterpret_cast<T*>(front);
        T moved_element        = std::move(element);
        element.~T();
        front_.store(next(front_.load(std::memory_order_relaxed)), std::memory_order_relaxed);
        if (full())
            front_.store(nullptr, std::memory_order_relaxed);
        return moved_element;
    }

    T& peek() noexcept { return *reinterpret_cast<T*>(front_.load(std::memory_order_relaxed)); }

    bool empty() noexcept { return front_.load(std::memory_order_relaxed) == nullptr; }

    bool full() noexcept {
        return front_.load(std::memory_order_relaxed) == rear_.load(std::memory_order_relaxed);
    }

    size_t size() noexcept {
        if (empty())
            return 0;

        auto size = rear_.load(std::memory_order_relaxed) - front_.load(std::memory_order_relaxed);
        if (size <= 0)
            size += max_size;

        return size;
    }

private:
    struct alignas(T) TypeErasedDataT {
        uint8_t data[sizeof(T)];
    } queue_[max_size];
    std::atomic<TypeErasedDataT*> front_, rear_;

    TypeErasedDataT* next(TypeErasedDataT* pointer) noexcept {
        auto next = pointer + 1;
        if (next == queue_ + max_size)
            next = queue_;
        return next;
    }
};

} // namespace data_struct
} // namespace utility
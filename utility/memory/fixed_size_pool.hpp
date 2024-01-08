#pragma once
/**
 * @author Qzh (zihanqin2048@gmail.com)
 * @brief A super lightweight fixed-size memory pool.
 * @copyright Copyright (c) 2023 by Alliance, All Rights Reserved.
 */

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace utility {
namespace memory {

template <size_t block_size, size_t max_block_count, size_t alignment = 4>
class FixedSizeMemoryPool {
public:
    using block_id_t                       = uint16_t;
    static constexpr block_id_t kNullBlock = UINT16_MAX;
    static_assert(max_block_count < UINT16_MAX);

    FixedSizeMemoryPool() {
        for (block_id_t i = 0; i < (block_id_t)max_block_count; i++) {
            id2block(i).next = i + 1;
        }
        id2block(max_block_count - 1).next = kNullBlock;
        stack_.store(Stack{0, 0}, std::memory_order_relaxed);
    }

    void* alloc() {
        Stack old_stack, new_stack;
        do {
            old_stack = stack_.load(std::memory_order_relaxed);
            if (old_stack.top == kNullBlock)
                return nullptr;
            new_stack.top   = id2block(old_stack.top).next;
            new_stack.stamp = old_stack.stamp + 1;
        } while (!stack_.compare_exchange_weak(old_stack, new_stack, std::memory_order_relaxed));
        return static_cast<void*>(&id2block(old_stack.top));
    }

    void dealloc(void* pointer) {
        Block& block = *static_cast<Block*>(pointer);
        Stack old_stack, new_stack;
        do {
            old_stack       = stack_.load(std::memory_order_relaxed);
            block.next      = old_stack.top;
            new_stack.top   = block2id(block);
            new_stack.stamp = old_stack.stamp + 1;
        } while (!stack_.compare_exchange_weak(old_stack, new_stack, std::memory_order_relaxed));
    }

    template <typename T, typename... Args>
    T* make(Args&&... args) {
        static_assert(sizeof(T) <= block_size);
        auto pointer = static_cast<T*>(alloc());
        if (pointer) {
            new (pointer) T{std::forward<Args>(args)...};
        }
        return pointer;
    }

    template <typename T>
    void destroy(T* pointer) {
        pointer->~T();
        dealloc(static_cast<void*>(pointer));
    }

private:
    struct Stack {
        block_id_t top;
        uint16_t stamp;
    };

    struct alignas(std::max(sizeof(block_id_t), alignment)) Block {
        uint8_t data[block_size];
        block_id_t next;
    };

    Block& id2block(block_id_t id) { return stack_body_[id]; }

    block_id_t block2id(const Block& block) {
        return static_cast<block_id_t>(&block - stack_body_);
    }

    static_assert(std::atomic<Stack>::is_always_lock_free);
    std::atomic<Stack> stack_;
    Block stack_body_[max_block_count];
};

} // namespace memory
} // namespace utility
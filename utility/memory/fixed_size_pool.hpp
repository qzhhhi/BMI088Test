#pragma once
/**
 * @author Qzh (zihanqin2048@gmail.com)
 * @brief A super lightweight fixed-size memory pool.
 * @copyright Copyright (c) 2023 by Alliance, All Rights Reserved.
 */

#include <cstddef>
#include <cstdint>

#include "utility/data_struct/stack.hpp"
#include "utility/immovable.hpp"

namespace utility {
namespace memory {

template <size_t block_size, size_t max_block_count, size_t alignment = 4>
class FixedSizeMemoryPool : Immovable {
public:
    FixedSizeMemoryPool() {
        for (size_t i = 0; i < max_block_count; i++) {
            free_block_stack_.push(static_cast<block_id_t>(i));
        }
    }

    void* alloc() {
        if (free_block_stack_.is_empty())
            return nullptr;

        return static_cast<void*>(&block_array_[free_block_stack_.pop()]);
    }

    void dealloc(void* ptr) {
        auto block_id = static_cast<Block*>(ptr) - block_array_;
        free_block_stack_.push(static_cast<block_id_t>(block_id));
    }

private:
    static constexpr auto get_block_id_t() {
        if constexpr (max_block_count > static_cast<uint64_t>(UINT32_MAX) + 1) {
            return static_cast<uint64_t>(0);
        } else {
            if constexpr (max_block_count > static_cast<uint32_t>(UINT16_MAX) + 1) {
                return static_cast<uint32_t>(0);
            } else {
                if constexpr (max_block_count > static_cast<uint16_t>(UINT8_MAX) + 1) {
                    return static_cast<uint16_t>(0);
                } else {
                    return static_cast<uint8_t>(0);
                }
            }
        }
    }

    using block_id_t = decltype(get_block_id_t());

    utility::data_struct::Stack<block_id_t, max_block_count> free_block_stack_;

    struct alignas(alignment) Block {
        uint8_t data[block_size];
    } block_array_[max_block_count];
};

} // namespace memory
} // namespace utility
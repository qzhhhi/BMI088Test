#pragma once
/**
 * @author Qzh (zihanqin2048@gmail.com)
 * @brief A encapsulation of fixed size pool. Strongly typed smart pointers available.
 * @details All objects of the same type share the same memory pool to avoid unnecessary waste of
 * memory space. Before allocating memory for a new type, the maximum number of elements of the pool
 * must be declared in typed_pool_declaration.hpp
 * @copyright Copyright (c) 2023 by Alliance, All Rights Reserved.
 */

#include <memory>
#include <new>

#include "utility/memory/fixed_size_pool.hpp"

namespace utility {
namespace memory {

template <typename T>
struct TypedPoolDeclaration {
    static_assert(
        0, "Before allocating memory for a new type, the maximum number of elements of the pool "
           "must be declared in typed_pool_declaration.hpp");
    static constexpr size_t max_element_count = 0;
};

template <typename T>
class TypedPool {
public:
    TypedPool() = delete;

    template <typename... Args>
    static auto make_unique(Args&&... args) {
        static auto pool = FixedSizeMemoryPool<
            sizeof(T), TypedPoolDeclaration<T>::max_element_count, alignof(T)>();
        auto destroy = [](T* ptr) {
            ptr->~T();
            pool.dealloc(static_cast<void*>(ptr));
        };
        auto raw_pointer = static_cast<T*>(pool.alloc());
        new (raw_pointer) T{std::forward<Args>(args)...};
        return std::unique_ptr<T, decltype(destroy)>(raw_pointer, destroy);
    }
};

} // namespace memory
} // namespace utility

// Prevent users from forgetting to include this file and causing undefined behavior.
#include "utility/memory/typed_pool_declaration.hpp"
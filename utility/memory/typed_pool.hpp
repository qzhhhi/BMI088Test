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
#include <utility>

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
private:
    static inline FixedSizeMemoryPool<
        sizeof(T), TypedPoolDeclaration<T>::max_element_count, alignof(T)>
        pool_{};
    struct Destroy {
        void operator()(T* ptr) {
            ptr->~T();
            pool_.dealloc(static_cast<void*>(ptr));
        }
    };

public:
    using UniquePtr = std::unique_ptr<T, Destroy>;

    TypedPool() = delete;

    static UniquePtr make_unique_from_raw(T* pointer) { return UniquePtr{pointer}; }

    template <typename... Args>
    static UniquePtr make_unique(Args&&... args) {
        return make_unique_from_raw(make_raw(std::forward<Args>(args)...));
    }

    template <typename... Args>
    static T* make_raw(Args&&... args) {
        auto pointer = static_cast<T*>(pool_.alloc());
        if (pointer) {
            new (pointer) T{std::forward<Args>(args)...};
        }
        return pointer;
    }

    static void destroy_raw(T* pointer) { destroy_(pointer); }

    static size_t free_count() { return pool_.free_count(); }
};

} // namespace memory
} // namespace utility

// Prevent users from forgetting to include this file and causing undefined behavior.
#include "utility/memory/typed_pool_declaration.hpp"
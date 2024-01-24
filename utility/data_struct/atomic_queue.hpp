#pragma once
/**
 * @author Qzh (zihanqin2048@gmail.com)
 * @brief Atomic linked queue with CAS
 * @copyright Copyright (c) 2024 by Alliance, All Rights Reserved.
 */

// #include "utility/memory/fixed_size_pool.hpp"
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace utility {
namespace data_struct {

// using T                          = int;
constexpr size_t block_size = 8;
constexpr size_t max_size   = 16;
constexpr size_t alignment  = 4;

class AtomicQueue {

    using node_id_t                      = uint16_t;
    static constexpr node_id_t kNullNode = UINT16_MAX;
    static_assert(max_size < UINT16_MAX);

    AtomicQueue() {
        for (node_id_t i = 0; i < (node_id_t)max_size; i++) {
            id2block(i).next = i + 1;
        }
        id2block(max_size - 1).next = kNullNode;
        stack_.store(Pointer{0, 0}, std::memory_order_relaxed);
    }

    void* alloc() {
        Pointer old_stack, new_stack;
        do {
            old_stack = stack_.load(std::memory_order_relaxed);
            if (old_stack.top == kNullNode)
                return nullptr;
            new_stack.top   = id2block(old_stack.top).next;
            new_stack.stamp = old_stack.stamp + 1;
        } while (!stack_.compare_exchange_weak(old_stack, new_stack, std::memory_order_relaxed));
        return static_cast<void*>(&id2block(old_stack.top));
    }

    void dealloc(void* pointer) {
        Block& block = *static_cast<Block*>(pointer);
        Pointer old_stack, new_stack;
        do {
            old_stack       = stack_.load(std::memory_order_relaxed);
            block.next      = old_stack.top;
            new_stack.top   = block2id(block);
            new_stack.stamp = old_stack.stamp + 1;
        } while (!stack_.compare_exchange_weak(old_stack, new_stack, std::memory_order_relaxed));
    }

    struct Pointer {
        node_id_t top;
        uint16_t stamp;
    };

    struct alignas(std::max(sizeof(node_id_t), alignment)) Block {
        uint8_t data[block_size];
        node_id_t next;
    };

    Block& id2block(node_id_t id) { return stack_body_[id]; }

    node_id_t block2id(const Block& block) { return static_cast<node_id_t>(&block - stack_body_); }

    static_assert(std::atomic<Pointer>::is_always_lock_free);
    std::atomic<Pointer> stack_;
    Block stack_body_[max_size];

    // using node_id_t                      = uint16_t;
    // static constexpr node_id_t kNullNode = UINT16_MAX;
    // static_assert(max_size < UINT16_MAX);

    // struct pointer_t {
    //     node_id_t ptr;
    //     uint16_t count;
    // };

    // struct node_t {
    //     T value;
    //     std::atomic<pointer_t> next;
    // };

    // struct queue_t {
    //     std::atomic<pointer_t> Head, Tail;
    // };

    // static_assert(std::atomic<pointer_t>::is_always_lock_free);

    // using MemoryPool = memory::FixedSizeMemoryPool<sizeof(node_t), max_size, alignof(node_t)>;
    // MemoryPool pool_;

    // node_id_t new_node() { return 0; }

    // // bool
    // //     CAS(std::atomic<pointer_t>& target, const pointer_t& expected, const pointer_t&
    // //     new_value) { return std::atomic_compare_exchange_strong(&target, &expected,
    // new_value);
    // // }

    // void initialize(queue_t* Q) {
    //     auto a = pool_.template make<node_t>();

    //                  node node = new_node();
    //     node->next.store({kNullNode, 0});
    //     Q->Head.store({
    //         ptr = Q->Tail.ptr = node;
    // }

    // void enqueue(queue_t* Q, T value) {
    //         node_t* node = new_node();
    //         node->value  = value;
    //         node->next.store(nullptr);

    //         while (true) {
    //             pointer_t tail = Q->Tail;
    //             pointer_t next = tail.ptr->next.load();

    //             if (tail == Q->Tail) {
    //                 if (next.ptr == nullptr) {
    //                     if (CAS(Q->Tail, tail, {node, tail.count + 1})) {
    //                         break;
    //                     }
    //                 } else {
    //                     CAS(Q->Tail, tail, {next.ptr, tail.count + 1});
    //                 }
    //             }
    //         }

    //         CAS(Q->Tail, {node, tail.count + 1}, {node, tail.count + 1});
    // }

    // bool dequeue(queue_t* Q, T* pvalue) {
    //         while (true) {
    //             pointer_t head = Q->Head;
    //             pointer_t tail = Q->Tail;
    //             node_t* next   = head.ptr->next.load();

    //             if (head == Q->Head) {
    //                 if (head.ptr == tail.ptr) {
    //                     if (next == nullptr) {
    //                         return false; // Queue is empty, couldn't dequeue
    //                     }
    //                     CAS(Q->Tail, tail, {next, tail.count + 1});
    //                 } else {
    //                     *pvalue = next->value;
    //                     if (CAS(Q->Head, head, {next, head.count + 1})) {
    //                         break;
    //                     }
    //                 }
    //             }
    //         }

    //         delete Q->Head.ptr;
    //         return true;
    // }
};

} // namespace data_struct
} // namespace utility

#pragma once
/**
 * @author Qzh (zihanqin2048@gmail.com)
 * @brief Imitation of ros2 topic.
 * @copyright Copyright (c) 2023 by Alliance, All Rights Reserved.
 */

#include <cassert>

#include "glue/double_buffer.hpp"
#include "utility/immovable.hpp"

namespace glue {
namespace topic {

// template <typename T>
// using T = int;
// class Topic {
// public:
//     struct Subscriber {
//         virtual bool callback(T& message) = 0;
//     };

//     class PublishMessageWrapper : utility::Immovable {
//     public:
//         PublishMessageWrapper(Topic* parent, T& message)
//             : parent_(parent)
//             , message_(message) {}

//         ~PublishMessageWrapper() { parent_->buffer_.finish_writing(); }

//         operator bool() const { return true; }
//         T* get() { return &message_; }
//         T* operator->() { return &message_; }
//         T& operator*() { return message_; }

//     private:
//         Topic* parent_;
//         T& message_;
//     };



//     template <typename... Args>
//     PublishMessageWrapper publish(Args&&... args) {
//         T& message = buffer_.start_writing();
//         new (&message) T{std::forward<Args>(args)...};
//         return PublishMessageWrapper{this, message};
//     }

//     void subscribe(Subscriber* subscriber) { subscriber_ = subscriber; }

//     bool execute() {
//         assert(subscriber_);

//         if (buffer_.readable()) {

//         }
//         auto message = buffer_.pop();
//         if (!message)
//             message = restored_message_;
//         if (!message)
//             return false;

//         bool restore = subscriber_->callback(*message);
//         if (restore) {
//             restored_message_ = message;
//             return false;
//         }

//         return true;
//     }

// private:
//     Subscriber* subscriber_ = nullptr;

//     DoubleBuffer<T> buffer_ = {};
//     T* restored_message_    = nullptr;
// };

// template <typename T>
// concept EnableLittleMessageOptimization =
//     (std::is_trivial_v<T>)&&(sizeof(T) <= 2 * sizeof(intptr_t));

// template <typename T>
// struct Subscriber {
//     using MemoryPool = utility::memory::TypedPool<T>;
//     using MessagePtr = MemoryPool::UniquePtr;

//     virtual MessagePtr callback(MessagePtr) = 0;
// };

// template <typename T, size_t max_size>
// class Topic {
// public:
//     using MemoryPool = utility::memory::TypedPool<T>;
//     using MessagePtr = MemoryPool::UniquePtr;

//     static constexpr bool enable_little_message_optimization = false;

//     template <typename... Args>
//     MessagePtr create_message(Args&&... args) {
//         return MemoryPool::make_unique(std::forward<Args>(args)...);
//     }

//     void publish(MessagePtr&& msg) {
//         assert(msg);
//         if (message_queue_.full())
//             first_message_released_ = true;

//         message_queue_.push(std::move(msg));
//     }

//     void subscribe(Subscriber<T>* subscriber) { subscriber_ = subscriber; }

//     bool execute() {
//         // todo: Critical Section
//         assert(subscriber_);

//         if (message_queue_.empty())
//             return false;

//         // utility::InterruptLock lock;

//         utility::InterruptMutex::lock();
//         auto msg                = std::move(message_queue_.peek());
//         first_message_released_ = false;
//         utility::InterruptMutex::unlock();

//         auto restore = subscriber_->callback(std::move(msg));

//         if (restore) {
//             utility::InterruptLockGuard guard;
//             if (!first_message_released_) {
//                 message_queue_.peek() = std::move(restore);
//             }
//             return false;
//         } else {
//             utility::InterruptLockGuard guard;
//             if (!first_message_released_) {
//                 message_queue_.pop();
//             }
//             return true;
//         }
//     }

// private:
//     utility::data_struct::CircularQueue<MessagePtr, max_size> message_queue_{};

//     Subscriber<T>* subscriber_;
//     bool first_message_released_;
// };

// template <typename T, size_t max_size>
// requires(EnableLittleMessageOptimization<T>)
// class Topic<T, max_size> {
// public:
//     static constexpr bool enable_little_message_optimization = true;

//     template <typename... Args>
//     void publish(Args&&... args) {
//         message_queue_.push(std::forward<Args>(args)...);
//     }

// private:
//     utility::data_struct::CircularQueue<T, max_size> message_queue_{};
// };

} // namespace topic
} // namespace glue
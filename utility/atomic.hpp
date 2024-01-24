#pragma once

// #include <atomic>

// namespace utility {

// template <typename T>
// class Atomic : private std::atomic<T> {
// public:
//     using std::atomic<T>::atomic;

//     void store(T desired, std::memory_order order = std::memory_order_relaxed) noexcept {
//         std::atomic<T>::store(desired, order);
//     }

//     T load(std::memory_order order = std::memory_order_seq_cst) const noexcept {
//         return std::atomic<T>::load(order);
//     }

//     operator T() const noexcept { return load(); }

//     T operator=(T desired) noexcept {
//         store(desired);
//         return desired;
//     }

//     Atomic& operator=(const Atomic&) = delete;

//     T fetch_add(T i, std::memory_order order = std::memory_order_relaxed) noexcept {
//         return std::atomic<T>::fetch_add(i, order);
//     }

//     T fetch_sub(T i, std::memory_order order = std::memory_order_relaxed) noexcept {
//         return std::atomic<T>::fetch_sub(i, order);
//     }

//     T fetch_and(T i, std::memory_order order = std::memory_order_relaxed) noexcept {
//         return std::atomic<T>::fetch_and(i, order);
//     }

//     T fetch_or(T i, std::memory_order order = std::memory_order_relaxed) noexcept {
//         return std::atomic<T>::fetch_or(i, order);
//     }

//     T fetch_xor(T i, std::memory_order order = std::memory_order_relaxed) noexcept {
//         return std::atomic<T>::fetch_xor(i, order);
//     }

//     int operator++() noexcept { return fetch_add(1) + 1; }
//     int operator++(int) noexcept { return fetch_add(1); }
//     int operator--() noexcept { return fetch_sub(1) - 1; }
//     int operator--(int) noexcept { return fetch_sub(1); }

//     T operator+=(T arg) noexcept { return fetch_add(arg) + arg; }
//     T operator-=(T arg) noexcept { return fetch_sub(arg) - arg; }
//     T operator&=(T arg) noexcept { return fetch_sub(arg) & arg; }
//     T operator|=(T arg) noexcept { return fetch_sub(arg) | arg; }
//     T operator^=(T arg) noexcept { return fetch_sub(arg) ^ arg; }

//     // T* operator+=(std::ptrdiff_t arg) noexcept;
//     // T* operator-=(std::ptrdiff_t arg) noexcept;
// };

// template <typename T>
// requires(std::is_integral_v<T>)
// class Atomic<T> : private std::atomic<T> {
// public:
//     using std::atomic<T>::atomic;

//     void store(T desired, std::memory_order order = std::memory_order_relaxed) noexcept {
//         std::atomic<T>::store(desired, order);
//     }

//     T load(std::memory_order order = std::memory_order_seq_cst) const noexcept {
//         return std::atomic<T>::load(order);
//     }

//     operator T() const noexcept { return load(); }

//     T operator=(T desired) noexcept {
//         store(desired);
//         return desired;
//     }

//     Atomic& operator=(const Atomic&) = delete;

//     T fetch_add(T i, std::memory_order order = std::memory_order_relaxed) noexcept {
//         return std::atomic<T>::fetch_add(i, order);
//     }

//     T fetch_sub(T i, std::memory_order order = std::memory_order_relaxed) noexcept {
//         return std::atomic<T>::fetch_sub(i, order);
//     }

//     T fetch_and(T i, std::memory_order order = std::memory_order_relaxed) noexcept {
//         return std::atomic<T>::fetch_and(i, order);
//     }

//     T fetch_or(T i, std::memory_order order = std::memory_order_relaxed) noexcept {
//         return std::atomic<T>::fetch_or(i, order);
//     }

//     T fetch_xor(T i, std::memory_order order = std::memory_order_relaxed) noexcept {
//         return std::atomic<T>::fetch_xor(i, order);
//     }

//     int operator++() noexcept { return fetch_add(1) + 1; }
//     int operator++(int) noexcept { return fetch_add(1); }
//     int operator--() noexcept { return fetch_sub(1) - 1; }
//     int operator--(int) noexcept { return fetch_sub(1); }

//     T operator+=(T arg) noexcept { return fetch_add(arg) + arg; }
//     T operator-=(T arg) noexcept { return fetch_sub(arg) - arg; }
//     T operator&=(T arg) noexcept { return fetch_sub(arg) & arg; }
//     T operator|=(T arg) noexcept { return fetch_sub(arg) | arg; }
//     T operator^=(T arg) noexcept { return fetch_sub(arg) ^ arg; }

//     __attribute__((always_inline)) T load_link() {
//         static_assert(sizeof(T) == sizeof(int));
//         T value;
//         asm volatile("ldrex %0, [%1]" : "=r"(value) : "r"(this));
//         return value;
//     }

//     __attribute__((always_inline)) int store_conditional(T value) {
//         static_assert(sizeof(T) == sizeof(int));
//         int result;
//         asm volatile("strex %0, %1, [%2]" : "=r"(result) : "r"(value), "r"(this));
//         return result;
//     }
// };

// template <typename T>
// requires(std::is_pointer_v<T>)
// class Atomic<T> : private std::atomic<T> {
// public:
//     using std::atomic<T>::atomic;

//     void store(T desired, std::memory_order order = std::memory_order_relaxed) noexcept {
//         std::atomic<T>::store(desired, order);
//     }

//     T load(std::memory_order order = std::memory_order_seq_cst) const noexcept {
//         return std::atomic<T>::load(order);
//     }

//     operator T() const noexcept { return load(); }

//     T operator=(T desired) noexcept {
//         store(desired);
//         return desired;
//     }

//     Atomic& operator=(const Atomic&) = delete;

//     T fetch_add(T i, std::memory_order order = std::memory_order_relaxed) noexcept {
//         return std::atomic<T>::fetch_add(i, order);
//     }

//     T fetch_sub(T i, std::memory_order order = std::memory_order_relaxed) noexcept {
//         return std::atomic<T>::fetch_sub(i, order);
//     }

//     T fetch_and(T i, std::memory_order order = std::memory_order_relaxed) noexcept {
//         return std::atomic<T>::fetch_and(i, order);
//     }

//     T fetch_or(T i, std::memory_order order = std::memory_order_relaxed) noexcept {
//         return std::atomic<T>::fetch_or(i, order);
//     }

//     T fetch_xor(T i, std::memory_order order = std::memory_order_relaxed) noexcept {
//         return std::atomic<T>::fetch_xor(i, order);
//     }

//     int operator++() noexcept { return fetch_add(1) + 1; }
//     int operator++(int) noexcept { return fetch_add(1); }
//     int operator--() noexcept { return fetch_sub(1) - 1; }
//     int operator--(int) noexcept { return fetch_sub(1); }

//     T* operator+=(std::ptrdiff_t arg) noexcept { return fetch_add(arg) + arg; }
//     T* operator-=(std::ptrdiff_t arg) noexcept { return fetch_add(arg) - arg; }

//     __attribute__((always_inline)) T load_link() {
//         T value;
//         asm volatile("ldrex %0, [%1]" : "=r"(value) : "r"(this));
//         return value;
//     }

//     __attribute__((always_inline)) int store_conditional(T value) {
//         int result;
//         asm volatile("strex %0, %1, [%2]" : "=r"(result) : "r"(value), "r"(this));
//         return result;
//     }
// };

// }; // namespace utility
#pragma once

#include <atomic>
#include <cstddef>

#include "utility/immovable.hpp"

namespace utility {

template <size_t max_size>
class InterruptSafeBuffer final : Immovable {
public:
    constexpr InterruptSafeBuffer() = default;

    std::byte* allocate(size_t size) {
        size_t written_size;

        do {
            written_size = written_size_.load(std::memory_order::relaxed);
            if (max_size - written_size < size)
                return nullptr;
        } while (!written_size_.compare_exchange_weak(
            written_size, written_size + size, std::memory_order::relaxed));

        return data_ + written_size;
    }

    std::byte* data() { return data_; }
    const std::byte* data() const { return data_; }

    size_t written_size() const { return written_size_.load(std::memory_order::relaxed); }
    void set_written_size(size_t new_written_size) {
        written_size_.store(new_written_size, std::memory_order::relaxed);
    }

    void clear() { set_written_size(0); }

private:
    std::atomic<size_t> written_size_ = 0;
    std::byte data_[max_size]{};
};

} // namespace utility
#pragma once

#include <cstddef>
#include <cstdint>

#include <new>
#include <numeric>

namespace device {
namespace usb {

constexpr size_t kPackageMaxSize = 64;

using package_head_t  = uint8_t;
using package_type_t  = uint8_t;
using package_index_t = uint8_t;
using package_size_t  = uint8_t;

using package_verify_code_t = uint8_t;
inline package_verify_code_t calculate_verify_code(const uint8_t* data, size_t size) {
    return std::accumulate(data, data + size, static_cast<package_verify_code_t>(0));
}

constexpr package_head_t kPackageHead = 0xAF;

struct __attribute__((packed)) PackageStaticPart final {
    package_head_t head;
    package_type_t type;
    package_index_t index;
    package_size_t data_size;
};

struct alignas(4) Package final {
    template <typename T, typename... Args>
    void init(package_type_t type, package_index_t index, Args... args) {
        static_part().head      = kPackageHead;
        static_part().type      = type;
        static_part().index     = index;
        static_part().data_size = sizeof(T);
        new (dymatic_part()) T{std::forward<Args>(args)...};
    }

    static constexpr size_t static_part_size() noexcept { return sizeof(PackageStaticPart); }

    PackageStaticPart& static_part() noexcept {
        return *reinterpret_cast<PackageStaticPart*>(buffer);
    }

    size_t dymatic_part_size() noexcept { return static_part().data_size; }

    uint8_t* dymatic_part() noexcept { return buffer + static_part_size(); }

    template <typename T>
    T& dymatic_part() noexcept {
        return *reinterpret_cast<T*>(dymatic_part());
    }

    using package_dymatic_buffer_t = uint8_t (&)[kPackageMaxSize - sizeof(PackageStaticPart)];
    package_dymatic_buffer_t dymatic_buffer() {
        return reinterpret_cast<package_dymatic_buffer_t>(*(buffer + static_part_size()));
    }

    static constexpr size_t verify_part_size() noexcept { return sizeof(package_verify_code_t); }

    package_verify_code_t& verify_part() noexcept {
        size_t length_without_verify = static_part_size() + dymatic_part_size();
        return *reinterpret_cast<package_verify_code_t*>(buffer + length_without_verify);
    }

    size_t size() noexcept { return static_part_size() + dymatic_part_size() + verify_part_size(); }

    uint8_t buffer[kPackageMaxSize];
};

} // namespace usb
} // namespace device
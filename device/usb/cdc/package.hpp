#pragma once

#include <cstddef>
#include <cstdint>

#include <new>
#include <numeric>

namespace device {
namespace usb {

constexpr size_t kPackageMaxSize = 64;

// using package_head_t  = uint8_t;
// using package_type_t  = uint8_t;
// using package_index_t = uint8_t;
// using package_size_t  = uint8_t;

// using package_verify_code_t = uint8_t;
// inline package_verify_code_t calculate_verify_code(const uint8_t* data, size_t size) {
//     return std::accumulate(data, data + size, static_cast<package_verify_code_t>(0));
// }

constexpr uint8_t kPackageHead = 0xAF;

struct __attribute__((packed)) PackageStaticPart final {
    uint8_t head;
    uint8_t type;
    uint8_t index;
    uint8_t data_size;
};

template <typename T>
class __attribute__((packed)) Package final {
public:
    Package(uint8_t type, uint8_t index = 0) {
        head_      = kPackageHead;
        type_      = type;
        index_     = index;
        data_size_ = sizeof(T);
    }

    uint8_t& type() { return type_; }
    uint8_t& index() { return index_; }
    T& data() { return data_; }

    const uint8_t* c_str() noexcept {
        uint8_t* str_this = reinterpret_cast<uint8_t*>(this);
        verify_ = std::accumulate(
            str_this, str_this + sizeof(*this) - sizeof(verify_), static_cast<uint8_t>(0));
        return str_this;
    }

private:
    uint8_t head_;
    uint8_t type_;
    uint8_t index_;
    uint8_t data_size_;
    T data_;
    uint8_t verify_;
};

// struct alignas(4) Package final {
//     template <typename T, typename... Args>
//     void init(package_type_t type, package_index_t index, Args... args) {
//         static_part().head      = kPackageHead;
//         static_part().type      = type;
//         static_part().index     = index;
//         static_part().data_size = sizeof(T);
//         new (dymatic_part()) T{std::forward<Args>(args)...};
//     }

//     static constexpr size_t static_part_size() noexcept { return sizeof(PackageStaticPart); }

//     PackageStaticPart& static_part() noexcept {
//         return *reinterpret_cast<PackageStaticPart*>(buffer);
//     }

//     size_t dymatic_part_size() noexcept { return static_part().data_size; }

//     uint8_t* dymatic_part() noexcept { return buffer + static_part_size(); }

//     template <typename T>
//     T& dymatic_part() noexcept {
//         return *reinterpret_cast<T*>(dymatic_part());
//     }

//     using package_dymatic_buffer_t = uint8_t (&)[kPackageMaxSize - sizeof(PackageStaticPart)];
//     package_dymatic_buffer_t dymatic_buffer() {
//         return reinterpret_cast<package_dymatic_buffer_t>(*(buffer + static_part_size()));
//     }

//     static constexpr size_t verify_part_size() noexcept { return sizeof(package_verify_code_t); }

//     package_verify_code_t& verify_part() noexcept {
//         size_t length_without_verify = static_part_size() + dymatic_part_size();
//         return *reinterpret_cast<package_verify_code_t*>(buffer + length_without_verify);
//     }

//     void update_verify_part() noexcept {
//         auto data = reinterpret_cast<package_verify_code_t*>(this);
//         auto code = std::accumulate(
//             data, data + static_part_size() + dymatic_part_size(),
//             static_cast<package_verify_code_t>(0));
//         verify_part() = code;
//     }

//     size_t size() noexcept { return static_part_size() + dymatic_part_size() +
//     verify_part_size(); }

//     uint8_t buffer[kPackageMaxSize];
// };

} // namespace usb
} // namespace device
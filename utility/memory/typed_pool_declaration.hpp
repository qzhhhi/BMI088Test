#pragma once
/**
 * @author Qzh (zihanqin2048@gmail.com)
 * @brief Declare the maximum number of elements of the typed pool.
 * @copyright Copyright (c) 2023 by Alliance, All Rights Reserved.
 */

#include "device/usb/cdc/package.hpp"
#include "utility/memory/typed_pool.hpp"

namespace utility {
namespace memory {

template <>
struct TypedPoolDeclaration<device::usb::Package> {
    static constexpr size_t max_element_count = 64;
};

} // namespace memory
} // namespace utility
#pragma once

#include "utility/lazy.hpp"

namespace device {
namespace usb {

class Cdc {};

inline utility::Lazy<Cdc> cdc;

} // namespace usb
} // namespace device
#pragma once

namespace utility {

class Immovable {
public:
    Immovable()                            = default;
    Immovable(const Immovable&)            = delete;
    Immovable& operator=(const Immovable&) = delete;
    Immovable(Immovable&&)                 = delete;
    Immovable& operator=(Immovable&&)      = delete;
};

} // namespace utility
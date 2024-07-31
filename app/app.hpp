#pragma once

#include "utility/immovable.hpp"
#include "utility/lazy.hpp"

namespace app {

class App : private utility::Immovable {
public:
    using Lazy = utility::Lazy<App>;

    App();

    [[noreturn]] void main();
};

inline constinit App::Lazy app;

} // namespace app
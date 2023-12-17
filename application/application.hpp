#pragma once

#include "utility/immovable.hpp"
#include "utility/lazy.hpp"

class Application : private utility::Immovable {
public:
    using Lazy = utility::Lazy<Application>;

    Application();

    void main();
};

inline Application::Lazy application;
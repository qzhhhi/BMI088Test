#pragma once

#include "utility/singleton/singleton.hpp"

class Application : private utility::Immovable {
public:
    friend class utility::Singleton<Application>;
    using Singleton = utility::Singleton<Application>;

    void main();

protected:
    Application();
};
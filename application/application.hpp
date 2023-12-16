#pragma once

#include "module/spi/bmi088/accel.hpp"
#include "module/spi/spi.hpp"
#include "utility/singleton.hpp"
#include "utility/immovable.hpp"

class Application : private utility::Immovable {
public:
    friend class utility::Singleton<Application>;
    using Singleton = utility::Singleton<Application>;

    void main();

    module::Spi spi;
    module::bmi088::Accelerometer accel;

private:
    Application();
};
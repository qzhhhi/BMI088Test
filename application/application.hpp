#pragma once

#include "module/spi/bmi088/accel.hpp"
#include "utility/lazy.hpp"
#include "utility/immovable.hpp"

class Application : private utility::Immovable {
public:
    Application();

    void main();

private:
    module::spi::bmi088::Accelerometer accel;
};

inline utility::Lazy<Application> application;
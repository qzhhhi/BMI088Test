#include "app/app.hpp"

#include <main.h>

#include "device/can/can.hpp"
#include "device/usb/cdc/cdc.hpp"

extern "C" {
void AppEntry() { app::app->main(); }
}

app::App::App() = default;

[[noreturn]] void app::App::main() {
    auto& cdc  = *device::usb::cdc;
    auto& can1 = *device::can::can1;
    auto& can2 = *device::can::can2;

    while (true) {
        cdc.try_transmit();
        can1.try_transmit();
        can2.try_transmit();
    }
}
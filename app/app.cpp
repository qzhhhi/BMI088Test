#include "app/app.hpp"

#include <main.h>

#include "device/can/can.hpp"
#include "device/spi/bmi088/accel.hpp"
#include "device/spi/bmi088/gyro.hpp"
#include "device/uart/uart.hpp"
#include "device/usb/cdc/cdc.hpp"

extern "C" {
void AppEntry() { app::app->main(); }
}

app::App::App() = default;

[[noreturn]] void app::App::main() {
    auto& cdc       = *device::usb::cdc;
    auto& can1      = *device::can::can1;
    auto& can2      = *device::can::can2;
    auto& uart1     = *device::uart::uart1;
    auto& uart2     = *device::uart::uart2;
    auto& uart_dbus = *device::uart::uart_dbus;
    auto& accel     = *device::spi::bmi088::accelerometer;
    auto& gyro      = *device::spi::bmi088::gyroscope;

    (void)accel;
    (void)gyro;

    while (true) {
        cdc.try_transmit();
        can1.try_transmit();
        cdc.try_transmit();
        can2.try_transmit();
        cdc.try_transmit();
        uart1.try_transmit();
        cdc.try_transmit();
        uart2.try_transmit();
        cdc.try_transmit();
        uart_dbus.try_transmit();
    }
}
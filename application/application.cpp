#include "application.hpp"

#include <main.h>
#include <spi.h>

#include "module/spi/bmi088/accel.hpp"
#include "module/spi/spi.hpp"
#include "stm32f4xx_hal.h"

void AppEntry() {
    auto& app = Application::Singleton::get_instance();
    app.main();
}

Application::Application() { HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET); }

void Application::main() {
    HAL_Delay(5000);

    module::bmi088::Accelerometer accel;
    using module::SpiTransmitReceiveMode;
    using module::bmi088::Accelerometer;

    while (true) {
        accel.get_value();
        HAL_Delay(100);
    }
}
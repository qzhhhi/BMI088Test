#include "application.hpp"

#include <main.h>
#include <spi.h>

#include "module/spi/bmi088/accel.hpp"
#include "module/timer/us_delay.hpp"

float acc_x, acc_y, acc_z;

void AppEntry() { application->main(); }

Application::Application() {
    // HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET);
}

void Application::main() {
    using namespace std::chrono_literals;
    module::spi::bmi088::accelerometer.get();
    while (true) {
        module::timer::us_delay(500ms);
        HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
    }
}
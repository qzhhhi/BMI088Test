#include "application.hpp"

#include <main.h>
#include <spi.h>

#include "module/spi/spi.hpp"
#include "module/timer/us_delay.hpp"

void AppEntry() {
    application->main();
}

Application::Application()
    : accel(*module::spi::spi1) {
    HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET);
}

void Application::main() {
    while (true) {
        using namespace std::chrono_literals;
        module::timer::us_delay(500ms);
        HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
        accel.get_value();
    }
}
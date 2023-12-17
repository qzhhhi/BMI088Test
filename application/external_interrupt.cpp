#include <cstdint>

#include <main.h>

#include "module/spi/bmi088/accel.hpp"

extern "C" {

void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin) {
    if (gpio_pin == INT1_ACC_Pin) {
        module::spi::bmi088::accelerometer.weak_execute(
            [](module::spi::bmi088::Accelerometer* accel) { accel->data_ready_callback(); });
    }
}

}; // extern "C"
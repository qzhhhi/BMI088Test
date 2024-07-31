#include <cstdint>

#include <main.h>

#include "device/spi/bmi088/accel.hpp"
#include "device/spi/bmi088/gyro.hpp"
#include "stm32f4xx_hal_gpio.h"

extern "C" {

void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin) {
    if (gpio_pin == INT1_ACC_Pin) {
        if (auto accel = device::spi::bmi088::accelerometer.try_get())
            accel->data_ready_callback();
    } else if (gpio_pin == INT1_GYRO_Pin) {
        if (auto gyro = device::spi::bmi088::gyroscope.try_get())
            gyro->data_ready_callback();
    }
}

}; // extern "C"
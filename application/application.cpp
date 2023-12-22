#include "application/application.hpp"

#include <main.h>

#include "device/spi/bmi088/accel.hpp"
#include "device/timer/us_delay.hpp"

float acc_x, acc_y, acc_z;

void AppEntry() { application->main(); }

Application::Application() { HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET); }

extern USBD_HandleTypeDef hUsbDeviceFS;

void Application::main() {
    using namespace std::chrono_literals;
    // device::timer::us_delay(5s);

    // static char string_buffer[128];
    // sprintf(string_buffer, "%ld\n", reinterpret_cast<long int>(b.get()));
    // USBD_CDC_SetTxBuffer(
    //     &hUsbDeviceFS, reinterpret_cast<uint8_t*>(string_buffer), strlen(string_buffer));
    // USBD_CDC_TransmitPacket(&hUsbDeviceFS);
    // device::timer::us_delay(1ms);

    device::spi::bmi088::accelerometer.get();
    while (true) {
        device::timer::us_delay(500ms);
        HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
    }
}
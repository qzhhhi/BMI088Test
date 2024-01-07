#include "application/application.hpp"

#include <atomic>
#include <main.h>

#include "device/spi/bmi088/accel.hpp"
#include "device/timer/us_delay.hpp"
#include "device/usb/cdc/cdc.hpp"
#include "device/usb/cdc/package.hpp"
#include "glue/topic/topic.hpp"
#include "utility/memory/typed_pool.hpp"

using namespace std::chrono_literals;

float acc_x, acc_y, acc_z;

void AppEntry() { application->main(); }

Application::Application() {
    device::timer::us_delay(5s);
    HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET);
}

extern "C" {
    extern volatile int free_count;
}

class MySubscriber : public glue::topic::Subscriber<device::usb::Package> {
    virtual MessagePtr callback(MessagePtr msg) {
        device::usb::cdc->transmit(msg);
        return msg;
    }
} my_subscriber;

void Application::main() {
    auto& acc = *device::spi::bmi088::accelerometer;
    acc.topic.subscribe(&my_subscriber);

    HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);

    while (true) {
        acc.topic.execute();
        free_count = utility::memory::TypedPool<device::usb::Package>::free_count();
    }

    // device::spi::bmi088::accelerometer.get();
    // while (true) {
    //     device::timer::us_delay(500ms);
    //     HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
    // }
}
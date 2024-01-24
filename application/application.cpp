#include "application/application.hpp"

#include <cassert>
#include <cstddef>
#include <main.h>
#include <utility>

#include "device/can/can.hpp"
#include "device/spi/bmi088/accel.hpp"
#include "device/spi/bmi088/gyro.hpp"
#include "device/usb/cdc/cdc.hpp"
#include "device/usb/cdc/package.hpp"
#include "glue/double_buffer.hpp"

using namespace std::chrono_literals;

float acc_x, acc_y, acc_z;

void AppEntry() { application->main(); }

Application::Application() {}

extern "C" {
extern volatile int free_count;
}

void Application::main() {
    auto& cdc = *device::usb::cdc;

    auto& can1 = *device::can::can1;
    can1.receive_buffer.construct_each((uint8_t)0x11);

    auto& acc  = *device::spi::bmi088::accelerometer;
    auto& gyro = *device::spi::bmi088::gyroscope;
    gyro.buffer.construct_each((uint8_t)0x31);

    while (true) {
        if (cdc.ready()) {
            if (can1.receive_buffer.readable()) {
                auto& package = can1.receive_buffer.read();
                cdc.transmit(package);
                continue;
            }
            if (gyro.buffer.readable()) {
                auto& acc_data       = acc.buffer.read();
                auto& package        = gyro.buffer.read();
                package.data().acc_x = acc_data.x;
                package.data().acc_y = acc_data.y;
                package.data().acc_z = acc_data.z;
                cdc.transmit(package);
                continue;
            }
        }
    }
}

// /* DWT (Data Watchpoint and Trace) registers, only exists on ARM Cortex with a DWT unit */

// /*!< DWT Control register */
// #define KIN1_DWT_CONTROL (*((volatile uint32_t*)0xE0001000))

// /*!< CYCCNTENA bit in DWT_CONTROL register */
// #define KIN1_DWT_CYCCNTENA_BIT (1UL << 0)

// /*!< DWT Cycle Counter register */
// #define KIN1_DWT_CYCCNT (*((volatile uint32_t*)0xE0001004))

// /*!< DEMCR: Debug Exception and Monitor Control Register */
// #define KIN1_DEMCR (*((volatile uint32_t*)0xE000EDFC))

// /*!< Trace enable bit in DEMCR register */
// #define KIN1_TRCENA_BIT (1UL << 24)

// /*!< TRCENA: Enable trace and debug block DEMCR (Debug Exception and Monitor Control Register */
// #define KIN1_InitCycleCounter() KIN1_DEMCR |= KIN1_TRCENA_BIT

// /*!< Reset cycle counter */
// #define KIN1_ResetCycleCounter() KIN1_DWT_CYCCNT = 0

// /*!< Enable cycle counter */
// #define KIN1_EnableCycleCounter() KIN1_DWT_CONTROL |= KIN1_DWT_CYCCNTENA_BIT

// /*!< Disable cycle counter */
// #define KIN1_DisableCycleCounter() KIN1_DWT_CONTROL &= ~KIN1_DWT_CYCCNTENA_BIT

// /*!< Read cycle counter register */
// #define KIN1_GetCycleCounter() KIN1_DWT_CYCCNT

// KIN1_InitCycleCounter();   /* enable DWT hardware */
// KIN1_ResetCycleCounter();  /* reset cycle counter */
// KIN1_EnableCycleCounter(); /* start counting */
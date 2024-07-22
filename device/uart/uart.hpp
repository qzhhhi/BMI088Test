#pragma once

#include "device/usb/cdc/package.hpp"
#include "glue/double_buffer.hpp"
#include "usart.h"
#include "utility/lazy.hpp"
#include <cassert>
#include <cstdint>

namespace device {
namespace uart {

class Uart {
public:
    using Lazy = utility::Lazy<Uart, UART_HandleTypeDef*>;

    explicit Uart(UART_HandleTypeDef* hal_uart_handle)
        : hal_uart_handle_(hal_uart_handle) {
        assert(
            HAL_UARTEx_ReceiveToIdle_IT(
                hal_uart_handle_, uart_receive_buffer, sizeof(uart_receive_buffer))
            == HAL_OK);

        cdc_transmit_buffer.construct_each();
    }

    void receive_callback(uint16_t size) {
        auto& buf = cdc_transmit_buffer.start_writing();
        buf.reset();
        buf.append(uart_receive_buffer, size);
        cdc_transmit_buffer.finish_writing();

        HAL_UARTEx_ReceiveToIdle_IT(
            hal_uart_handle_, uart_receive_buffer, sizeof(uart_receive_buffer));
    }

    glue::DoubleBuffer<device::usb::TransmitPackage<device::usb::Dynamic>> cdc_transmit_buffer;

private:
    UART_HandleTypeDef* hal_uart_handle_;

    uint8_t uart_receive_buffer[128];
};

inline constinit Uart::Lazy uart3{&huart3};

} // namespace uart
} // namespace device

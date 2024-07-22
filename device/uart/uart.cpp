#include "uart.hpp"
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include <cassert>

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* hal_uart_handle, uint16_t size) {
    HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
    if (__HAL_UART_GET_FLAG(hal_uart_handle, UART_FLAG_IDLE))
        return;
    if (hal_uart_handle->Instance == USART1) {
    } else if (hal_uart_handle->Instance == USART3) {
        if (auto uart = device::uart::uart3.try_get())
            uart->receive_callback(size);
    } else if (hal_uart_handle->Instance == USART6) {
    }
}
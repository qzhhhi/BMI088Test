/**
 * @author Qzh (zihanqin2048@gmail.com)
 * @brief The assertion error handling.
 * @copyright Copyright (c) 2023 by Alliance, All Rights Reserved.
 */

#include <cassert>

#include <main.h>

const char* __assert_file = nullptr;
int __assert_line = 0;
const char* __assert_function = nullptr;
const char* __assert_expression = nullptr;

void __assert_func(const char* file, int line, const char* function, const char* expression) {
    __disable_irq();
    __assert_file = file;
    __assert_line = line;
    __assert_function = function;
    __assert_expression = expression;
    HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET);
    while (true)
        ;
}
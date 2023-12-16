#pragma once

#include <main.h>
#include <tim.h>

#include "utility/immovable.hpp"
#include "utility/singleton.hpp"

template <TIM_HandleTypeDef* hal_timer_handle>
class Timer : utility::Immovable {
public:
    friend class utility::Singleton<Timer>;
    using Singleton = utility::Singleton<Timer>;

    auto get_tick() { return __HAL_TIM_GET_COUNTER(hal_timer_handle); }

private:
    Timer() { HAL_TIM_Base_Start(hal_timer_handle); };
};
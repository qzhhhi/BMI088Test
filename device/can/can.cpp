#include "device/can/can.hpp"

#include <can.h>

extern "C" {

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
    if (hcan == &hcan1) {
        if (auto can = device::can::can1.try_get())
            can->receive_callback();
    } else if (hcan == &hcan2) {
        if (auto can = device::can::can2.try_get())
            can->receive_callback();
    }
}

} // extern "C"
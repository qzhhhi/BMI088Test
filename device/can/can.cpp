#include "device/can/can.hpp"

#include <can.h>

extern "C" {

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
    if (hcan == &hcan1) {
        device::can::can1.weak_execute([](device::can::Can* self) { self->receive_callback(); });
    } else if (hcan == &hcan2) {
        device::can::can2.weak_execute([](device::can::Can* self) { self->receive_callback(); });
    }
}

} // extern "C"
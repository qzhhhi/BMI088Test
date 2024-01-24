#include "device/can/can.hpp"

#include <can.h>

extern "C" {

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
    // static CAN_Data can_rx_data;

    if (hcan == &hcan1) {
        device::can::can1.weak_execute([](device::can::Can* self) {
            self->receive_callback();
        });

        // HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &Rx_header, can_rx_data.data);
        // if (Rx_header.RTR == CAN_RTR_DATA && Rx_header.IDE == CAN_ID_STD && Rx_header.DLC ==
        // 0x08) {
        //     can_rx_data.StdId = Rx_header.StdId;
        //     can1_rx.Push(reinterpret_cast<uint8_t *>(&can_rx_data), sizeof(can_rx_data));
    }
}

// if (hcan->Instance == CAN1) {
// } else if (hcan->Instance == CAN2) {
//     HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &Rx_header, can_rx_data.data);
//     if (Rx_header.RTR == CAN_RTR_DATA && Rx_header.IDE == CAN_ID_STD && Rx_header.DLC == 0x08) {
//         can_rx_data.StdId = Rx_header.StdId;
//         can2_rx.Push(reinterpret_cast<uint8_t *>(&can_rx_data), sizeof(can_rx_data));
//     }
// }

} // extern "C"
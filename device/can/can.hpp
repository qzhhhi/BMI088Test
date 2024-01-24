#pragma once

#include "device/usb/cdc/package.hpp"
#include "glue/double_buffer.hpp"
#include "utility/lazy.hpp"
#include <can.h>

#include <cassert>
#include <utility/immovable.hpp>

namespace device {
namespace can {

class Can : private utility::Immovable {
public:
    Can(CAN_HandleTypeDef* hal_can_handle, uint32_t hal_filter_bank)
        : hal_can_handle_(hal_can_handle) {
        config_can(hal_filter_bank);

        hal_can_tx_header_.RTR = CAN_RTR_DATA;
        hal_can_tx_header_.IDE = CAN_ID_STD;
        hal_can_tx_header_.DLC = 8;
    }

    void receive_callback() {
        auto& buf = receive_buffer.start_writing();
        HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &hal_can_rx_header_, buf.data().data);
        if (hal_can_rx_header_.RTR == CAN_RTR_DATA && hal_can_rx_header_.DLC == 8) {
            if (hal_can_rx_header_.IDE == CAN_ID_STD)
                buf.data().id = hal_can_rx_header_.StdId;
            else
                buf.data().id = hal_can_rx_header_.ExtId;
            receive_buffer.finish_writing();
        }
    }

    bool try_transmit() {
        if (!transmit_buffer.readable())
            return false;
        if (HAL_CAN_GetTxMailboxesFreeLevel(hal_can_handle_) == 0)
            return false;

        auto& data               = transmit_buffer.read();
        hal_can_tx_header_.StdId = data.id;
        uint32_t tx_mailbox;
        HAL_CAN_AddTxMessage(hal_can_handle_, &hal_can_tx_header_, data.data, &tx_mailbox);
        return true;
    }

    struct __attribute__((packed)) Data {
        uint32_t id;
        uint8_t data[8];
    };

    glue::DoubleBuffer<usb::Package<Data>> receive_buffer;
    glue::DoubleBuffer<Data> transmit_buffer;

private:
    void config_can(uint32_t hal_filter_bank) {
        CAN_FilterTypeDef sFilterConfig;

        sFilterConfig.FilterBank           = hal_filter_bank;
        sFilterConfig.FilterMode           = CAN_FILTERMODE_IDMASK;
        sFilterConfig.FilterScale          = CAN_FILTERSCALE_32BIT;
        sFilterConfig.FilterIdHigh         = 0x0000;
        sFilterConfig.FilterIdLow          = 0x0000;
        sFilterConfig.FilterMaskIdHigh     = 0x0000;
        sFilterConfig.FilterMaskIdLow      = 0x0000;
        sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
        sFilterConfig.FilterActivation     = ENABLE;

        constexpr auto ok = HAL_OK;
        assert(HAL_CAN_ConfigFilter(hal_can_handle_, &sFilterConfig) == ok);
        assert(HAL_CAN_Start(hal_can_handle_) == ok);
        assert(HAL_CAN_ActivateNotification(hal_can_handle_, CAN_IT_RX_FIFO0_MSG_PENDING) == ok);
    }

    CAN_HandleTypeDef* hal_can_handle_;
    CAN_TxHeaderTypeDef hal_can_tx_header_;
    CAN_RxHeaderTypeDef hal_can_rx_header_;
};

inline utility::Lazy<Can, CAN_HandleTypeDef*, uint32_t> can1{&hcan1, 0};
inline utility::Lazy<Can, CAN_HandleTypeDef*, uint32_t> can2{&hcan2, 14};

} // namespace can
} // namespace device

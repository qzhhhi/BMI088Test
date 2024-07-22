#pragma once

#include <cstdint>

#include <atomic>

#include <usbd_cdc.h>

#include "device/can/can.hpp"
#include "device/usb/cdc/package.hpp"
#include "usbd_def.h"

namespace device::usb {

extern "C" {
extern USBD_HandleTypeDef hUsbDeviceFS;
}

class Cdc {
public:
    Cdc() = delete;

    static bool ready() {
        // The value of cdc_handle remains null until a USB connection occurs, and an interrupt
        // modifies it to a non-zero value. Therefore, atomic loading must be used here to prevent
        // compiler optimization.

        auto hal_cdc_handle_atomic =
            std::atomic_ref<void*>(hUsbDeviceFS.pClassDataCmsit[hUsbDeviceFS.classId]);
        void* hal_cdc_handle = hal_cdc_handle_atomic.load(std::memory_order_relaxed);

        if (!hal_cdc_handle)
            return false;

        return static_cast<USBD_CDC_HandleTypeDef*>(hal_cdc_handle)->TxState == 0U;
    }

    template <typename T>
    static void transmit(TransmitPackage<T>& package) {
        USBD_CDC_SetTxBuffer(&hUsbDeviceFS, const_cast<uint8_t*>(package.c_str()), package.size());
        USBD_CDC_TransmitPacket(&hUsbDeviceFS);
    }

private:
    friend constexpr USBD_CDC_ItfTypeDef get_hal_cdc_interfaces();

    static int8_t hal_cdc_init_callback() {
        USBD_CDC_SetRxBuffer(&hUsbDeviceFS, receive_package_.c_str());
        return USBD_OK;
    }

    static int8_t hal_cdc_deinit_callback() { return USBD_OK; }

    static int8_t hal_cdc_control_callback(uint8_t command, uint8_t* buffer, uint16_t length) {
        return USBD_OK;
    }

    // NOLINTNEXTLINE(readability-non-const-parameter) because the requirement of HAL api.
    static int8_t hal_cdc_receive_callback(uint8_t* buffer, uint32_t* length) {
        do {
            if (*length <= 4)
                break;
            if (!receive_package_.verify())
                break;
            if (receive_package_.size() != *length)
                break;

            if (receive_package_.type() == 0x11) {
                if (auto can = device::can::can1.try_get()) {
                    can->try_transmit(receive_package_.data<device::can::Can::Data>());
                }
            } else if (receive_package_.type() == 0x12) {
                if (auto can = device::can::can2.try_get()) {
                    can->try_transmit(receive_package_.data<device::can::Can::Data>());
                }
            }
        } while (false);

        USBD_CDC_SetRxBuffer(&hUsbDeviceFS, receive_package_.c_str());
        USBD_CDC_ReceivePacket(&hUsbDeviceFS);
        return USBD_OK;
    }

    static int8_t hal_cdc_transmit_complete_callback(
        uint8_t* buffer, uint32_t* length, uint8_t endpoint_num) {
        return USBD_OK;
    }

    inline static ReceivePackage receive_package_;

    // std::atomic<bool> task_created_;
    // utility::memory::TypedPool<Package>::UniquePtr package_;
};

} // namespace device::usb

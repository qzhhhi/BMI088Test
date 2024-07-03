// #pragma once

// #include <cassert>
// #include <cstdint>

// #include <atomic>

// #include <type_traits>
// #include <usbd_cdc.h>

// #include "device/can/can.hpp"
// #include "device/usb/cdc/package.hpp"
// #include "main.h"
// #include "stm32f4xx_hal_gpio.h"
// #include "usbd_def.h"
// #include "utility/immovable.hpp"
// #include "utility/lazy.hpp"

// namespace device {
// namespace usb {

// extern "C" {
// extern USBD_HandleTypeDef hUsbDeviceFS;
// }

// extern "C" {
// extern volatile int free_count;
// }

// class Cdc {
// public:
//     Cdc() = delete;

//     static bool ready() {
//         // The value of cdc_handle remains null until a USB connection occurs, and an interrupt
//         // modifies it to a non-zero value. Therefore, atomic loading must be used here to prevent
//         // compiler optimization.
//         const auto& cdc_handle_atomic = *reinterpret_cast<std::atomic<USBD_CDC_HandleTypeDef*>*>(
//             &hUsbDeviceFS.pClassDataCmsit[hUsbDeviceFS.classId]);
//         auto cdc_handle = cdc_handle_atomic.load(std::memory_order_relaxed);
//         if (!cdc_handle)
//             return false;

//         return cdc_handle->TxState == 0U;
//     }

//     template <typename T>
//     static void transmit(TransmitPackage<T>& package) {
//         USBD_CDC_SetTxBuffer(&hUsbDeviceFS, const_cast<uint8_t*>(package.c_str()), package.size());
//         USBD_CDC_TransmitPacket(&hUsbDeviceFS);
//     }

// private:
//     friend constexpr USBD_CDC_ItfTypeDef get_hal_cdc_interfaces();

//     static int8_t hal_cdc_init() {
//         USBD_CDC_SetRxBuffer(&hUsbDeviceFS, receive_package_.c_str());
//         return USBD_OK;
//     }

//     static int8_t hal_cdc_deinit() { return USBD_OK; }

//     static int8_t hal_cdc_control(uint8_t command, uint8_t* buffer, uint16_t length) {
//         return USBD_OK;
//     }

//     static int8_t hal_cdc_receive(uint8_t* buffer, uint32_t* length) {
//         do {
//             // free_count = *length;

//             static uint32_t last_tick;
//             static int count;
//             uint32_t now_tick = HAL_GetTick();
//             if (now_tick - last_tick > 1000) {
//                 last_tick  = now_tick;
//                 free_count = count;
//                 count      = 0;
//             }

//             count++;

//             if (*length <= 4)
//                 break;
//             if (!receive_package_.verify())
//                 break;
//             if (receive_package_.size() != *length)
//                 break;

//             if (receive_package_.type() == 0x11) {
//                 device::can::can1.weak_execute([](device::can::Can* self) {
//                     assert(self->try_transmit(receive_package_.data<device::can::Can::Data>()));
//                 });
//             } else if (receive_package_.type() == 0x12) {
//                 device::can::can2.weak_execute([](device::can::Can* self) {
//                     assert(self->try_transmit(receive_package_.data<device::can::Can::Data>()));
//                 });
//             }
//         } while (false);

//         // USBD_CDC_SetTxBuffer(&hUsbDeviceFS, const_cast<uint8_t*>(buffer), *length);
//         // USBD_CDC_TransmitPacket(&hUsbDeviceFS);

//         USBD_CDC_SetRxBuffer(&hUsbDeviceFS, receive_package_.c_str());
//         USBD_CDC_ReceivePacket(&hUsbDeviceFS);
//         return USBD_OK;
//     }

//     static int8_t hal_cdc_transmit_complete(uint8_t* buffer, uint32_t* length, uint8_t epnum) {
//         return USBD_OK;
//     }

//     inline static ReceivePackage receive_package_;

//     // std::atomic<bool> task_created_;
//     // utility::memory::TypedPool<Package>::UniquePtr package_;
// };

// } // namespace usb
// } // namespace device
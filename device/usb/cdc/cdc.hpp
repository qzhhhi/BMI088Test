#pragma once

#include <cstdint>

#include <atomic>

#include <usbd_cdc.h>

#include "device/usb/cdc/package.hpp"
#include "utility/immovable.hpp"
#include "utility/lazy.hpp"

namespace device {
namespace usb {

extern "C" {
extern USBD_HandleTypeDef hUsbDeviceFS;
}

class Cdc : private utility::Immovable {
public:
    using Lazy = utility::Lazy<Cdc>;

    Cdc(){

    };

    // bool transmit(utility::memory::TypedPool<Package>::UniquePtr& package) {
    //     if (__sync_bool_compare_and_swap(&task_created_, false, true)) {
    //         if (hal_ready()) {
    //             package_ = std::move(package);

    //             USBD_CDC_SetTxBuffer(&hUsbDeviceFS, package_->buffer, package_->size());
    //             USBD_CDC_TransmitPacket(&hUsbDeviceFS);
    //             task_created_ = false;

    //             return true;
    //         } else {
    //             task_created_ = false;
    //         }
    //     }
    //     return false;
    // }

    // class TransmitTask : private utility::Uncopyable {
    // public:
    //     friend class Cdc;

    //     TransmitTask(TransmitTask&& task) noexcept
    //         : parent_(task.parent_) {
    //         task.parent_ = nullptr;
    //     }
    //     TransmitTask& operator=(TransmitTask&& task) = delete;

    //     operator bool() const noexcept { return parent_; }

    //     void set_package(Package& package) { package_ = &package; }

    //     ~TransmitTask() noexcept {
    //         if (*this && package_) {
    //             USBD_CDC_SetTxBuffer(
    //                 &hUsbDeviceFS, parent_->package_->buffer, parent_->package_->size());
    //             USBD_CDC_TransmitPacket(&hUsbDeviceFS);

    //             parent_->task_created_ = false;
    //         }
    //     }

    // private:
    //     TransmitTask() noexcept
    //         : parent_(nullptr) {}

    //     TransmitTask(Cdc* parent) noexcept
    //         : parent_(parent) {}

    //     Cdc* parent_;
    //     Package* package_ = nullptr;
    // };

    // TransmitTask create_transmit_task() {
    //     bool expected = false;
    //     if (task_created_.compare_exchange_strong(expected, true, std::memory_order_acquire)) {
    //         if (hal_ready()) {
    //             return TransmitTask{this};
    //         } else {
    //             task_created_ = false;
    //         }
    //     }
    //     return TransmitTask{};
    // }
    bool ready() const {
        // The value of cdc_handle remains null until a USB connection occurs, and an interrupt
        // modifies it to a non-zero value. Therefore, atomic loading must be used here to prevent
        // compiler optimization.
        const auto& cdc_handle_atomic = *reinterpret_cast<std::atomic<USBD_CDC_HandleTypeDef*>*>(
            &hUsbDeviceFS.pClassDataCmsit[hUsbDeviceFS.classId]);
        auto cdc_handle = cdc_handle_atomic.load(std::memory_order_relaxed);
        if (!cdc_handle)
            return false;

        return cdc_handle->TxState == 0U;
    }

    template <typename T>
    void transmit(Package<T>& package) {
        USBD_CDC_SetTxBuffer(&hUsbDeviceFS, const_cast<uint8_t*>(package.c_str()), sizeof(package));
        USBD_CDC_TransmitPacket(&hUsbDeviceFS);
    }

private:
    // std::atomic<bool> task_created_;
    // utility::memory::TypedPool<Package>::UniquePtr package_;
};

inline Cdc::Lazy cdc;

} // namespace usb
} // namespace device
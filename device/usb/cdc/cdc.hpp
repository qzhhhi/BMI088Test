#pragma once

#include <memory>
#include <usbd_cdc.h>

#include "device/usb/cdc/package.hpp"
#include "utility/immovable.hpp"
#include "utility/lazy.hpp"
#include "utility/memory/typed_pool.hpp"

namespace device {
namespace usb {

extern "C" {
extern USBD_HandleTypeDef hUsbDeviceFS;
}

class Cdc : private utility::Immovable {
public:
    using Lazy = utility::Lazy<Cdc>;

    Cdc() = default;

    bool transmit(utility::memory::TypedPool<Package>::UniquePtr& package) {
        if (__sync_bool_compare_and_swap(&task_created_, false, true)) {
            if (hal_ready()) {
                package_ = std::move(package);

                USBD_CDC_SetTxBuffer(&hUsbDeviceFS, package_->buffer, package_->size());
                USBD_CDC_TransmitPacket(&hUsbDeviceFS);
                task_created_ = false;

                return true;
            } else {
                task_created_ = false;
            }
        }
        return false;
    }

    // class TransmitTask : private utility::Uncopyable {
    // public:
    //     friend class Cdc;

    //     TransmitTask(TransmitTask&& task) noexcept
    //         : handle_(task.handle_) {
    //         task.handle_ = nullptr;
    //     }
    //     TransmitTask& operator=(TransmitTask&& task) = delete;

    //     operator bool() const noexcept { return handle_ != nullptr; }

    //     ~TransmitTask() noexcept {
    //         if (*this) {
    //             USBD_CDC_SetTxBuffer(
    //                 &hUsbDeviceFS, handle_->package_->buffer, handle_->package_->size());
    //             USBD_CDC_TransmitPacket(&hUsbDeviceFS);

    //             handle_->task_created_ = false;
    //         }
    //     }

    // private:
    //     TransmitTask() noexcept
    //         : handle_(nullptr) {}

    //     TransmitTask(Cdc* handle) noexcept
    //         : handle_(handle) {}

    //     Cdc* handle_;
    // };

private:
    bool hal_ready() const {
        USBD_CDC_HandleTypeDef* cdc_handle = static_cast<USBD_CDC_HandleTypeDef*>(
            hUsbDeviceFS.pClassDataCmsit[hUsbDeviceFS.classId]);
        return cdc_handle->TxState == 0U;
    }

    bool task_created_;
    utility::memory::TypedPool<Package>::UniquePtr package_;
};

inline Cdc::Lazy cdc;

} // namespace usb
} // namespace device
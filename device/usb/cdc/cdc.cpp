#include "cdc.hpp"

namespace device {
namespace usb {

constexpr USBD_CDC_ItfTypeDef get_hal_cdc_interfaces() {
    return {
        &Cdc::hal_cdc_init_callback, &Cdc::hal_cdc_deinit_callback, &Cdc::hal_cdc_control_callback, &Cdc::hal_cdc_receive_callback,
        &Cdc::hal_cdc_transmit_complete_callback};
}

extern "C" {
USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = get_hal_cdc_interfaces();
}

} // namespace usb
} // namespace device

// #include "cdc.hpp"

// namespace device {
// namespace usb {

// constexpr USBD_CDC_ItfTypeDef get_hal_cdc_interfaces() {
//     return {
//         &Cdc::hal_cdc_init, &Cdc::hal_cdc_deinit, &Cdc::hal_cdc_control, &Cdc::hal_cdc_receive,
//         &Cdc::hal_cdc_transmit_complete};
// }

// extern "C" {
// USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = get_hal_cdc_interfaces();
// }

// } // namespace usb
// } // namespace device

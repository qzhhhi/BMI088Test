#pragma once

#include <cassert>
#include <cstdio>
#include <optional>

#include <spi.h>

#include "utility/immovable.hpp"
#include "utility/lazy.hpp"

// extern USBD_HandleTypeDef hUsbDeviceFS;
// inline static void print_buffer(char flag, uint8_t* buffer, size_t size) {
//     static char string_buffer[128];
//     char* p = string_buffer;

//     *p++ = flag;
//     *p++ = ' ';

//     for (size_t i = 0; i < size; i++) {
//         sprintf(p, "%02X ", buffer[i]);
//         p += 3;
//     }

//     *p++ = '\n';

//     USBD_CDC_SetTxBuffer(
//         &hUsbDeviceFS, reinterpret_cast<uint8_t*>(string_buffer), p - string_buffer);
//     USBD_CDC_TransmitPacket(&hUsbDeviceFS);
// }

namespace device {
namespace spi {

class SpiModuleInterface {
public:
    SpiModuleInterface(GPIO_TypeDef* _chip_select_port, uint16_t _chip_select_pin)
        : chip_select_port(_chip_select_port)
        , chip_select_pin(_chip_select_pin) {}

    virtual void transmit_receive_callback(uint8_t* rx_buffer, size_t size) = 0;

    GPIO_TypeDef* const chip_select_port;
    const uint16_t chip_select_pin;
};

enum class SpiTransmitReceiveMode { BLOCK, INTERRUPT };

class Spi : private utility::Immovable {
public:
    using Lazy = utility::Lazy<Spi, SPI_HandleTypeDef*>;

    Spi(SPI_HandleTypeDef* hal_spi_handle)
        : hal_spi_handle_(hal_spi_handle)
        , task_created_(false)
        , spi_module_(nullptr)
        , tx_rx_size_(0) {}

    template <SpiTransmitReceiveMode mode>
    class TransmitReceiveTask {
    public:
        friend class Spi;

        TransmitReceiveTask(const TransmitReceiveTask&)            = delete;
        TransmitReceiveTask& operator=(const TransmitReceiveTask&) = delete;

        TransmitReceiveTask(TransmitReceiveTask&& task)
            : tx_buffer(task.tx_buffer)
            , spi_(task.spi_) {
            task.spi_ = nullptr;
        }
        TransmitReceiveTask& operator=(TransmitReceiveTask&& task) = delete;

        ~TransmitReceiveTask() {
            if (spi_ != nullptr) {
                spi_->start_transmit_receive();

                // print_buffer('<', spi_->tx_data_buffer_, spi_->tx_rx_size_);

                if constexpr (mode == SpiTransmitReceiveMode::BLOCK) {
                    HAL_SPI_TransmitReceive(
                        spi_->hal_spi_handle_, spi_->tx_data_buffer_, spi_->rx_data_buffer_,
                        spi_->tx_rx_size_, HAL_MAX_DELAY);
                    spi_->transmit_receive_callback();
                } else if constexpr (mode == SpiTransmitReceiveMode::INTERRUPT) {
                    HAL_SPI_TransmitReceive_IT(
                        spi_->hal_spi_handle_, spi_->tx_data_buffer_, spi_->rx_data_buffer_,
                        spi_->tx_rx_size_);
                }

                spi_->task_created_ = false;
            }
        }

        uint8_t* tx_buffer;

    private:
        TransmitReceiveTask(Spi* spi)
            : tx_buffer(spi->tx_data_buffer_)
            , spi_(spi) {}

        Spi* spi_;
    };

    template <SpiTransmitReceiveMode mode>
    std::optional<TransmitReceiveTask<mode>>
        create_transmit_receive_task(SpiModuleInterface* module, size_t size) {

        assert(max_buffer_size_ >= size);

        if (__sync_bool_compare_and_swap(&task_created_, false, true)) {
            if (hal_ready()) {
                // The function transmit_receive_callback has a very low probability of not being
                // called. Therefore, before creating a new task, release the chip select pin again
                // to ensure correct data transmission.
                if (spi_module_)
                    stop_transmit_receive();

                spi_module_ = module;
                tx_rx_size_ = size;
                return TransmitReceiveTask<mode>(this);
            } else {
                task_created_ = false;
            }
        }
        return std::nullopt;
    }

    void transmit_receive_callback() {
        stop_transmit_receive();

        // print_buffer('>', rx_data_buffer_, tx_rx_size_);

        spi_module_->transmit_receive_callback(rx_data_buffer_, tx_rx_size_);
    }

private:
    bool hal_ready() const { return hal_spi_handle_->State == HAL_SPI_STATE_READY; }

    void start_transmit_receive() {
        HAL_GPIO_WritePin(
            spi_module_->chip_select_port, spi_module_->chip_select_pin, GPIO_PIN_RESET);
    }

    void stop_transmit_receive() {
        HAL_GPIO_WritePin(
            spi_module_->chip_select_port, spi_module_->chip_select_pin, GPIO_PIN_SET);
    }

    SPI_HandleTypeDef* hal_spi_handle_;

    bool task_created_;

    SpiModuleInterface* spi_module_;
    size_t tx_rx_size_;

    static constexpr size_t max_buffer_size_ = 16;
    alignas(4) uint8_t tx_data_buffer_[max_buffer_size_];
    alignas(4) uint8_t rx_data_buffer_[max_buffer_size_];
};

inline Spi::Lazy spi1(&hspi1);

} // namespace spi
} // namespace device
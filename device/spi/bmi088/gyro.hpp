#pragma once

#include <cassert>
#include <cstdint>

#include <cstdio>
#include <spi.h>
#include <usbd_cdc.h>

#include "device/spi/spi.hpp"
#include "device/timer/us_delay.hpp"
#include "device/usb/cdc/package.hpp"
#include "glue/double_buffer.hpp"

extern float acc_x, acc_y, acc_z;

namespace device {
namespace spi {
namespace bmi088 {

class Gyroscope : SpiModuleInterface {
public:
    using Lazy = utility::Lazy<Gyroscope, Spi::Lazy*>;

    enum class DataRange : uint8_t {
        _2000 = 0x00,
        _1000 = 0x01,
        _500  = 0x02,
        _250  = 0x03,
        _125  = 0x04
    };
    enum class DataRateAndBandwidth : uint8_t {
        _2000_532 = 0x00,
        _2000_230 = 0x01,
        _1000_116 = 0x02,
        _400_47   = 0x03,
        _200_23   = 0x04,
        _100_12   = 0x05,
        _200_64   = 0x06,
        _100_32   = 0x07
    };

    struct __attribute__((packed)) Data {
        int16_t x;
        int16_t y;
        int16_t z;
    };

    struct __attribute__((packed)) ImuData {
        int16_t gyro_x, gyro_y, gyro_z;
        int16_t acc_x, acc_y, acc_z;
    };

    Gyroscope(
        Spi::Lazy* spi, DataRange range = DataRange::_2000,
        DataRateAndBandwidth rate = DataRateAndBandwidth::_2000_230)
        : SpiModuleInterface(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin)
        , spi_(spi->get())
        , initialized_(false)
        , init_rx_buffer_(nullptr)
        , init_rx_size_(0) {

        using namespace std::chrono_literals;

        constexpr int max_try_time = 3;

        auto read_with_confirm = [this](RegisterAddress address, uint8_t value) {
            for (int i = max_try_time; i-- > 0;) {
                if (!read<SpiTransmitReceiveMode::BLOCK>(address, 1))
                    return false;
                assert(init_rx_buffer_ && init_rx_size_ == 2);
                if (init_rx_buffer_[1] == value)
                    return true;
                device::timer::us_delay(1ms);
            }
            return false;
        };
        auto write_with_confirm = [this](RegisterAddress address, uint8_t value) {
            for (int i = max_try_time; i-- > 0;) {
                if (!write<SpiTransmitReceiveMode::BLOCK>(address, value))
                    return false;
                device::timer::us_delay(1ms);
                if (!read<SpiTransmitReceiveMode::BLOCK>(address, 1))
                    return false;
                assert(init_rx_buffer_ && init_rx_size_ == 2);
                if (init_rx_buffer_[1] == value)
                    return true;
            }
            return false;
        };

        // Reset all registers to reset value.
        write<SpiTransmitReceiveMode::BLOCK>(RegisterAddress::GYRO_SOFTRESET, 0xB6);
        device::timer::us_delay(1ms);

        // "Who am I" check.
        assert(read_with_confirm(RegisterAddress::GYRO_CHIP_ID, 0x0F));

        // Enables the new data interrupt.
        assert(write_with_confirm(RegisterAddress::GYRO_INT_CTRL, 0x80));

        // Set both INT3 and INT4 as push-pull, active-low, even though only INT3 is used.
        assert(write_with_confirm(RegisterAddress::INT3_INT4_IO_CONF, 0b0000));
        // Map data ready interrupt to INT3 pin.
        assert(write_with_confirm(RegisterAddress::INT3_INT4_IO_MAP, 0x01));

        // Set ODR (output data rate, Hz) and filter bandwidth (Hz).
        assert(
            write_with_confirm(RegisterAddress::GYRO_BANDWIDTH, 0x80 | static_cast<uint8_t>(rate)));
        // Set data range.
        assert(write_with_confirm(RegisterAddress::GYRO_RANGE, static_cast<uint8_t>(range)));

        // Switch the main power mode into normal mode.
        assert(write_with_confirm(RegisterAddress::GYRO_LPM1, 0x00));

        initialized_ = true;
    }

    static constexpr auto hal_spi_handle = &hspi1;

    void transmit_receive_callback(uint8_t* rx_buffer, size_t size) override {
        if (initialized_) {
            assert(size == sizeof(Data) + 1);
            auto& data = *reinterpret_cast<Data*>(rx_buffer + 1);
            auto& buf  = buffer.start_writing();

            buf.data().gyro_x = data.x;
            buf.data().gyro_y = data.y;
            buf.data().gyro_z = data.z;

            buffer.finish_writing();
        } else {
            init_rx_buffer_ = rx_buffer;
            init_rx_size_   = size;
        }
    }

    void data_ready_callback() {
        read<SpiTransmitReceiveMode::BLOCK>(RegisterAddress::RATE_X_LSB, 6);
    }

    glue::DoubleBuffer<usb::Package<ImuData>> buffer;

private:
    enum class RegisterAddress : uint8_t {
        GYRO_SELF_TEST    = 0x3C,
        INT3_INT4_IO_MAP  = 0x18,
        INT3_INT4_IO_CONF = 0x16,
        GYRO_INT_CTRL     = 0x15,
        GYRO_SOFTRESET    = 0x14,
        GYRO_LPM1         = 0x11,
        GYRO_BANDWIDTH    = 0x10,
        GYRO_RANGE        = 0x0F,
        GYRO_INT_STAT_1   = 0x0A,
        RATE_Z_MSB        = 0x07,
        RATE_Z_LSB        = 0x06,
        RATE_Y_MSB        = 0x05,
        RATE_Y_LSB        = 0x04,
        RATE_X_MSB        = 0x03,
        RATE_X_LSB        = 0x02,
        GYRO_CHIP_ID      = 0x00
    };

    template <SpiTransmitReceiveMode mode>
    bool write(RegisterAddress address, uint8_t value) {
        if (auto task = spi_->create_transmit_receive_task<mode>(this, 2)) {
            task->tx_buffer[0] = static_cast<uint8_t>(address);
            task->tx_buffer[1] = value;
            return true;
        }
        return false;
    }

    template <SpiTransmitReceiveMode mode>
    bool read(RegisterAddress address, size_t read_size) {
        if (auto task = spi_->create_transmit_receive_task<mode>(this, read_size + 1)) {
            task->tx_buffer[0] = 0x80 | static_cast<uint8_t>(address);
            return true;
        }
        return false;
    }

    Spi* const spi_;

    bool initialized_;

    uint8_t* init_rx_buffer_;
    size_t init_rx_size_;
};

inline Gyroscope::Lazy gyroscope(&spi1);

} // namespace bmi088
} // namespace spi
} // namespace device
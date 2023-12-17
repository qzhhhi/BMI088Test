#pragma once

#include <cassert>
#include <cstdint>

#include <cstdio>
#include <spi.h>
#include <usbd_cdc.h>

#include "module/spi/spi.hpp"
#include "module/timer/us_delay.hpp"

extern USBD_HandleTypeDef hUsbDeviceFS;
extern float acc_x, acc_y, acc_z;

namespace module {
namespace spi {
namespace bmi088 {

class Accelerometer : SpiModuleInterface {
public:
    using Lazy = utility::Lazy<Accelerometer, Spi::Lazy*>;

    enum class Range : uint8_t { _3G = 0x00, _6G = 0x01, _12G = 0x02, _24G = 0x03 };
    enum class DataRate : uint8_t {
        _12   = 0x05,
        _25   = 0x06,
        _50   = 0x07,
        _100  = 0x08,
        _200  = 0x09,
        _400  = 0x0A,
        _800  = 0x0B,
        _1600 = 0x0C
    };

    Accelerometer(Spi::Lazy* spi, Range range = Range::_6G, DataRate data_rate = DataRate::_1600)
        : SpiModuleInterface(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin)
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
                assert(init_rx_buffer_ && init_rx_size_ == 3);
                if (init_rx_buffer_[2] == value)
                    return true;
                module::timer::us_delay(1ms);
            }
            return false;
        };
        auto write_with_confirm = [this](RegisterAddress address, uint8_t value) {
            for (int i = max_try_time; i-- > 0;) {
                if (!write<SpiTransmitReceiveMode::BLOCK>(address, value))
                    return false;
                module::timer::us_delay(1ms);
                if (!read<SpiTransmitReceiveMode::BLOCK>(address, 1))
                    return false;
                assert(init_rx_buffer_ && init_rx_size_ == 3);
                if (init_rx_buffer_[2] == value)
                    return true;
            }
            return false;
        };

        // Dummy read to switch accelerometer to SPI mode
        read<SpiTransmitReceiveMode::BLOCK>(RegisterAddress::ACC_CHIP_ID, 1);
        module::timer::us_delay(1ms);

        // Reset all registers to reset value
        write<SpiTransmitReceiveMode::BLOCK>(RegisterAddress::ACC_SOFTRESET, 0xB6);
        module::timer::us_delay(1ms);

        // "Who am I" check.
        assert(read_with_confirm(RegisterAddress::ACC_CHIP_ID, 0x1E));

        // Enable INT1 as output pin, push-pull, active-low.
        assert(write_with_confirm(RegisterAddress::INT1_IO_CTRL, 0b00001000));
        // Map data ready interrupt to pin INT1.
        assert(write_with_confirm(RegisterAddress::INT_MAP_DATA, 0b00000100));

        // Set ODR (output data rate) = 1600 and OSR (over-sampling-ratio) = 1.
        assert(write_with_confirm(
            RegisterAddress::ACC_CONF,
            0x80 | (0x02 << 4) | (static_cast<uint8_t>(data_rate) << 0)));
        // Set Accelerometer range.
        assert(write_with_confirm(RegisterAddress::ACC_RANGE, static_cast<uint8_t>(range)));

        // Switch the accelerometer into active mode.
        assert(write_with_confirm(RegisterAddress::ACC_PWR_CONF, 0x00));
        // Turn on the accelerometer.
        assert(write_with_confirm(RegisterAddress::ACC_PWR_CTRL, 0x04));

        initialized_ = true;
    }

    static constexpr auto hal_spi_handle = &hspi1;

    void transmit_receive_callback(uint8_t* rx_buffer, size_t size) override {
        if (initialized_) {
            assert(size == sizeof(AccelerometerData) + 2);
            auto& data = *reinterpret_cast<AccelerometerData*>(rx_buffer + 2);

            acc_x = data.x / 32767.0f * 6.0f;
            acc_y = data.y / 32767.0f * 6.0f;
            acc_z = data.z / 32767.0f * 6.0f;

            // static char string_buffer[64];
            // sprintf(string_buffer, "%d %d %d\n", data.x, data.y, data.z);

            // USBD_CDC_SetTxBuffer(
            //     &hUsbDeviceFS, reinterpret_cast<uint8_t*>(string_buffer), strlen(string_buffer));
            // USBD_CDC_TransmitPacket(&hUsbDeviceFS);

            // using namespace std::chrono_literals;
            // module::timer::us_delay(1ms);
        } else {
            init_rx_buffer_ = rx_buffer;
            init_rx_size_   = size;
        }
    }

    void data_ready_callback() {
        assert(read<SpiTransmitReceiveMode::INTERRUPT>(RegisterAddress::ACC_X_LSB, 6));
    }

private:
    enum class RegisterAddress : uint8_t {
        ACC_SOFTRESET  = 0x7E,
        ACC_PWR_CTRL   = 0x7D,
        ACC_PWR_CONF   = 0x7C,
        ACC_SELF_TEST  = 0x6D,
        INT_MAP_DATA   = 0x58,
        INT2_IO_CTRL   = 0x54,
        INT1_IO_CTRL   = 0x53,
        ACC_RANGE      = 0x41,
        ACC_CONF       = 0x40,
        TEMP_LSB       = 0x23,
        TEMP_MSB       = 0x22,
        ACC_INT_STAT_1 = 0x1D,
        SENSORTIME_2   = 0x1A,
        SENSORTIME_1   = 0x19,
        SENSORTIME_0   = 0x18,
        ACC_Z_MSB      = 0x17,
        ACC_Z_LSB      = 0x16,
        ACC_Y_MSB      = 0x15,
        ACC_Y_LSB      = 0x14,
        ACC_X_MSB      = 0x13,
        ACC_X_LSB      = 0x12,
        ACC_STATUS     = 0x03,
        ACC_ERR_REG    = 0x02,
        ACC_CHIP_ID    = 0x00
    };

    struct AccelerometerData {
        int16_t x;
        int16_t y;
        int16_t z;
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
        if (auto task = spi_->create_transmit_receive_task<mode>(this, read_size + 2)) {
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

inline Accelerometer::Lazy accelerometer(&spi1);

} // namespace bmi088
} // namespace spi
} // namespace module
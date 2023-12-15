#include "spi.hpp"

#include <spi.h>

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi) {
    if (hspi == &hspi1) {
        module::Spi<&hspi1>::Singleton::weak_instance(
            [](module::Spi<&hspi1>* self) { self->transmit_receive_callback(); });
    }
}

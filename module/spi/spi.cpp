#include "spi.hpp"
#include "application/application.hpp"

#include <spi.h>

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi) {
    if (hspi == &hspi1) {
        Application::Singleton::weak_instance([](Application* app) {
            app->spi.transmit_receive_callback();
        });
    }
}

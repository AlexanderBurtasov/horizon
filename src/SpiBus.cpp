#include "SpiBus.h"
// std
#include <iostream>
#include <memory.h>
// esp-idf
#include <driver/gpio.h>

SpiBus::SpiBus(int clockPin, int mosiPin, int misoPin, int csPin, int frequencyHz)
{
  //
  {
    ::gpio_reset_pin(static_cast<gpio_num_t>(csPin));
    ::gpio_set_direction(static_cast<gpio_num_t>(csPin), GPIO_MODE_OUTPUT);
    ::gpio_set_level(static_cast<gpio_num_t>(csPin), 0);
  }

  ::memset(&m_busConfig, 0, sizeof(m_busConfig));
  m_busConfig.mosi_io_num = mosiPin;
  m_busConfig.miso_io_num = misoPin;
  m_busConfig.sclk_io_num = clockPin;
  m_busConfig.quadwp_io_num = -1;
  m_busConfig.quadhd_io_num = -1;
  m_busConfig.max_transfer_sz = 0;
  m_busConfig.flags = 0;

  {
    const auto status = ::spi_bus_initialize(HSPI_HOST, &m_busConfig, SPI_DMA_CH_AUTO);
    if (ESP_OK == status)
    {
      std::cout << "SPI init success" << std::endl;
    }
    else
      std::cout << "SPI init fail" << std::endl;
  }

  ::memset(&m_deviceConfig, 0, sizeof(m_deviceConfig));
	m_deviceConfig.clock_speed_hz = frequencyHz;
	m_deviceConfig.queue_size = 7;
	m_deviceConfig.mode = 2;
	m_deviceConfig.flags = SPI_DEVICE_NO_DUMMY;
  m_deviceConfig.spics_io_num = csPin;

  {
    const auto status = ::spi_bus_add_device(HSPI_HOST, &m_deviceConfig, &m_spiHandle);
    if (ESP_OK == status)
      std::cout << "device success" << std::endl;
     else
       std::cout << "device fail" << std::endl;
  }
}

void SpiBus::TransferIntValue(void *ptr, size_t length)
{
    spi_transaction_t transaction =
    {
      .flags = SPI_TRANS_USE_TXDATA,
      .cmd = 0x0,
      .addr = 0x0,
      .length = 8 * length
    };
    ::memcpy(transaction.tx_data, ptr, length);
    ::spi_device_transmit(m_spiHandle, &transaction);
}

void SpiBus::TransferData(const uint8_t *const bytes, size_t length)
{
  spi_transaction_t transaction =
  {
    .flags = 0x0,
    .cmd = 0x0,
    .addr = 0x0,
    .length = 8 * length,
    .rxlength = 0,
    .user = nullptr,
    .tx_buffer = bytes,
    .rx_buffer = nullptr
  };

  const auto status = ::spi_device_transmit(m_spiHandle, &transaction);
  if (ESP_OK != status)
  {
    std::cout << "TransferData unsuccess" << std::endl;
  }
}

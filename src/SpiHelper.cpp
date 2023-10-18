#include "SpiHelper.h"
// std
#include <iostream>
#include <memory>
// esp-idf
#include <driver/gpio.h>

SpiHelper::SpiHelper(BusType busType, int16_t clockPin, int16_t mosiPin, int16_t misoPin, int16_t csPinNum, int frequencyHz)
  : m_csPin{static_cast<gpio_num_t>(csPinNum)}
{
  ::gpio_reset_pin(m_csPin);
  ::gpio_set_direction(m_csPin, GPIO_MODE_OUTPUT);

  ::memset(&m_busConfig, 0, sizeof(m_busConfig));
  m_busConfig.mosi_io_num = mosiPin;
  m_busConfig.miso_io_num = misoPin;
  m_busConfig.sclk_io_num = clockPin;
  m_busConfig.quadwp_io_num = -1;
  m_busConfig.quadhd_io_num = -1;
  m_busConfig.max_transfer_sz = 0;
  m_busConfig.flags = 0;

  spi_host_device_t hostId = (BusType::eHSpi == busType ? HSPI_HOST : VSPI_HOST);
  std::cout << "hostId: " << hostId << std::endl;
  {
    const auto status = ::spi_bus_initialize(hostId, &m_busConfig, SPI_DMA_CH_AUTO);
    if (ESP_OK == status)
    {
      std::cout << "SPI init success" << std::endl;
    }
    else
    {
      std::cout << "SPI init fail" << std::endl;
    }
  }

  ::memset(&m_deviceConfig, 0, sizeof(m_deviceConfig));
	m_deviceConfig.clock_speed_hz = frequencyHz;
	m_deviceConfig.queue_size = 7;
	m_deviceConfig.mode = 2;
	m_deviceConfig.flags = SPI_DEVICE_NO_DUMMY;
  m_deviceConfig.spics_io_num = /*csPin*/-1;

  {
    const auto status = ::spi_bus_add_device(hostId, &m_deviceConfig, &m_spiHandle);
    if (ESP_OK == status)
    {
//      std::cout << "device success" << std::endl;
    }
    else
    {
//      std::cout << "device fail" << std::endl;
    }
  }
}

void SpiHelper::TransferIntValue(void *ptr, size_t length)
{
    spi_transaction_t transaction =
    {
      .flags = SPI_TRANS_USE_TXDATA,
      .cmd = 0x0,
      .addr = 0x0,
      .length = 8 * length,
      .rxlength = 0,
      .user = nullptr
    };
    ::memcpy(transaction.tx_data, ptr, length);
    ::spi_device_transmit(m_spiHandle, &transaction);
}

void SpiHelper::TransferData(const uint8_t *const bytes, size_t length)
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

  /*const auto status =*/ ::spi_device_transmit(m_spiHandle, &transaction);
  // if (ESP_OK != status)
  // {
  //   std::cout << "TransferData unsuccess" << std::endl;
  // }
}

uint8_t SpiHelper::TransferByte(uint8_t value)
{
  spi_transaction_t transaction =
  {
    .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA,
    .cmd = 0x0,
    .addr = 0x0,
    .length = 8,
    .rxlength = 8,
    .user = nullptr
  };
  transaction.tx_data[0] = value;
  /*const auto status = */::spi_device_transmit(m_spiHandle, &transaction);
  // if (ESP_OK != status)
  // {
  //   std::cout << "TransferData unsuccess" << std::endl;
  // }

  return transaction.rx_data[0];
}

void SpiHelper::CsLow()
{
  ::gpio_set_level(m_csPin, 0);
}

void SpiHelper::CsHigh()
{
  ::gpio_set_level(m_csPin, 1);
}

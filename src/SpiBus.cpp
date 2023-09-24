#include "SpiBus.h"
/*
#pragma once

#include "driver/spi_master.h"

class SpiBus
{
public:
  SpiBus(int aClockPin, int aMosiPin, int aMisoPin);
  uint8_t TransferByte(uint8_t byte);

  SpiBus(const SpiBus &) = delete;
  SpiBus &operator=(const SpiBus &) = delete;
  SpiBus(SpiBus &&) = delete;
  SpiBus &operator=(SpiBus &&) = delete;

private:
  spi_device_handle_t m_spiHandle = {};
  spi_bus_config_t m_spiConfig = {};
  spi_device_interface_config_t m_deviceConfig = {};
//-----------------------------------------------------------------------------------------------
}; // class SpiBus
//-----------------------------------------------------------------------------------------------
*/

SpiBus::SpiBus(int aClockPin, int aMosiPin, int aMisoPin, uint32_t aFrequency)
{
    m_spiConfig = { aMosiPin, aMisoPin, aClockPin, -1, -1, 4096, 0x0, 0x0 };

  m_deviceConfig.clock_speed_hz = 1 * 1000 * 1000;
	m_deviceConfig.mode = 0;
	m_deviceConfig.spics_io_num = cePin;
	m_deviceConfig.queue_size = 8;
	m_deviceConfig.duty_cycle_pos = 128;

  auto status = ::spi_bus_initialize(HSPI_HOST, &m_spiConfig, 1);
  if (ESP_OK == status)
    cout << "SPI init success" << endl;
  else
    cout << "SPI init fail" << endl;

  status = ::spi_bus_add_device(HSPI_HOST, &m_deviceConfig, &m_spiHandle);
  if (ESP_OK == status)
    cout << "device success" << endl;
  else
    cout << "device fail" << endl;
}

uint8_t SpiBus::TransferByte(uint8_t byte)
{
}

#pragma once

#include "driver/spi_master.h"

class SpiBus
{
public:
  SpiBus(int clockPin, int mosiPin, int misoPin, int cePin, int frequencyHz);
  void TransferByte(uint8_t byte);

  SpiBus(const SpiBus &) = delete;
  SpiBus &operator=(const SpiBus &) = delete;
  SpiBus(SpiBus &&) = delete;
  SpiBus &operator=(SpiBus &&) = delete;

private:
  spi_device_handle_t m_spiHandle = {};
  spi_bus_config_t m_busConfig = {};
  spi_device_interface_config_t m_deviceConfig = {};
//-----------------------------------------------------------------------------------------------
}; // class SpiBus
//-----------------------------------------------------------------------------------------------

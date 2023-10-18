#pragma once
#include <memory.h>
// esp-idf
#include "driver/gpio.h"
#include "driver/spi_master.h"

class SpiHelper
{
public:
  enum BusType
  {
    eHSpi = 1,
    eVSpi
  };

public:
  SpiHelper(BusType busType, int16_t clockPin, int16_t mosiPin, int16_t misoPin, int16_t csPin, int frequencyHz);

  void TransferIntValue(void *ptr, size_t length);
  void TransferData(const uint8_t *const bytes, size_t length);

  uint8_t TransferByte(uint8_t value);

  void CsLow();
  void CsHigh();

protected:
  SpiHelper(const SpiHelper &) = delete;
  SpiHelper &operator=(const SpiHelper &) = delete;
  SpiHelper(SpiHelper &&) = delete;
  SpiHelper &operator=(SpiHelper &&) = delete;

private:
  gpio_num_t m_csPin;
  spi_device_handle_t m_spiHandle = {};
  spi_bus_config_t m_busConfig = {};
  spi_device_interface_config_t m_deviceConfig = {};
//-----------------------------------------------------------------------------------------------
}; // class SpiHelper
//-----------------------------------------------------------------------------------------------

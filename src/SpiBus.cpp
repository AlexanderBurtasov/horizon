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

SpiBus::SpiBus(int clockPin, int mosiPin, int misoPin, int csPin, int frequencyHz)
{
//  m_spiConfig = { aMosiPin, aMisoPin, aClockPin, -1, -1, 4096, 0x0, 0x0 };
  m_busConfig = 
  {
    .mosi_io_num = mosiPin,
    .miso_io_num = misoPin,
    .sclk_io_num = clockPin,
    .quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = 0,
		.flags = 0
  };

  m_deviceConfig =
  {
    .command_bits = 0,
    .address_bits = 0,
    .dummy_bits = 0,
	  .mode = 2,
    .clock_source = static_cast<spi_clock_source_t>(0),
    .duty_cycle_pos = 0,
    .cs_ena_pretrans = 0,
	  .clock_speed_hz = frequencyHz,
	  .input_delay_ns = 0,
    .spics_io_num = csPin,
	  .flags = SPI_DEVICE_NO_DUMMY,
    .queue_size = 0,
    .pre_cb = 0,
    .post_cb = 0
  };

  auto status = ::spi_bus_initialize(HSPI_HOST, &m_busConfig, 1);
  // if (ESP_OK == status)
  //   cout << "SPI init success" << endl;
  // else
  //   cout << "SPI init fail" << endl;

  status = ::spi_bus_add_device(HSPI_HOST, &m_deviceConfig, &m_spiHandle);
  // if (ESP_OK == status)
  //   cout << "device success" << endl;
  // else
  //   cout << "device fail" << endl;
}

void SpiBus::TransferByte(uint8_t byte)
{
}

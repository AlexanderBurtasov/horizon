#pragma once
// std
#include <cstdint>
#include <vector>
// esp-idf
#include "driver/gpio.h"
// fwd
class SpiHelper;

class Nrf24l01
{
public:
  enum class DataRate: uint8_t
  {
    INVALID = 0
    , RATE_250KBPS = 1
    , RATE_1MBPS
    , RATE_2MBPS
  };

  enum class Power: uint8_t
  {
    INVALID = 0
    , MINUS_18DB
    , MINUS_12DB
    , MINUS_6DB
    , ZERO_DB
  };

public:
  Nrf24l01(SpiHelper &rSpiHelper, int16_t cePin);
  void Prepare();

  uint8_t ReadRegister(uint8_t addr);
  void WriteRegister(uint8_t addr, uint8_t value);

  void WriteRegisterValues(uint8_t addr, const std::vector<uint8_t> &values);
  void ReadRegisterValues(uint8_t addr, std::vector<uint8_t> &values);

  void SetChannel(uint8_t channelNum);
  void SetDataRate(Nrf24l01::DataRate rate);
  void SetPower(Nrf24l01::Power power);

  void ReadFifoStatus();
  void OpenReadingPipe(uint8_t pipeNum, const std::vector<uint8_t> &address, uint8_t payloadSize);
  void OpenWritingPipe(const std::vector<uint8_t> &address, uint8_t payloadSize);

  void StartListening();
  void StopListening();

  bool IsRxDataAvailable(uint8_t pipeNumber);
  bool IsRxDataAvailable();

  void ReadRxPayload(std::vector<uint8_t> &values);
  void Transmitter();
  void Send(const std::vector<uint8_t> &values);

  void DumpRegisters();

private:
  void FlushRx();
  void FlushTx();

private:
  void CeLow();
  void CeHigh();

private:
  SpiHelper &m_rSpiHelper;
  gpio_num_t m_cePin;
  uint8_t m_channelNum = 0;
};

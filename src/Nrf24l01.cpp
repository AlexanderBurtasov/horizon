#include "Nrf24l01.h"
// std
#include <bitset>
#include <thread>
#include <iostream>
// prj
#include "SpiHelper.h"

namespace
{
const uint8_t NRF_CMD_R_REGISTER = 0x0;
const uint8_t NRF_CMD_W_REGISTER = 0x20;
const uint8_t NRF_CMD_R_RX_PAYLOAD = 0x61;
const uint8_t NRF_CMD_W_TX_PAYLOAD = 0xA0;
const uint8_t NRF_CMD_FLUSH_RX = 0xE2;
const uint8_t NRF_CMD_FLUSH_TX = 0xE1;
const uint8_t NRF_RX_ADDR_P0 = 0x0A;

const uint8_t NRF_REG_CONFIG = 0x00;
const uint8_t NRF_REG_EN_RXADDR  = 0x02;
const uint8_t NRF_REG_SETUP_AW = 0x03;
const uint8_t NRF_REG_SETUP_RETR = 0x04;
const uint8_t NRF_REG_RF_CH = 0x05;
const uint8_t NRF_REG_RF_SETUP = 0x06;
const uint8_t NRF_REG_STATUS = 0x07;
const uint8_t NRF_REG_RX_ADDR_P0  = 0x0a;
const uint8_t NRF_REG_TX_ADDR = 0x10;
const uint8_t NRF_REG_RX_PW_P0 = 0x11;
const uint8_t NRF_REG_FIFO_STATUS = 0x17;
const uint8_t NRF_REG_DYNPD = 0x1c;

#pragma pack(push, 1)
struct ConfigRegister
{
  uint8_t PRIM_RX : 1;
  uint8_t PWR_UP : 1;
  uint8_t CRC0 : 1;
  uint8_t EN_CRC : 1;
  uint8_t MASK_MAX_RT : 1;
  uint8_t MASK_TX_DS : 1;
  uint8_t MASK_RX_DR : 1;
  uint8_t Reserved : 1;
};
//-----------------------------------------------------------------------------------------------
struct EnAaRegister
{
  uint8_t ENAA_P0  : 1;
  uint8_t ENAA_P1  : 1;
  uint8_t ENAA_P2  : 1;
  uint8_t ENAA_P3  : 1;
  uint8_t ENAA_P4  : 1;
  uint8_t ENAA_P5  : 1;
  uint8_t Reserved : 2;
};
//-----------------------------------------------------------------------------------------------
struct EnRxAddrRegister
{
  uint8_t ERX_P0  : 1;
  uint8_t ERX_P1  : 1;
  uint8_t ERX_P2  : 1;
  uint8_t ERX_P3  : 1;
  uint8_t ERX_P4  : 1;
  uint8_t ERX_P5  : 1;
  uint8_t Reserved : 2;
};
//-----------------------------------------------------------------------------------------------
struct SetupAwRegister
{
  uint8_t AW : 2;
  uint8_t Reserved : 6;
};
//-----------------------------------------------------------------------------------------------
struct RfSetupRegister
{
  uint8_t Obsolete : 1;
  uint8_t RF_PWR : 2;
  uint8_t RF_DR_HIGH : 1;
  uint8_t PLL_LOCK : 1;
  uint8_t RF_DR_LOW : 1;
  uint8_t Reserved : 1;
  uint8_t CONT_WAVE : 1;
};
//-----------------------------------------------------------------------------------------------
struct RfStatusRegister
{
  uint8_t TX_FULL : 1;
  uint8_t RX_P_NO : 3;
  uint8_t MAX_RT : 1;
  uint8_t TX_DS : 1;
  uint8_t RX_DR : 1;
  uint8_t Reserved : 1;
};
//-----------------------------------------------------------------------------------------------
struct FifoStatusRegister
{
  uint8_t RX_EMPTY : 1;
  uint8_t RX_FULL : 1;
  uint8_t Reserved_0 : 2;
  uint8_t TX_EMPTY : 1;
  uint8_t TX_FULL : 1;
  uint8_t TX_REUSE : 1;
  uint8_t Reserved_1 : 1;
};
#pragma pack(pop)
//-----------------------------------------------------------------------------------------------
} // namespace
//-----------------------------------------------------------------------------------------------
using std::cout;
using std::endl;
using std::vector;
namespace ch = std::chrono;
namespace th = std::this_thread;
//-----------------------------------------------------------------------------------------------
//                                   class Nrf24l01
//-----------------------------------------------------------------------------------------------
Nrf24l01::Nrf24l01(SpiHelper &rSpiHelper, int16_t cePin)
    : m_rSpiHelper(rSpiHelper)
    , m_cePin{static_cast<gpio_num_t>(cePin)}
{
  ::gpio_reset_pin(m_cePin);
  ::gpio_set_direction(m_cePin, GPIO_MODE_OUTPUT);
  CeLow();

  m_rSpiHelper.CsHigh();
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::Prepare()
{
  CeLow();
  // disable dynamic payload
  WriteRegister(NRF_REG_DYNPD, 0x0);

  // auto retransmit delay: 1750us
  // auto retransmit count: 8
  WriteRegister(NRF_REG_SETUP_RETR, (6 << 4) | 8);

  // enable crc
  {
    auto value = ReadRegister(NRF_REG_CONFIG);
    auto *config = reinterpret_cast<ConfigRegister *>(&value);
    config->CRC0 = 1;
    config->EN_CRC = 1;
    config->PWR_UP = 0;
    WriteRegister(NRF_REG_CONFIG, value);
  }

  FlushRx();
  FlushTx();
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::SetChannel(uint8_t channelNum)
{
  // channel num [0; 127]
  WriteRegister(NRF_REG_RF_CH, channelNum);
  m_channelNum = channelNum;
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::SetDataRate(Nrf24l01::DataRate rate)
{
  uint8_t value = ReadRegister(NRF_REG_RF_SETUP);
  RfSetupRegister *rfReg = reinterpret_cast<RfSetupRegister *>(&value);

  switch (rate)
  {
    case (DataRate::RATE_250KBPS):
      rfReg->RF_DR_LOW = 1;
      rfReg->RF_DR_HIGH = 0;
    break;
    case (DataRate::RATE_1MBPS):
      rfReg->RF_DR_LOW = 0;
      rfReg->RF_DR_HIGH = 0;
    break;
    case (DataRate::RATE_2MBPS):
      rfReg->RF_DR_LOW = 0;
      rfReg->RF_DR_HIGH = 1;
    break;
    default:
    {}
  }
  WriteRegister(NRF_REG_RF_SETUP, value);
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::SetPower(Nrf24l01::Power power)
{
  uint8_t value = ReadRegister(NRF_REG_RF_SETUP);
  RfSetupRegister *rfReg = reinterpret_cast<RfSetupRegister *>(&value);

  switch (power)
  {
    case (Power::MINUS_18DB):
      rfReg->RF_PWR = 0b00;
    break;
    case (Power::MINUS_12DB):
      rfReg->RF_PWR = 0b01;
    break;
    case (Power::MINUS_6DB):
      rfReg->RF_PWR = 0b10;
    break;
    case (Power::ZERO_DB):
      rfReg->RF_PWR = 0b11;
    break;
    default:
    {}
  }
  WriteRegister(NRF_REG_RF_SETUP, value);
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::ReadFifoStatus()
{
  uint8_t value = ReadRegister(NRF_REG_FIFO_STATUS);
  //FifoStatusRegister *fifoStatus = reinterpret_cast<FifoStatusRegister *>(&value);
  //cout << "Fifo status: " << bitset<8>(value) << endl;
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::OpenReadingPipe(uint8_t pipeNum, const vector<uint8_t> &address, uint8_t payloadSize)
{
  WriteRegisterValues(0xA + pipeNum, address);

  auto value = ReadRegister(0x2);
  value = value | (0x1 << pipeNum);
  WriteRegister(0x2, value);

  // todo!!! payload 32 bytes
  uint8_t addr = NRF_REG_RX_PW_P0 + pipeNum;
  WriteRegister(addr, payloadSize);
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::OpenWritingPipe(const std::vector<uint8_t> &address, uint8_t payloadSize)
{
  WriteRegisterValues(0x0A, address);
  WriteRegisterValues(NRF_REG_TX_ADDR, address);
  WriteRegister(NRF_REG_RX_PW_P0, payloadSize);
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::StopListening()
{
  CeLow();

  FlushRx();
  FlushTx();
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::StartListening()
{
  {
    uint8_t value = ReadRegister(NRF_REG_CONFIG);
    ConfigRegister *config = reinterpret_cast<ConfigRegister *>(&value);

    config->PWR_UP = 1;
    config->PRIM_RX = 1;

    WriteRegister(NRF_REG_CONFIG, value);

    th::sleep_for(ch::milliseconds(150));
  }
  {
    auto value = ReadRegister(NRF_REG_STATUS);
    auto *status = reinterpret_cast<RfStatusRegister *>(&value);
    status->RX_DR = 1;
    status->TX_DS = 1;
    status->MAX_RT = 1;
    WriteRegister(NRF_REG_STATUS, value);
  }

  FlushRx();
  FlushTx();

  CeHigh();
  ::esp_rom_delay_us(130);
}
//-----------------------------------------------------------------------------------------------
bool Nrf24l01::IsRxDataAvailable(uint8_t pipeNumber)
{
  const auto tmp = ReadRegister(NRF_REG_STATUS);
  const RfStatusRegister *status = reinterpret_cast<const RfStatusRegister *>(&tmp);

  return (status->RX_DR == 1 && status->RX_P_NO == pipeNumber);
}
//-----------------------------------------------------------------------------------------------
bool Nrf24l01::IsRxDataAvailable()
{
  const auto tmp = ReadRegister(NRF_REG_STATUS);
  const RfStatusRegister *status = reinterpret_cast<const RfStatusRegister *>(&tmp);

  return status->RX_DR == 1;
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::FlushRx()
{
  WriteRegisterValues(NRF_CMD_FLUSH_RX, {});
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::FlushTx()
{
  WriteRegisterValues(NRF_CMD_FLUSH_TX, {});
}
//-----------------------------------------------------------------------------------------------
uint8_t Nrf24l01::ReadRegister(uint8_t addr)
{
  uint8_t v1 = NRF_CMD_R_REGISTER | addr;

  m_rSpiHelper.CsLow();

  ::esp_rom_delay_us(3);

  m_rSpiHelper.TransferByte(v1);

  ::esp_rom_delay_us(1);

  const auto result = m_rSpiHelper.TransferByte(0xFF);
	
  m_rSpiHelper.CsHigh();

  return result;
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::WriteRegister(uint8_t addr, uint8_t value)
{
  WriteRegisterValues(addr, { value });
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::ReadRxPayload(vector<uint8_t> &values)
{
  {
    uint8_t v1 = NRF_CMD_R_RX_PAYLOAD;

    m_rSpiHelper.CsLow();

    ::esp_rom_delay_us(3);

    m_rSpiHelper.TransferByte(v1);

    for (auto &v : values)
    {
      ::esp_rom_delay_us(1);
      v = m_rSpiHelper.TransferByte(0xFF);
    }

    m_rSpiHelper.CsHigh();
  }

  uint8_t value = ReadRegister(NRF_REG_STATUS);

  RfStatusRegister *status = reinterpret_cast<RfStatusRegister *>(&value);
  status->RX_DR = 1;
  status->TX_DS = 0;
  status->MAX_RT = 0;

  WriteRegister(NRF_REG_STATUS, value); 
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::Transmitter()
{
  uint8_t value = ReadRegister(NRF_REG_CONFIG);
  ConfigRegister *config = reinterpret_cast<ConfigRegister *>(&value);
  config->PWR_UP = 1;
  config->PRIM_RX = 0; // go into TX mode

  WriteRegister(NRF_REG_CONFIG, value);
//  ::esp_rom_delay_us(150);
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::Send(const vector<uint8_t> &values)
{
//  CeLow();
  FlushTx();

  {
    uint8_t value = ReadRegister(NRF_REG_CONFIG);
    ConfigRegister *config = reinterpret_cast<ConfigRegister *>(&value);
    config->PWR_UP = 1;
    config->PRIM_RX = 0; // go into TX mode

    WriteRegister(NRF_REG_CONFIG, value);
    ::esp_rom_delay_us(2000);
  }

  m_rSpiHelper.CsLow();
  m_rSpiHelper.TransferByte(NRF_CMD_W_TX_PAYLOAD);
  
  for (const auto v : values)
  {
    ::esp_rom_delay_us(1);
    m_rSpiHelper.TransferByte(v);
  }
  m_rSpiHelper.CsHigh();

  ::esp_rom_delay_us(3);
  CeHigh();

  ::esp_rom_delay_us(30000);

  CeLow();
  uint8_t count = 0;
  while (true)
  {
    const uint8_t value = ReadRegister(NRF_REG_STATUS);
    const RfStatusRegister *status = reinterpret_cast<const RfStatusRegister *>(&value); 
    if (status->MAX_RT != 0 || status->TX_DS != 0 || count > 10)
      break;
    ::esp_rom_delay_us(25);
    ++count;
  }

  {
    uint8_t value = ReadRegister(NRF_REG_STATUS);
    RfStatusRegister *status = reinterpret_cast<RfStatusRegister *>(&value);
    status->RX_DR = 0;
    status->MAX_RT = 1;
    status->TX_DS = 1;
    WriteRegister(NRF_REG_STATUS, value);
  }

  { // sleep tx mode
    uint8_t value = ReadRegister(NRF_REG_CONFIG);
    ConfigRegister *config = reinterpret_cast<ConfigRegister *>(&value);
    config->PWR_UP = 0;
    config->PRIM_RX = 0;

    WriteRegister(NRF_REG_CONFIG, value);
  }
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::DumpRegisters()
{
  cout << std::hex;

  for (uint8_t addr = 0x0; addr < 0xA; ++addr)
  {
    uint8_t value = ReadRegister(addr);
    cout << "[0x" << static_cast<size_t>(addr) << "]: 0x" << /*bitset<8>*/static_cast<size_t>(value) << endl;
  }

  for (uint8_t addr = 0xA; addr < 0xC; ++addr)
  {
    vector<uint8_t> values;
    values.resize(5);

    ReadRegisterValues(addr, values);
    cout << "[0x" << static_cast<size_t>(addr) << "]: 0x"; 
    for (auto v : values)
      cout << /*bitset<8>*/static_cast<size_t>(v) << ' ';
    cout << endl;
  }

  for (uint8_t addr = 0xC; addr <= 0x17; ++addr)
  {
    uint8_t value = ReadRegister(addr);
    cout << "[0x" << static_cast<size_t>(addr) << "]: 0x" << /*bitset<8>*/static_cast<size_t>(value) << endl;
  }
}
//---------------------------------------------------------------------------
void Nrf24l01::ReadRegisterValues(uint8_t addr, vector<uint8_t> &values)
{
  uint8_t v1 = NRF_CMD_R_REGISTER | addr;

  m_rSpiHelper.CsLow();

  ::esp_rom_delay_us(3);

  m_rSpiHelper.TransferByte(v1);

  for (auto &v : values)
  {
    ::esp_rom_delay_us(1);
    v = m_rSpiHelper.TransferByte(0xFF);
  }

  m_rSpiHelper.CsHigh();
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::WriteRegisterValues(uint8_t addr, const vector<uint8_t> &values)
{
  uint8_t v1 = NRF_CMD_W_REGISTER | addr;

  m_rSpiHelper.CsLow();

  ::esp_rom_delay_us(3);

  m_rSpiHelper.TransferByte(v1);

  for (const auto v : values)
  {
    ::esp_rom_delay_us(1);
    m_rSpiHelper.TransferByte(v);
  }

  m_rSpiHelper.CsHigh();
}
//-----------------------------------------------------------------------------------------------
void Nrf24l01::CeLow()
{
  ::gpio_set_level(m_cePin, 0);
}
//----------------------------------------------------------------------------------------------
void Nrf24l01::CeHigh()
{
  ::gpio_set_level(m_cePin, 1);
}
//-----------------------------------------------------------------------------------------------

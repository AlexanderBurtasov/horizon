// std
#include <iostream>
#include <thread>
// prj
#include "SpiHelper.h"
#include "Nrf24l01.h"

const uint16_t cClkPin = 14;
const uint16_t cMosiPin = 32;
const uint16_t cMisoPin = 27;
const uint16_t cCsPin = 33;
const uint16_t cCePin = 12;

const uint8_t cPayloadSize = 8;

const uint32_t cFrequencyHz = 400'000;

const uint8_t cChanNumber = 0;
const uint8_t cPipeNumber = 0;

using namespace std::this_thread;
using namespace std::chrono;
using std::cout;
using std::endl;
using std::vector;

void DoReceiverTest()
{
  SpiHelper spiHelper{SpiHelper::eVSpi, cClkPin, cMosiPin, cMisoPin, cCsPin, cFrequencyHz};
  Nrf24l01 nrf24{spiHelper, cCePin};

  nrf24.Prepare();
  nrf24.SetChannel(cChanNumber);
  nrf24.SetDataRate(Nrf24l01::DataRate::RATE_1MBPS);
  nrf24.SetPower(Nrf24l01::Power::MINUS_6DB);

  const vector<uint8_t> gPipeAddress{1, 0, 0, 0, 0};
  nrf24.OpenReadingPipe(cPipeNumber, gPipeAddress, cPayloadSize);
  nrf24.StartListening();

  nrf24.DumpRegisters();

  cout << "listening started ..." << endl;

//  size_t count = 0;
  for (;;)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10'000));

    if (nrf24.IsRxDataAvailable(cPipeNumber))
    {
      nrf24.ReadFifoStatus();
      vector<uint8_t> values(cPayloadSize);
      nrf24.ReadRxPayload(values);

      cout << "v0: " << static_cast<uint16_t>(values[0]) << "; v1: " << static_cast<uint16_t>(values[1]) << "; v4: " << static_cast<uint16_t>(values[4])
      << endl;
    }
  }
}

void DoTransmitterTest()
{
  SpiHelper spiHelper{SpiHelper::eHSpi, cClkPin, cMosiPin, cMisoPin, cCsPin, cFrequencyHz};
  Nrf24l01 nrf24{spiHelper, cCePin};

  nrf24.Prepare();
  nrf24.SetChannel(cChanNumber);
  nrf24.SetDataRate(Nrf24l01::DataRate::RATE_1MBPS);
  nrf24.SetPower(Nrf24l01::Power::MINUS_6DB);

  const vector<uint8_t> gPipeAddress{ 1, 0, 0, 0, 0 };
  nrf24.OpenWritingPipe(gPipeAddress, cPayloadSize);
//  nrf24.StartListening();

  nrf24.DumpRegisters();

  cout << "transmitting started ..." << endl;
  vector<uint8_t> items = {8, 7, 6, 5, 4, 3, 2, 1};

  size_t count = 0;
  for (;;)
  {
//    nrf24.StopListening();

    nrf24.Send(items);
    nrf24.DumpRegisters();

//    nrf24.StartListening();
    std::this_thread::sleep_for(std::chrono::milliseconds(15'000));
  }
}

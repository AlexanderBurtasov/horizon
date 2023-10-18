#include "SpiHelper.h"
#include "TftSpi.h"
#include "VgaFont.h"

#include "utils/TimeMeter.h"

namespace
{
const uint16_t cWidth = 240;
const uint16_t cHeight = 280;

const int8_t cMosiPin = 13;
const int8_t cMisoPin = -1;
const int8_t cClkPin = 23;
const int8_t cCsPin = 16;
const int8_t cDcPin = 19;
const int8_t cResetPin = 4;
const int8_t cBlPin = 22;
}

const int cFrequencyHz = 40000000;

void DoTftTest()
{
  SpiHelper spiHelper(SpiHelper::eHSpi, cClkPin, cMosiPin, -1, cCsPin, cFrequencyHz);
  TftSpi tft{spiHelper, cDcPin, cResetPin, cBlPin, cWidth, cHeight};

  tft.DrawFillRect(0, 0, cWidth, cHeight, 0x0);
  const auto colorRed = TftSpi::ColorRgb(255, 0, 0);
  const auto colorGreen = TftSpi::ColorRgb(0, 255, 0);
  const auto colorBlue = TftSpi::ColorRgb(0, 0, 255);
  const auto colorYellow = TftSpi::ColorRgb(200, 200, 0);

  VgaFont8x8 vga8x8;
  VgaFont8x16 vga8x16;
  VgaFont16x32 vga16x32;

  for (int i = 0; i < cHeight; i += 1)
  {
    tft.DrawFastBresLine(cWidth/2, cHeight/2, cWidth, i, colorGreen);
  }

  for (int i = 0; i < cWidth; i += 1)
  {
    tft.DrawFastBresLine(cWidth / 2, cHeight / 2, i, cHeight, colorYellow);
  }

  for (int i = 0; i < cHeight; ++i)
  {
    tft.DrawFastBresLine(cWidth / 2, cHeight / 2, 0, i, colorBlue);
  }

  for (int i = 0; i < cWidth; i += 1)
  {
    tft.DrawFastBresLine(cWidth / 2, cHeight / 2, i, 0, colorRed);
  }

  tft.DrawString(20, 160, "Hells Bells!", vga8x16, colorRed);

  std::cout << "DoTftTest(): done" << std::endl;
}

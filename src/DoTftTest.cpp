#include "SpiHelper.h"
#include "TftSpi.h"
#include "VgaFont.h"

#include "utils/TimeMeter.h"

namespace
{
const int8_t cMosiPin = 13;
const int8_t cMisoPin = -1;
const int8_t cClkPin = 23;
const int8_t cCsPin = 16;
const int8_t cDcPin = 19;
const int8_t cResetPin = 4;
const int8_t cBlPin = 22;
}

const int cFrequencyHz = 40'000'000;
//const int cFrequencyHz = 26'000'000;

void DoSinTest()
{
  SpiHelper spiHelper(SpiHelper::eHSpi, cClkPin, cMosiPin, -1, cCsPin, cFrequencyHz);
  TftSpi tft{spiHelper, cDcPin, cResetPin, cBlPin, TftSpi::ScreenSize::e240x320, TftSpi::Orientation::ePortrait};

  const auto colorYellow = TftSpi::ColorRgb(255, 255, 0);
  tft.DrawFillRect(5, 20, tft.GetWidth()-5, tft.GetHeight() / 2, colorYellow);
}

void DoTftTest()
{
  SpiHelper spiHelper(SpiHelper::eHSpi, cClkPin, cMosiPin, -1, cCsPin, cFrequencyHz);
  TftSpi tft{spiHelper, cDcPin, cResetPin, cBlPin, TftSpi::ScreenSize::e240x280, TftSpi::Orientation::eLandscape};
  const uint16_t cWidth = tft.GetWidth();
  const uint16_t cHeight = tft.GetHeight();

  tft.DrawFillRect(0, 0, cWidth, cHeight, 0x0);
  const auto colorRed = TftSpi::ColorRgb(255, 0, 0);
  const auto colorGreen = TftSpi::ColorRgb(0, 255, 0);
  const auto colorBlue = TftSpi::ColorRgb(0, 0, 255);
  const auto colorYellow = TftSpi::ColorRgb(255, 255, 0);

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

  for (int i = 0; i < 5; ++i)
  {
    tft.DrawCircle(i * 10, i * 10, i * 10, colorRed);
  }

  std::cout << "DoTftTest(): done" << std::endl;
}

void DoLineTest()
{
  SpiHelper spiHelper(SpiHelper::eHSpi, cClkPin, cMosiPin, -1, cCsPin, cFrequencyHz);
  TftSpi tft{spiHelper, cDcPin, cResetPin, cBlPin, TftSpi::ScreenSize::e240x280, TftSpi::Orientation::ePortrait};

  const uint16_t cWidth = tft.GetWidth();
  const uint16_t cHeight = tft.GetHeight();

  const auto colorRed = TftSpi::ColorRgb(255, 0, 0);

  tft.DrawFillRect(0, 0, cWidth, cHeight, 0x0);
  uint16_t x0 = 1;
  for (uint16_t i = 0; i < 10; ++i)
  {
    tft.DrawFastBresLine(x0, 10, x0, cHeight - 11, colorRed);

    x0 += 10;
  }
}

#include "SpiBus.h"
#include "TftSpi.h"
#include "st7789.h"
#include "HorizonWidget.h"

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

#define CONFIG_OFFSETX 0
#define CONFIG_OFFSETY 20

const int cFrequencyHz = 40'000'000;

void DoTftTest()
{
  SpiBus spiBus(cClkPin, cMosiPin, -1, cCsPin, cFrequencyHz);
  TftSpi tft{spiBus, cDcPin, cResetPin, cBlPin, cWidth, cHeight};

  tft.FillRect(0, 0, cWidth, cHeight, 0x0);
  const auto colorRed = TftSpi::ColorRgb(255, 0, 0);
  const auto colorGreen = TftSpi::ColorRgb(0, 255, 0);

//  tft.DrawLinePixel(10, 10, 220, 220, colorRed);
//  tft.DrawLinePixel(0, 0, 239, 240, colorRed);

  {
    TimeMeter tm{"pixel line: "};
    for (uint16_t i = 0; i < cHeight; ++i)
    {
      tft.DrawLinePixel(0, 0, cWidth-1, i, colorRed);
      tft.DrawLineRect(0, 0, cWidth-1, i, colorGreen);
    }
  }



  std::cout << "DoTftTest(): done" << std::endl;
}

void DoTftTestOld()
{
  TFT_t dev;
  ::spi_master_init(&dev, cMosiPin, cClkPin, cCsPin, cDcPin, cResetPin, cBlPin);
  ::lcdInit(&dev, cWidth, cHeight, CONFIG_OFFSETX, CONFIG_OFFSETY);

  ::DrawFillRect(&dev, 0, 0, 240, 280, 0xffff);
/*
  HorizonWidget widget{&dev, 240, 280};
  {
    TimeMeter meter("Horizon: ");
    widget.paintEvent();
  }
*/
//  ::lcdDrawFillRect(&dev, 0, 0, 239, 279, RED);

  std::cout << "DoTftTestOld(): done" << std::endl;
}

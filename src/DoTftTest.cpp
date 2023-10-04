#include "SpiBus.h"
#include "TftSpi.h"
#include "st7789.h"
#include "HorizonWidget.h"

#include "utils/TimeMeter.h"

#define CONFIG_WIDTH  240
#define CONFIG_HEIGHT 280
#define CONFIG_MOSI_GPIO 13
//#define CONFIG_SCLK_GPIO 14
#define CONFIG_SCLK_GPIO 23
#define CONFIG_CS_GPIO 16
#define CONFIG_DC_GPIO 19
#define CONFIG_RESET_GPIO 4
#define CONFIG_BL_GPIO 22

#define CONFIG_OFFSETX 0
#define CONFIG_OFFSETY 20


const int cFrequencyHz = 40'000'000;

void DoTftTest()
{
  SpiBus spiBus(CONFIG_SCLK_GPIO, CONFIG_MOSI_GPIO, -1, CONFIG_CS_GPIO, cFrequencyHz);
  TftSpi tft{spiBus, CONFIG_DC_GPIO, CONFIG_RESET_GPIO, CONFIG_BL_GPIO, 240, 280};

  {
    TimeMeter tm{"Single Paint: "};
    tft.FillRect(0, 0, 240, 280, 0x0000);
    const uint16_t colorRed = TftSpi::ColorRgb(255, 0, 0);
    const uint16_t colorGreen = TftSpi::ColorRgb(0, 255, 0);
    const uint16_t colorBlue = TftSpi::ColorRgb(0, 0, 255);
    const uint16_t colorYellow = TftSpi::ColorRgb(127, 127, 0);

    tft.FillRect(20, 20, 100, 100, colorRed);
//    tft.FillRect(121, 20, 100, 100, colorGreen);
    tft.FillRect(20, 121, 100, 100, colorBlue);
//    tft.FillRect(121, 121, 100, 100, colorYellow);
    //tft.FillRect(0, 0, 200, 200, 0xadad);
    //tft.FillRect(0, 0, 200, 200, 0xabcd);
    //tft.PutPixel(200, 200, 0xffff);
  }
  std::cout << "DoTftTest(): done" << std::endl;
}


void DoTftTestOld()
{
  TFT_t dev;
  ::spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO, CONFIG_BL_GPIO);
  ::lcdInit(&dev, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_OFFSETX, CONFIG_OFFSETY);

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

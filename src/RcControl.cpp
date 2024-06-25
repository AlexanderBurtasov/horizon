// std
#include <iomanip>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>
// prj
#include "helpers/AdcJoystick.h"
#include "types/GuideVector.h"
#include "ui/TextItem.h"
#include "utils/TimeMeter.h"
#include "AdcAxis.h"
#include "I2cJoystick.h"
#include "I2cMasterHelper.h"
#include "SpiHelper.h"
#include "TftSpi.h"
#include "VgaFont.h"

using std::lock_guard;
using std::mutex;
using std::string;
using std::stringstream;
using std::thread;
using std::vector;

namespace ch = std::chrono;
namespace th = std::this_thread;

const uint16_t cConfigWidth = 240;
const uint16_t cConfigHeight = 280;

#define CONFIG_MOSI_GPIO 13
//#define CONFIG_SCLK_GPIO 14
#define CONFIG_SCLK_GPIO 23
#define CONFIG_CS_GPIO 16
#define CONFIG_DC_GPIO 19
#define CONFIG_RESET_GPIO 4
#define CONFIG_BL_GPIO 22

#define CONFIG_OFFSETX 0
#define CONFIG_OFFSETY 20

const auto cGuideUpdateInterval = std::chrono::milliseconds(100);
const auto cDisplayUpdateInterval = std::chrono::milliseconds(100);

namespace
{
const int8_t cMosiPin = 13;
const int8_t cMisoPin = -1;
const int8_t cClkPin = 23;
const int8_t cCsPin = 16;
const int8_t cDcPin = 19;
const int8_t cResetPin = 4;
const int8_t cBlPin = 22;

const int cFrequencyHz = 40'000'000;

const uint16_t cColorBlack = 0x0000;
const auto cColorYellow = TftSpi::ColorRgb(255, 255, 0);
const auto cColorRed = TftSpi::ColorRgb(255, 0, 0);

const int cAxisXSpan = 10;
const int cAxisYSpan = 10;
}


// void DoRcControlTest2()
// {
//   AdcAxis axisX0{GPIO_NUM_35, 220, 2150, 4095, 10};
//   AdcAxis axisY0{GPIO_NUM_34, 3830, 1680, 60, 10};
  
//   AdcAxis axisX1{GPIO_NUM_39, 120, 2070, 4040, 10};
//   AdcAxis axisY1{GPIO_NUM_36, 4020, 1820, 160, 10};

//   TFT_t dev;
//   spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO, CONFIG_BL_GPIO);
//   lcdInit(&dev, cConfigWidth, cConfigHeight, CONFIG_OFFSETX, CONFIG_OFFSETY);
//   lcdDrawFillRect(&dev, 0, 0, 239, 279, CYAN);

//   GuideVector guideVector;
//   mutex lock;

//   auto readGuide = [&guideVector, &lock, &axisX0, &axisY0, &axisX1, &axisY1]()
//   {
//     for (;;)
//     {
//         const int x0 = axisX0.GetValue();
//         const int y0 = axisY0.GetValue();

//         const int x1 = axisX1.GetValue();
//         const int y1 = axisY1.GetValue();
//         {
//             lock_guard<mutex> guard{lock};
//             guideVector.x0 = x0;
//             guideVector.y0 = y0;
//             guideVector.x1 = x1;
//             guideVector.y1 = y1;
//         }

//         std::this_thread::sleep_for(cGuideUpdateInterval);
//     }
//   };

//   std::thread updateThread{readGuide};
//   std::thread dispayThread{DoPlainView, &dev, std::reference_wrapper<GuideVector>(guideVector), std::reference_wrapper<mutex>(lock)};

//   updateThread.join();
//   dispayThread.join();
// }

// void DoRcControlTest()
// {
//   I2cMasterHelper i2cMaster{I2cMasterHelper::I2cPort::cNumber_0, 21, 22, 100000};
//   I2cJoystick i2cJoystick{0x9, i2cMaster, 10};

//   TFT_t dev;
//   spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO, CONFIG_BL_GPIO);
//   lcdInit(&dev, cConfigWidth, cConfigHeight, CONFIG_OFFSETX, CONFIG_OFFSETY);
//   lcdDrawFillRect(&dev, 0, 0, 239, 279, CYAN);

//   GuideVector guideVector;
//   mutex lock;

//   auto readGuide = [&guideVector, &lock, &i2cJoystick]()
//   {
//     for (;;)
//     {
//         const auto change = i2cJoystick.Update();
//         if (0x0 != change)
//         {
//             lock_guard<mutex> guard{lock};
//             guideVector.x0 = i2cJoystick.GetX();
//             guideVector.y0 = i2cJoystick.GetY();
//         }

//         std::this_thread::sleep_for(cGuideUpdateInterval);
//     }
//   };

//   std::thread updateThread{readGuide};
//   std::thread dispayThread{DoPlainView, &dev, std::reference_wrapper<GuideVector>(guideVector), std::reference_wrapper<mutex>(lock)};

//   updateThread.join();
//   dispayThread.join();
// }

/*
//   AdcAxis axisX0{GPIO_NUM_35, 220, 2150, 4095, 10};
//   AdcAxis axisY0{GPIO_NUM_34, 3830, 1680, 60, 10};
  
//   AdcAxis axisX1{GPIO_NUM_39, 120, 2070, 4040, 10};
//   AdcAxis axisY1{GPIO_NUM_36, 4020, 1820, 160, 10};
*/

void DoJoystickTftTest()
{
  const int cFrequencyHz = 26'000'000;

  SpiHelper spiHelper(SpiHelper::eHSpi, cClkPin, cMosiPin, -1, cCsPin, cFrequencyHz);
  TftSpi tft{spiHelper, cDcPin, cResetPin, cBlPin, TftSpi::ScreenSize::e240x320, TftSpi::Orientation::eLandscape};

  tft.DrawFillRect(0, 0, 320, 240, cColorBlack);
  tft.DrawFillRect(10, 10, 200, 240, cColorYellow);

  VgaFont16x32 font16x32;

  IntItem itemX0{25, 25, &font16x32, 15, "x0:%3d", cColorRed, cColorBlack, tft};
  IntItem itemY0{25, 65, &font16x32, 15, "y0:%3d", cColorRed, cColorBlack, tft};

  IntItem itemX1{25, 105, &font16x32, 15, "x1:%3d", cColorRed, cColorBlack, tft};
  IntItem itemY1{25, 145, &font16x32, 15, "y1:%3d", cColorRed, cColorBlack, tft};

  AdcJoystick joystick0({GPIO_NUM_35, 220, 2150, 4095, cAxisXSpan}, {GPIO_NUM_34, 3830, 1680, 60, cAxisYSpan});
  AdcJoystick joystick1({GPIO_NUM_39, 120, 2070, 4040, cAxisXSpan}, {GPIO_NUM_36, 4020, 1820, 160, cAxisYSpan});

  auto callable = [&joystick0, &joystick1, &itemX0, &itemY0, &itemX1, &itemY1]() -> void
  {
    auto [lastValueX, lastValueY] = joystick0.ReadValues();

    for (;;)
    {
      const auto [currentX0, currentY0] = joystick0.ReadValues();
      const auto [currentX1, currentY1] = joystick1.ReadValues();

      itemX0.SetValue(currentX0);
      itemY0.SetValue(currentY0);

      itemX1.SetValue(currentX1);
      itemY1.SetValue(currentY1);

      itemX0.Draw();
      itemY0.Draw();

      itemX1.Draw();
      itemY1.Draw();

      lastValueX = currentX0;
      lastValueY = currentY0;

      th::sleep_for(ch::milliseconds(100));
    }
  };

  thread workThread{callable};
  workThread.join();
}

namespace
{

string IntToStr(int value, uint16_t count)
{
    stringstream out;
    out << std::setw(count) << std::setfill(' ') << value;
    return out.str();
}

  // Vga16x32 vga16x32{};
  // auto *line0 = new TextItem{20, 40, &vga16x32, 4, WHITE, BLACK};
  // auto *line1 = new TextItem{20, 70, &vga16x32, 4, WHITE, BLACK};
  // auto *line2 = new TextItem{20, 110, &vga16x32, 4, WHITE, BLACK};
  // auto *line3 = new TextItem{20, 140, &vga16x32, 4, WHITE, BLACK};

  // auto *valueLine0 = new IntItem{70, 40, &vga16x32, 3, cFmt, WHITE, BLACK};
  // auto *valueLine1 = new IntItem{70, 70, &vga16x32, 3, cFmt, WHITE, BLACK};
  // auto *valueLine2 = new IntItem{70, 110, &vga16x32, 3, cFmt, WHITE, BLACK};
  // auto *valueLine3 = new IntItem{70, 140, &vga16x32, 3, cFmt, WHITE, BLACK};

  // line0->Update("x0: ", 4);
  // line1->Update("y0: ", 4);
  // line2->Update("x1: ", 4);
  // line3->Update("y1: ", 4);

  // vector<IUiItem *> items = {line0, line1, line2, line3
  //     , valueLine0, valueLine1, valueLine2, valueLine3 };

  // GuideVector current{};
  // {
  //   lock_guard<mutex> lock{rMutex};
  //   current = rGuide;
  // }
  // valueLine0->UpdateForce(current.x0);
  // valueLine1->UpdateForce(current.y0);
  // valueLine2->UpdateForce(current.x1);
  // valueLine3->UpdateForce(current.y1);

  // for (;;)
  // {
  //   std::this_thread::sleep_for(cDisplayUpdateInterval);
  //   {
  //     lock_guard<mutex> lock{rMutex};
  //     current = rGuide;
  //   }
  //   valueLine0->Update(current.x0);
  //   valueLine1->Update(current.y0);
  //   valueLine2->Update(current.x1);
  //   valueLine3->Update(current.y1);
     
  //   {
  //     // TimeMeter meter{"update time: "};
  //     for (const auto it : items)
  //     {
  //       it->Draw();
  //     }
  //   }
  // }
} // namespace

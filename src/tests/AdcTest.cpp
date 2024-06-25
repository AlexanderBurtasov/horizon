#include "driver/adc.h"
// std
#include <iostream>
#include <thread>
// prj
#include "ui/TextItem.h"
#include "helpers/AdcJoystick.h"
#include "helpers/MotorDriver.h"
#include "helpers/Servo.h"
#include "helpers/SmoothMotor.h"
#include "TftSpi.h"
#include "SpiHelper.h"
#include "VgaFont.h"

using std::cout;
using std::endl;
using std::thread;

namespace ch = std::chrono;
namespace th = std::this_thread;
using std::cout;
using std::endl;

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
}

const uint16_t cAxisXSpan = 10;
const uint16_t cAxisYSpan = 10;

float SpanToAngle(int value, uint16_t span)
{
  float mult = static_cast<float>(value) / span;
  if (mult < -1.f)
  {
    mult = -1.f;
  }
  else if (mult > 1.f)
  {
    mult = 1.f;
  }
  return 90.f + 90.f * mult;
}

void DoAdcJoystickTest()
{
  AdcJoystick joystick({GPIO_NUM_35, 0, 1900, 4095, cAxisXSpan}, {GPIO_NUM_34, 0, 1900, 4095, cAxisYSpan});
  auto [lastValueX, lastValueY] = joystick.ReadValues();
  Servo servoX(GPIO_NUM_17, LEDC_CHANNEL_0);
  Servo servoY(GPIO_NUM_5, LEDC_CHANNEL_1);

  servoX.Write(SpanToAngle(lastValueX, cAxisXSpan));
  servoY.Write(SpanToAngle(lastValueY, cAxisYSpan));

  for (;;)
  {
    bool updated = false;
    const auto [currentX, currentY] = joystick.ReadValues();
    if (lastValueX != currentX)
    {
      lastValueX = currentX;
      updated = true;
    }
    if (lastValueY != currentY)
    {
      lastValueY = currentY;
      updated = true;
    }
    if (updated)
    {
      cout << "x: " << lastValueX << "; y: " << lastValueY << endl;
      servoX.Write(SpanToAngle(lastValueX, cAxisXSpan));
      servoY.Write(SpanToAngle(lastValueY, cAxisYSpan));
    }

    th::sleep_for(ch::milliseconds(100));
  }
}

const uint16_t cColorBlack = 0x0000;
const auto cColorYellow = TftSpi::ColorRgb(255, 255, 0);
const auto cColorRed = TftSpi::ColorRgb(255, 0, 0);
const char *cMessage = "Hells Bells!";

void DoAdcJoystickTftTest()
{
//const int cFrequencyHz = 26'000'000;

  SpiHelper spiHelper(SpiHelper::eHSpi, cClkPin, cMosiPin, -1, cCsPin, cFrequencyHz);
  TftSpi tft{spiHelper, cDcPin, cResetPin, cBlPin, TftSpi::ScreenSize::e240x280, TftSpi::Orientation::ePortrait};

  tft.DrawFillRect(0, 0, 240, 280, cColorBlack);
  tft.DrawFillRect(10, 10, 220, 260, cColorYellow);

  VgaFont16x32 font16x32;

  IntItem itemX{25, 25, &font16x32, 15, "x0:%3d", cColorRed, cColorBlack, tft};
  IntItem itemY{25, 65, &font16x32, 15, "y0:%3d", cColorRed, cColorBlack, tft};
  AdcJoystick joystick({GPIO_NUM_35, 0, 1900, 4095, cAxisXSpan}, {GPIO_NUM_34, 0, 1900, 4095, cAxisYSpan});

//  Servo servoX(GPIO_NUM_17, LEDC_CHANNEL_0);
  SmoothMotor motor{GPIO_NUM_17, LEDC_CHANNEL_0, static_cast<uint8_t>(cAxisXSpan)};
  //MotorDriverPwm motor{GPIO_NUM_5, GPIO_NUM_18, LEDC_CHANNEL_1, LEDC_CHANNEL_2, cAxisXSpan};
  //motor.WriteLevel(5);

//  Servo servoY(GPIO_NUM_5, LEDC_CHANNEL_1);
//  Servo servoZ(GPIO_NUM_18, LEDC_CHANNEL_2);

  auto callable = [&joystick, &motor, /*&servoY, &servoZ,*/ &itemX, &itemY]() -> void
  {
    auto [lastValueX, lastValueY] = joystick.ReadValues();

//    servoX.Write(SpanToAngle(lastValueX, cAxisXSpan));
//    servoY.Write(SpanToAngle(lastValueY, cAxisYSpan));
//    servoZ.Write(SpanToAngle(lastValueY, cAxisYSpan));

    for (;;)
    {
      const auto [currentX, currentY] = joystick.ReadValues();
      itemX.SetValue(currentX);
      itemY.SetValue(currentY);

      if (10 == currentY && currentY != lastValueY)
      {
        motor.Arm();
        cout << "arm done" << endl;
      }

      //servoX.Write(SpanToAngle(currentX, cAxisXSpan));
      motor.WriteLevel(currentX);
      //servoY.Write(SpanToAngle(currentY, cAxisYSpan));
      //servoZ.Write(SpanToAngle(currentY, cAxisYSpan));

      itemX.Draw();
      itemY.Draw();

      lastValueX = currentX;
      lastValueY = currentY;

      th::sleep_for(ch::milliseconds(100));
    }
  };

  thread workThread{callable};
  workThread.join();
}

void DoAdcJoystickLn298Test()
{
  SpiHelper spiHelper(SpiHelper::eHSpi, cClkPin, cMosiPin, -1, cCsPin, cFrequencyHz);
  TftSpi tft{spiHelper, cDcPin, cResetPin, cBlPin, TftSpi::ScreenSize::e240x280, TftSpi::Orientation::ePortrait};

  tft.DrawFillRect(0, 0, 240, 280, cColorBlack);
  tft.DrawFillRect(10, 10, 220, 260, cColorYellow);

  VgaFont16x32 font16x32;

  IntItem itemX{25, 25, &font16x32, 15, "x0:%3d", cColorRed, cColorBlack, tft};
  IntItem itemY{25, 65, &font16x32, 15, "y0:%3d", cColorRed, cColorBlack, tft};
  AdcJoystick joystick({GPIO_NUM_35, 0, 1900, 4095, cAxisXSpan}, {GPIO_NUM_34, 0, 1900, 4095, cAxisYSpan});

//  Servo servoX(GPIO_NUM_17, LEDC_CHANNEL_0);
  MotorDriverLn298N motor(GPIO_NUM_17, LEDC_CHANNEL_0, GPIO_NUM_5, GPIO_NUM_18, 10);

  auto callable = [&joystick, &motor, &itemX, &itemY]() -> void
  {
    auto [lastValueX, lastValueY] = joystick.ReadValues();
    motor.WriteLevel(0);

    for (;;)
    {
      const auto [currentX, currentY] = joystick.ReadValues();

      itemX.SetValue(currentX);
      itemY.SetValue(currentY);
      lastValueX = currentX;

      if (currentY != lastValueY)
      {
        cout << "new level: " << static_cast<int16_t>(currentY) << endl;
        motor.WriteLevel(currentY);
        lastValueY = currentY;
      }

      itemX.Draw();
      itemY.Draw();

      th::sleep_for(ch::milliseconds(100));
    }
  };

  thread workThread{callable};
  workThread.join();
}

void DoAdcChannelTest(adc1_channel_t adcChannel)
{
  cout << "DoAdcChanneTest()" << endl;
  gpio_num_t aPin; 
  auto r = ::adc1_pad_get_io_num(adcChannel, &aPin); // gpio_12
  assert(r == ESP_OK);

  r = ::adc1_config_channel_atten(adcChannel, ADC_ATTEN_DB_11);
  assert(r == ESP_OK);

  int vX = ::adc1_get_raw(adcChannel);

  cout << "first: " << vX << endl;

  for (;;)
  {
    int value = ::adc1_get_raw(adcChannel);
    if (vX != value)
    {
      cout << "value: " << value << endl;
    }
    th::sleep_for(ch::milliseconds(100));
  }

  // r = adc2_pad_get_io_num(ADC2_CHANNEL_6, &bPin);      // gpio 14
  // assert(r == ESP_OK);
  // r = adc2_config_channel_atten(ADC2_CHANNEL_6, ADC_ATTEN_DB_11);
  // assert(r == ESP_OK);                                                                                                                                                                                                                                                                                                                                                                                                                               
} 

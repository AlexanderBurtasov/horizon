#pragma once
// std
#include <cstdint>
// esp-idf
#include "driver/gpio.h"

class SpiHelper;
class VgaFont;

class TftSpi
{
public:
  enum class ScreenSize
  {
    e240x240
    , e240x280
    , e240x320
  };

  enum class Orientation
  {
    eLandscape
    , ePortrait
  };

public:
  TftSpi(SpiHelper &rSpiHelper
    , int16_t dcPinNum, int16_t resetPinNum, int16_t blPinNum
    , ScreenSize screenSize, Orientation orientation);

  void DrawPixel(uint16_t x, uint16_t y, uint16_t color);
  void DrawBresLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
  void DrawFastBresLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
  void DrawFillRect(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t color);
  void DrawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color);

  void DrawChar(uint16_t x0, uint16_t y0, const char symbol, const VgaFont &font, uint16_t fontColor, uint16_t backColor);
  void DrawChar(uint16_t x0, uint16_t y0, const char symbol, const VgaFont &font, uint16_t fontColor);

  void DrawString(uint16_t x0, uint16_t y0, const char *const str, const VgaFont &font, uint16_t fontColor, uint16_t backColor);
  void DrawString(uint16_t x0, uint16_t y0, const char *const str, const VgaFont &font, uint16_t fontColor);

  static uint16_t ColorRgb(uint8_t red, uint8_t green, uint8_t blue);

  uint16_t GetWidth() const;
  uint16_t GetHeight() const;

protected:
  void Init(ScreenSize screenSize, Orientation orientation);

  void WriteCommand(uint8_t cmd);

  void WriteData(uint8_t value);
  void WriteData(uint16_t value);
  void WriteData(uint32_t value);

  void WriteAddress(uint16_t addr1, uint16_t addr2);
  void WriteBytes(const uint8_t *bytes, uint16_t length);

protected:
  SpiHelper &m_rSpiHelper;
  gpio_num_t m_dcPin = GPIO_NUM_NC;
  gpio_num_t m_resetPin = GPIO_NUM_NC;
  gpio_num_t m_blPin = GPIO_NUM_NC;

  // 240x280
  uint16_t m_width;
  uint16_t m_height;
  uint8_t m_offsetX = 0;
  uint8_t m_offsetY = 0;

private:
  TftSpi(const TftSpi &) = delete;
  TftSpi &operator=(const TftSpi &) = delete;
  TftSpi(TftSpi &&) = delete;
  TftSpi &operator=(TftSpi &&) = delete;
};

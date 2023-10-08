#pragma once
// std
#include <cstdint>

class SpiBus;

class TftSpi
{
public:
  TftSpi(SpiBus &rSpiBus
    , int16_t dcPinNum, int16_t resetPinNum, int16_t blPinNum
    , uint16_t width, uint16_t height);

  void PutPixel(uint16_t x, uint16_t y, uint16_t color);
  void DrawLinePixel(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
  void DrawLineRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
  void FillRect(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t color);

  static uint16_t ColorRgb(uint8_t red, uint8_t green, uint8_t blue);

protected:
  void Init();

  void WriteCommand(uint8_t cmd);

  void WriteData(uint8_t value);
  void WriteData(uint16_t value);
  void WriteData(uint32_t value);

  void WriteAddress(uint16_t addr1, uint16_t addr2);
  void WriteBytes(const uint8_t *bytes, uint16_t length);

protected:
  SpiBus &m_rSpiBus;
  int16_t m_dcPinNum = -1;
  int16_t m_resetPinNum = -1;
  int16_t m_blPinNum = -1;

  // 240x280
  uint16_t m_width;
  uint16_t m_height;
  uint8_t m_offsetX = 0;
  uint8_t m_offsetY = 20;

private:
  TftSpi(const TftSpi &) = delete;
  TftSpi &operator=(const TftSpi &) = delete;
  TftSpi(TftSpi &&) = delete;
  TftSpi &operator=(TftSpi &&) = delete;
};

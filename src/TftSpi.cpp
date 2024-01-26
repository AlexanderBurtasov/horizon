#include "TftSpi.h"
// std
#include <thread>
// esp-idf
#include <driver/gpio.h>
// prj
#include "SpiHelper.h"
#include "VgaFont.h"

TftSpi::TftSpi(SpiHelper &rSpiHelper
  , int16_t dcPinNum, int16_t resetPinNum, int16_t blPinNum
  , ScreenSize screenSize, Orientation orientation)
    : m_rSpiHelper(rSpiHelper)
    , m_dcPin{static_cast<gpio_num_t>(dcPinNum)}
    , m_resetPin{static_cast<gpio_num_t>(resetPinNum)}
    , m_blPin{static_cast<gpio_num_t>(blPinNum)}
{
  // cs == 0 at all time
  m_rSpiHelper.CsLow();

  gpio_reset_pin(m_dcPin);
  gpio_set_direction(m_dcPin, GPIO_MODE_OUTPUT);
  gpio_set_level(m_dcPin, 0);

  if (m_resetPin != GPIO_NUM_NC)
  {
    ::gpio_reset_pin(m_resetPin);
    ::gpio_set_direction(m_resetPin, GPIO_MODE_OUTPUT);

    ::gpio_set_level(m_resetPin, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ::gpio_set_level(m_resetPin, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ::gpio_set_level(m_resetPin, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  if (m_blPin != GPIO_NUM_NC)
  {
    gpio_reset_pin(m_blPin);
    gpio_set_direction(m_blPin, GPIO_MODE_OUTPUT);
    gpio_set_level(m_blPin, 0);
  }

  Init(screenSize, orientation);
}

void TftSpi::DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  uint16_t _x = x + m_offsetX;
  uint16_t _y = y + m_offsetY;

  WriteCommand(0x2A); // set column(x) address
  WriteAddress(_x, _x);

  WriteCommand(0x2B); // set Page(y) address
  WriteAddress(_y, _y);
  WriteCommand(0x2C); // Memory Write
  WriteData(color);
}

void TftSpi::DrawBresLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  /* distance between two points */
  const uint16_t dx = (x2 > x1) ? x2 - x1 : x1 - x2;
  const uint16_t dy = (y2 > y1) ? y2 - y1 : y1 - y2;

  /* direction of two point */
  const int8_t sx = (x2 > x1) ? 1 : -1;
  const int8_t sy = (y2 > y1) ? 1 : -1;

  if (dx > dy)
  {
    int16_t E = -dx;
    for (uint16_t i = 0; i <= dx; ++i)
    {
      DrawPixel(x1, y1, color);
      x1 += sx;
      E += 2 * dy;
      if (E >= 0)
      {
        y1 += sy;
        E -= 2 * dx;
      }
    }
  }
  else
  {
    int16_t E = -dy;
    for (uint16_t i = 0; i <= dy; ++i)
    {
      DrawPixel(x1, y1, color);
      y1 += sy;
      E += 2 * dx;
      if (E >= 0)
      {
        x1 += sx;
        E -= 2 * dy;
      }
    }
  }
}

void TftSpi::DrawFastBresLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  // draw line from left side anyway
  if (x1 > x2)
  {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }

  /* distance between two points */
  const uint16_t dx = x2 - x1;
  const uint16_t dy = (y2 > y1) ? y2 - y1 : y1 - y2;

  /* direction of two point */
  const int8_t sy = (y2 > y1) ? 1 : -1;

  /* inclination < 1 */
  if (dx > dy)
  {
    int16_t E = -dx;
    int width = 0;
    int currentX = x1;
    for (uint16_t i = 0; i <= dx; i++)
    {
      x1 += 1;
      width += 1;
      E += 2 * dy;
      if (E >= 0)
      {
        if (width > 0)
        {
          DrawFillRect(currentX, y1, width, 1, color);
        }
        y1 += sy;
        E -= 2 * dx;
        currentX = x1;
        width = 0;
      }
    }
    if (width > 0)
    {
      DrawFillRect(currentX, y1, width, 1, color);
    }
  }
  else
  {
    int16_t E = -dy;
    int height = 0;
    int currentY = y1;
    for (uint16_t i = 0; i <= dy; i++)
    {
      y1 += sy;
      E += 2 * dx;
      height += sy;
      if (E >= 0)
      {
        if (height > 0)
        {
          DrawFillRect(x1, currentY, 1, height, color);
        }
        else
        {
          DrawFillRect(x1, currentY + height + 1, 1, -height, color);
        }
        x1 += 1;
        E -= 2 * dy;
        currentY = y1;
        height = 0;
      }
    }
    if (height > 0)
    {
      DrawFillRect(x1, currentY, 1, height, color);
    }
    else
    {
      DrawFillRect(x1, currentY + height + 1, 1, -height, color);
    }
  }
}

void TftSpi::DrawFillRect(uint16_t topX, uint16_t topY, uint16_t width, uint16_t height, uint16_t color)
{
  const uint16_t _x1 = topX + m_offsetX;
  const uint16_t _x2 = _x1 + width - 1;
  const uint16_t _y1 = topY + m_offsetY;
  const uint16_t _y2 = _y1 + height - 1;

  WriteCommand(0x2A);  // set column(x) address
  WriteAddress(_x1, _x2);
  WriteCommand(0x2B);  // set Page(y) address
  WriteAddress(_y1, _y2);
  WriteCommand(0x2C);  // Memory Write

  static uint8_t tmpBuf[1024];
  gpio_set_level(m_dcPin, 1);

  const uint32_t byteCount = width * height * 2;
  const uint16_t fullCount = byteCount / sizeof(tmpBuf);

  const uint32_t restCount = byteCount - fullCount * sizeof(tmpBuf);
  uint16_t bufSize = fullCount ? sizeof(tmpBuf) / 2 : restCount / 2; 

  uint16_t *ptr = reinterpret_cast<uint16_t *>(tmpBuf);
  for (uint16_t j = 0; j < bufSize; ++j)
  {
    *(ptr++) = color;
  }

  for (uint16_t i = 0; i < fullCount; ++i)
  {
    m_rSpiHelper.TransferData(tmpBuf, sizeof(tmpBuf));
  }

  if (restCount)
  {
    m_rSpiHelper.TransferData(tmpBuf, restCount);
  }
}

void TftSpi::DrawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color)
{
  auto innerFunc = [this, x0, y0, color](int x, int y) -> void
  {
    DrawPixel(x + x0, y + y0, color);
    DrawPixel(x + x0, -y + y0, color);
    DrawPixel(-x + x0, -y + y0, color);
    DrawPixel(-x + x0, y + y0, color);
    DrawPixel(y + x0, x + y0, color);
    DrawPixel(y + x0, -x + y0, color);
    DrawPixel(-y + x0, -x + y0, color);
    DrawPixel(-y + x0, x + y0, color);
  };

  int x = 0, y = radius, d = 3 - (2 * radius);
  innerFunc(x, y);

  while (x <= y)
  {
    if (d <= 0)
    {
      d = d + (4 * x) + 6;
    }
    else
    {
      d = d + (4 * x) - (4 * y) + 10;
      y = y - 1;
    }
    x = x + 1;
    innerFunc(x, y);
  }
}

void TftSpi::DrawChar(uint16_t x0, uint16_t y0, const char symbol, const VgaFont &font, uint16_t fontColor, uint16_t backColor)
{
  const auto [width, height] = font.GetDim();

  const uint16_t _x1 = x0 + m_offsetX;
  const uint16_t _x2 = _x1 + width - 1;
  const uint16_t _y1 = y0 + m_offsetY;
  const uint16_t _y2 = _y1 + height - 1;

  WriteCommand(0x2A); // set column(x) address
  WriteAddress(_x1, _x2);
  WriteCommand(0x2B); // set Page(y) address
  WriteAddress(_y1, _y2);
  WriteCommand(0x2C); // Memory Write

  gpio_set_level(m_dcPin, 1);
  const uint16_t *pixmap = font.BuildPixmap(symbol, fontColor, backColor);
  m_rSpiHelper.TransferData(reinterpret_cast<const uint8_t *>(pixmap), width * height * sizeof(*pixmap));
}

void TftSpi::DrawChar(uint16_t x0, uint16_t y0, const char symbol, const VgaFont &font, uint16_t fontColor)
{
  const auto [width, height] = font.GetDim();
  const uint16_t *pixmap = font.BuildPixmap(symbol, fontColor, 0x0);

  uint16_t pos = 0;
  for (uint8_t j = 0; j < height; ++j)
  {
    for (uint8_t i = 0; i < width; ++i)
    {
      if (0x0 != pixmap[pos])
      {
        DrawPixel(x0+i, y0+j, fontColor);
      }
      ++pos;
    }
  }
}

void TftSpi::DrawString(uint16_t x0, uint16_t y0, const char *const str, const VgaFont &font, uint16_t fontColor, uint16_t backColor)
{
  const auto [width, height] = font.GetDim();
  const char *symbol = str;
  while (x0 + width < m_width && y0 + height < m_height && *symbol != '\0')
  {
    DrawChar(x0, y0, *symbol, font, fontColor, backColor);
    ++symbol;
    x0 += width;
  }
}

void TftSpi::DrawString(uint16_t x0, uint16_t y0, const char *const str, const VgaFont &font, uint16_t fontColor)
{
  const auto [width, height] = font.GetDim();
  const char *symbol = str;
  while (x0 + width < m_width && y0 + height < m_height && *symbol != '\0')
  {
    DrawChar(x0, y0, *symbol, font, fontColor);
    ++symbol;
    x0 += width;
  }
}

uint16_t TftSpi::GetWidth() const
{
  return m_width;
}

uint16_t TftSpi::GetHeight() const
{
  return m_height;
}

uint16_t TftSpi::ColorRgb(uint8_t r, uint8_t g, uint8_t b)
{
  // value reversed already
  const uint16_t v1 = (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
  return (v1 & 0xff) << 8 | v1 >> 8;
}

void TftSpi::WriteCommand(uint8_t cmd)
{
  ::gpio_set_level(m_dcPin, 0);
  m_rSpiHelper.TransferIntValue(&cmd, sizeof(cmd));
}

void TftSpi::WriteData(uint8_t value)
{
  ::gpio_set_level(m_dcPin, 1);
  m_rSpiHelper.TransferIntValue(&value, sizeof(value));
}

void TftSpi::WriteData(uint16_t value)
{
  ::gpio_set_level(m_dcPin, 1);
  m_rSpiHelper.TransferIntValue(&value, sizeof(value));
}

void TftSpi::WriteAddress(uint16_t addr1, uint16_t addr2)
{
  ::gpio_set_level(m_dcPin, 1);
  uint8_t value[] =
  {
    static_cast<uint8_t>((addr1 >> 8) & 0xFF),
    static_cast<uint8_t>(addr1 & 0xFF),
    static_cast<uint8_t>((addr2 >> 8) & 0xFF),
    static_cast<uint8_t>(addr2 & 0xFF)
  };

  m_rSpiHelper.TransferIntValue(value, sizeof(value));
}

void TftSpi::Init(ScreenSize screenSize, Orientation orientation)
{
  WriteCommand(0x01); // Software Reset
  std::this_thread::sleep_for(std::chrono::milliseconds(150));

  WriteCommand(0x11); // Sleep Out
  std::this_thread::sleep_for(std::chrono::milliseconds(255));

  WriteCommand(0x3A); // Interface Pixel Format
  WriteData(static_cast<uint8_t>(0x55));
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  WriteCommand(0x36); // Memory Data Access Control
  const uint8_t regValue = static_cast<uint8_t>(0x0 | (Orientation::ePortrait == orientation ? 0x00 : 0x20 | 0x40));
  WriteData(regValue);

  WriteCommand(0x2A); // Column Address Set
  WriteData(static_cast<uint8_t>(0x00));
  WriteData(static_cast<uint8_t>(0x00));
  WriteData(static_cast<uint8_t>(0x00));
  WriteData(static_cast<uint8_t>(0xF0));

  WriteCommand(0x2B); // Row Address Set
  WriteData(static_cast<uint8_t>(0x00));
  WriteData(static_cast<uint8_t>(0x00));
  WriteData(static_cast<uint8_t>(0x00));
  WriteData(static_cast<uint8_t>(0xF0));

  WriteCommand(0x21); // Display Inversion On
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  WriteCommand(0x13); // Normal Display Mode On
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  WriteCommand(0x29); // Display ON
  std::this_thread::sleep_for(std::chrono::milliseconds(255));

  if (m_blPin != GPIO_NUM_NC)
  {
    gpio_set_level(m_blPin, 1);
  }

  if (ScreenSize::e240x240 == screenSize)
  {
    m_offsetX = 0;
    m_offsetY = 0;
    m_width = 240;
    m_height = 280;
  }
  else if (ScreenSize::e240x280 == screenSize)
  {
    m_offsetX = 0;
    m_offsetY = 20;
    m_width = 240;
    m_height = 280;
  }
  else if (ScreenSize::e240x320 == screenSize)
  {
    m_offsetX = 0;
    m_offsetY = 0;
    m_width = 240;
    m_height = 320;
  }

  if (Orientation::eLandscape == orientation)
  {
    std::swap(m_width, m_height);
    std::swap(m_offsetX, m_offsetY);
  }
}

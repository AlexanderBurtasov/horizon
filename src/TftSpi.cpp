#include "TftSpi.h"
// std
#include <iostream>
#include <thread>
// esp-idf
#include <driver/gpio.h>
// prj
#include "SpiBus.h"

TftSpi::TftSpi(SpiBus &rSpiBus
  , int16_t dcPinNum, int16_t resetPinNum, int16_t blPinNum
  , uint16_t width, uint16_t height)
    : m_rSpiBus(rSpiBus)
    , m_dcPinNum{dcPinNum}
    , m_resetPinNum{resetPinNum}
    , m_blPinNum{blPinNum}
    , m_width{width}, m_height{height}
{
  gpio_reset_pin(static_cast<gpio_num_t>(m_dcPinNum));
  gpio_set_direction(static_cast<gpio_num_t>(m_dcPinNum), GPIO_MODE_OUTPUT);
  gpio_set_level(static_cast<gpio_num_t>(m_dcPinNum), 0);

  if (m_resetPinNum >= 0)
  {
    ::gpio_reset_pin(static_cast<gpio_num_t>(m_resetPinNum));
    ::gpio_set_direction( static_cast<gpio_num_t>(m_resetPinNum), GPIO_MODE_OUTPUT);

    ::gpio_set_level(static_cast<gpio_num_t>(m_resetPinNum), 1);		
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ::gpio_set_level(static_cast<gpio_num_t>(m_resetPinNum), 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ::gpio_set_level(static_cast<gpio_num_t>(m_resetPinNum), 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  if (m_blPinNum >= 0)
  {
    gpio_reset_pin(static_cast<gpio_num_t>(m_blPinNum));
    gpio_set_direction(static_cast<gpio_num_t>(m_blPinNum), GPIO_MODE_OUTPUT);
    gpio_set_level(static_cast<gpio_num_t>(m_blPinNum), 0);
  }

  Init();
}

void TftSpi::PutPixel(uint16_t x, uint16_t y, uint16_t color)
{
  uint16_t _x = x + 0;
  uint16_t _y = y + m_offsetY;

  WriteCommand(0x2A);	// set column(x) address
  WriteAddress(_x, _x);

  WriteCommand(0x2B);	// set Page(y) address
  WriteAddress(_y, _y);
  WriteCommand(0x2C);	//	Memory Write
  WriteData(color);
}

void TftSpi::DrawLinePixel(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	/* distance between two points */
  const uint16_t dx = ( x2 > x1 ) ? x2 - x1 : x1 - x2;
  const uint16_t dy = ( y2 > y1 ) ? y2 - y1 : y1 - y2;

  /* direction of two point */
  const int8_t sx = ( x2 > x1 ) ? 1 : -1;
  const int8_t sy = ( y2 > y1 ) ? 1 : -1;

  if (dx > dy)
  {
    int16_t E = -dx;
    for (uint16_t i = 0; i <= dx; ++i)
    {
      PutPixel(x1, y1, color);
			x1 += sx;
			E += 2 * dy;
			if ( E >= 0 )
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
      PutPixel(x1, y1, color);
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

void TftSpi::DrawLineRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  /* distance between two points */
  const uint16_t dx = (x2 > x1) ? x2 - x1 : x1 - x2;
  const uint16_t dy = (y2 > y1) ? y2 - y1 : y1 - y2;

  /* direction of two point */
  const int8_t sx = (x2 > x1) ? 1 : -1;
  const int8_t sy = (y2 > y1) ? 1 : -1;

  /* inclination < 1 */
  if (dx > dy)
  {
    int16_t E = -dx;
    int16_t width = 0;
    for (uint16_t i = 0; i <= dx; ++i)
    {
      width += sx;
      E += 2 * dy;
      if (E >= 0)
      {
        if (width > 0)
        {
          FillRect(x1, y1-1, width, 1, color);
        }
        else
        {
          FillRect(x1 + width, y1-1, -width, 1, color);
        }
        y1 += sy;
        x1 += width;
        width = 0;
        E -= 2 * dx;
      }
    }
    if (width != 0)
    {
			if (width > 0)
       	FillRect(x1, y1, width, 1, color);
			else
				FillRect(x1 + width, y1, -width, 1, color);
    }
  }
  else // inclination >= 1
  {
    int E = -dy;
    int height = 0;
    for (uint16_t i = 0; i <= dy; i++)
    {
      height += sy;
      E += 2 * dx;
      if (E >= 0)
      {
        if (height > 0)
        {
          FillRect(x1, y1, 1, height, color);
        }
        else
        {
          FillRect(x1, y1+height, 1, -height, color);
        }
        x1 += sx;
        y1 += height;
        height = 0;
        E -= 2 * dy;
      }
    }
    if (height != 0)
    {
		  if (height > 0)
       	FillRect(x1, y1, 1, height, color);
			else
				FillRect(x1, y1+height, 1, -height, color);
    }
  }
}

void TftSpi::FillRect(uint16_t topX, uint16_t topY, uint16_t width, uint16_t height, uint16_t color)
{
  uint16_t _x1 = topX + m_offsetX;
  uint16_t _x2 = _x1 + width - 1;
  uint16_t _y1 = topY + m_offsetY;
  uint16_t _y2 = _y1 + height - 1;

  WriteCommand(0x2A);	// set column(x) address
  WriteAddress(_x1, _x2);
  WriteCommand(0x2B);	// set Page(y) address
  WriteAddress(_y1, _y2);
  WriteCommand(0x2C);	//	Memory Write

  static uint8_t tmpBuf[1024];
  gpio_set_level(static_cast<gpio_num_t>(m_dcPinNum), 1);
  const uint8_t w0 = (color >> 8) & 0xFF;
  const uint8_t w1 = color & 0xFF;

  uint32_t byteCount = width * height * 2;
  const uint16_t fullCount = byteCount / sizeof(tmpBuf);

  const uint32_t restCount = byteCount - fullCount * sizeof(tmpBuf);
  uint16_t bufSize = fullCount ? sizeof(tmpBuf) / 2 : restCount / 2; 

  uint8_t *ptr = tmpBuf;
  for (uint16_t j = 0; j < bufSize; ++j)
  {
    *(ptr++) = w0;
    *(ptr++) = w1;
  }

  for (uint16_t i = 0; i < fullCount; ++i)
  {
    m_rSpiBus.TransferData(tmpBuf, sizeof(tmpBuf));
  }

  if (restCount)
  {
    m_rSpiBus.TransferData(tmpBuf, restCount);
  }
}

uint16_t TftSpi::ColorRgb(uint8_t r,uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void TftSpi::WriteCommand(uint8_t cmd)
{
  ::gpio_set_level(static_cast<gpio_num_t>(m_dcPinNum), 0);
  m_rSpiBus.TransferIntValue(&cmd, sizeof(cmd));
}

void TftSpi::WriteData(uint8_t value)
{
  ::gpio_set_level(static_cast<gpio_num_t>(m_dcPinNum), 1);
  m_rSpiBus.TransferIntValue(&value, sizeof(value));
}

void TftSpi::WriteData(uint16_t value)
{
  ::gpio_set_level(static_cast<gpio_num_t>(m_dcPinNum), 1);
  uint16_t v1 = ((value >> 8) & 0xff) | (value & 0xff);
  m_rSpiBus.TransferIntValue(&v1, sizeof(v1));
}

void TftSpi::WriteAddress(uint16_t addr1, uint16_t addr2)
{
  ::gpio_set_level(static_cast<gpio_num_t>(m_dcPinNum), 1);
  uint8_t value[] =
  {
    static_cast<uint8_t>((addr1 >> 8) & 0xFF),
    static_cast<uint8_t>(addr1 & 0xFF),
    static_cast<uint8_t>((addr2 >> 8) & 0xFF),
    static_cast<uint8_t>(addr2 & 0xFF)
  };

  m_rSpiBus.TransferIntValue(value, sizeof(value));
}

void TftSpi::Init()
{
  WriteCommand(0x01);	//Software Reset
  std::this_thread::sleep_for(std::chrono::milliseconds(150));

  WriteCommand(0x11);	//Sleep Out
  std::this_thread::sleep_for(std::chrono::milliseconds(255));
	
  WriteCommand(0x3A);	//Interface Pixel Format
  WriteData(static_cast<uint8_t>(0x55));
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
	
  WriteCommand(0x36);	//Memory Data Access Control
  WriteData(static_cast<uint8_t>(0x00));

  WriteCommand(0x2A);	//Column Address Set
  WriteData(static_cast<uint8_t>(0x00));
  WriteData(static_cast<uint8_t>(0x00));
  WriteData(static_cast<uint8_t>(0x00));
  WriteData(static_cast<uint8_t>(0xF0));

  WriteCommand(0x2B);	//Row Address Set
  WriteData(static_cast<uint8_t>(0x00));
  WriteData(static_cast<uint8_t>(0x00));
  WriteData(static_cast<uint8_t>(0x00));
  WriteData(static_cast<uint8_t>(0xF0));

  WriteCommand(0x21);	//Display Inversion On
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  WriteCommand(0x13);	//Normal Display Mode On
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  WriteCommand(0x29);	//Display ON
  std::this_thread::sleep_for(std::chrono::milliseconds(255));

  if (m_blPinNum >= 0)
  {
    gpio_set_level(static_cast<gpio_num_t>(m_blPinNum), 1);
  }
}

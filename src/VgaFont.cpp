#include "VgaFont.h"
// std
#include "fontbitmap.h"

using std::tuple;
//-----------------------------------------------------------------------------------------------
//                                  class VgaFont
//-----------------------------------------------------------------------------------------------
uint16_t VgaFont::m_buffer[32*32];

VgaFont::VgaFont(uint8_t width, uint8_t height, const uint8_t *pMask)
  : m_width{width}, m_height{height}, m_pMask{pMask}
{}

tuple<uint8_t, uint8_t> VgaFont::GetDim() const
{
  return {m_width, m_height};
}

const uint16_t *VgaFont::BuildPixmap(char symbol, uint16_t fontColor, uint16_t backColor) const
{
  const uint8_t cBit7 = 0x80;
  const uint8_t cBit6 = 0x40;
  const uint8_t cBit5 = 0x20;
  const uint8_t cBit4 = 0x10;
  const uint8_t cBit3 = 0x08;
  const uint8_t cBit2 = 0x04;
  const uint8_t cBit1 = 0x02;
  const uint8_t cBit0 = 0x01;

  uint16_t *current = m_buffer;
  uint16_t passCount = 0, size = 0, each = 0;

  if (8 == m_height)
  {
    passCount = 1;
    size = 8;
    each = 0;
  }
  else if (16 == m_height)
  {
    passCount = 2;
    size = 16;
    each = 8;
  }
  else if (32 == m_height)
  {
    each = 16;
    passCount = 4;
    size = 64;
  }
  for (uint16_t line = 0; line < passCount; ++line)
  {
    uint16_t idx = (symbol) * size + (each * line);
    for (uint16_t k = 0; k < m_width; ++k)
    {
      const uint16_t s = m_pMask[idx + k];
      *(current++) = (s & cBit7) != 0 ? fontColor : backColor;
      *(current++) = (s & cBit6) != 0 ? fontColor : backColor;
      *(current++) = (s & cBit5) != 0 ? fontColor : backColor;
      *(current++) = (s & cBit4) != 0 ? fontColor : backColor;
      *(current++) = (s & cBit3) != 0 ? fontColor : backColor;
      *(current++) = (s & cBit2) != 0 ? fontColor : backColor;
      *(current++) = (s & cBit1) != 0 ? fontColor : backColor;
      *(current++) = (s & cBit0) != 0 ? fontColor : backColor;
    }
  }

  return m_buffer;
}
//-----------------------------------------------------------------------------------------------
//                                  class VgaFont8x8
//-----------------------------------------------------------------------------------------------
VgaFont8x8::VgaFont8x8(): VgaFont{8, 8, gVga8x8Bytes}
{}
//-----------------------------------------------------------------------------------------------
//                                  class VgaFont8x16
//-----------------------------------------------------------------------------------------------
VgaFont8x16::VgaFont8x16(): VgaFont{8, 16, gVga8x16Bytes}
{}
//-----------------------------------------------------------------------------------------------
//                                  class VgaFont16x32
//-----------------------------------------------------------------------------------------------
VgaFont16x32::VgaFont16x32(): VgaFont{16, 32, gVga16x32Bytes}
{}
/*
namespace
{
void DisplayCharOnTft(TFT_t *dev, uint16_t left, uint16_t top, uint8_t symbol, uint8_t offset
  , const uint8_t *const pBytes
  , uint8_t width, uint8_t height
  , uint16_t fontColor, uint16_t backgroundColor);

void DisplayCharOnTft(TFT_t *dev, uint16_t left, uint16_t top, uint8_t symbol, uint8_t offset
  , const uint8_t *const pBytes
  , uint8_t width, uint8_t height
  , uint16_t fontColor);
}

using std::cout;
using std::endl;

uint8_t VgaFont::m_symbolBuffer[32 * 32];
//-----------------------------------------------------------------------------------------------------
//                                        class Vga8x8
//-----------------------------------------------------------------------------------------------------
Vga8x8::Vga8x8(TFT_t *tft): VgaFont{tft}
{
  m_pBytes = gVga8x8Bytes;
}

void Vga8x8::DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color, uint16_t backColor) const
{
  DisplayCharOnTft(m_tft, left, top, symbol, 0x0, m_pBytes, 8, 8, color, backColor);
}

void Vga8x8::DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color) const
{
  DisplayCharOnTft(m_tft, left, top, symbol, 0x0, m_pBytes, 8, 8, color);
}
//-----------------------------------------------------------------------------------------------------
//                                        class Vga8x16
//-----------------------------------------------------------------------------------------------------
Vga8x16::Vga8x16(TFT_t *tft): VgaFont{tft}
{
  m_pBytes = gVga8x16Bytes;
}

void Vga8x16::DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color, uint16_t backColor) const
{
  DisplayCharOnTft(m_tft, left, top, symbol, 0x0, m_pBytes, 8, 16, color, backColor);
}

void Vga8x16::DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color) const
{
  DisplayCharOnTft(m_tft, left, top, symbol, 0x0, m_pBytes, 8, 16, color);
}
//-----------------------------------------------------------------------------------------------------
//                                        class Vga16x32
//-----------------------------------------------------------------------------------------------------
Vga16x32::Vga16x32(TFT_t *tft): VgaFont{tft}
{
  m_pBytes = gVga16x32Bytes;
}

void Vga16x32::DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color, uint16_t backColor) const
{
  DisplayCharOnTft(m_tft, left, top, symbol, 0x0, m_pBytes, 16, 32, color, backColor);
}

void Vga16x32::DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color) const
{
  DisplayCharOnTft(m_tft, left, top, symbol, 0x0, m_pBytes, 16, 32, color);
}

void Vga16x32::DisplayText(const char *text, uint16_t left, uint16_t top, uint16_t color, uint16_t backColor) const
{
  uint16_t pos = 0;
  uint16_t tmpLeft = left;
  while (text[pos] != '\0')
  {
    DisplayChar(text[pos], tmpLeft, top, color, backColor);
    tmpLeft += 16;
    ++pos;
  }
}

namespace
{
const uint8_t cBit7 = 0x80;
const uint8_t cBit6 = 0x40;
const uint8_t cBit5 = 0x20;
const uint8_t cBit4 = 0x10;
const uint8_t cBit3 = 0x08;
const uint8_t cBit2 = 0x04;
const uint8_t cBit1 = 0x02;
const uint8_t cBit0 = 0x01;


void DisplayCharOnTft(TFT_t *dev, uint16_t left, uint16_t top, uint8_t symbol, uint8_t offset
  , const uint8_t *const pBytes
  , uint8_t width, uint8_t height
  , uint16_t fontColor, uint16_t backgroundColor)
{
  static uint16_t buffer[32 * 32];
  uint16_t *current = buffer;

  uint16_t passCount = 0, size = 0, each = 0;

  const uint16_t color0 = ((fontColor >> 8) & 0xFF) | ((fontColor & 0xff) << 8);
  const uint16_t color1 = ((backgroundColor >> 8) & 0xFF) | ((backgroundColor & 0xff) << 8);

  if (8 == height)
  {
    passCount = 1;
    size = 8;
    each = 0;
  }
  else if (16 == height)
  {
    passCount = 2;
    size = 16;
    each = 8;
  }
  else if (32 == height)
  {
    each = 16;
    passCount = 4;
    size = 64;
  }
  for (uint16_t line = 0; line < passCount; ++line)
  {
    uint16_t idx = (symbol - offset) * size + (each * line);
    for (uint16_t k = 0; k < width; ++k)
    {
      const uint16_t s = pBytes[idx + k];
      *(current++) = (s & cBit7) != 0 ? color0 : color1;
      *(current++) = (s & cBit6) != 0 ? color0 : color1;
      *(current++) = (s & cBit5) != 0 ? color0 : color1;
      *(current++) = (s & cBit4) != 0 ? color0 : color1;
      *(current++) = (s & cBit3) != 0 ? color0 : color1;
      *(current++) = (s & cBit2) != 0 ? color0 : color1;
      *(current++) = (s & cBit1) != 0 ? color0 : color1;
      *(current++) = (s & cBit0) != 0 ? color0 : color1;
    }
  }

  DrawRectBuf(dev, left, top, width, height, buffer);
}

void DisplayCharOnTft(TFT_t *dev, uint16_t left, uint16_t top, uint8_t symbol, uint8_t offset
    , const uint8_t *const pBytes
    , uint8_t width, uint8_t height
    , uint16_t fontColor)
{
    uint16_t passCount = 0, size = 0, each = 0;

    const uint16_t color0 = ((fontColor >> 8) & 0xFF) | ((fontColor & 0xff) << 8);

    if (8 == height)
    {
        passCount = 1;
        size = 8;
        each = 0;
    }
    else if (16 == height)
    {
        passCount = 2;
        size = 16;
        each = 8;
    }
    else if (32 == height)
    {
        each = 16;
        passCount = 4;
        size = 64;
    }
    for (uint16_t line = 0; line < passCount; ++line)
    {
        uint16_t idx = (symbol - offset) * size + (each * line);
        for (uint16_t k = 0; k < width; ++k)
        {
            const uint16_t s = pBytes[idx + k];
            if ((s & cBit7) != 0)
            {
                ::lcdDrawPixel(dev, left + k + 1, top, color0);
            }
            if ((s & cBit6) != 0)
            {
                ::lcdDrawPixel(dev, left + k + 2, top, color0);
            }
            if ((s & cBit5) != 0)
            {
                ::lcdDrawPixel(dev, left + k + 3, top, color0);
            }
            if ((s & cBit4) != 0)
            {
                ::lcdDrawPixel(dev, left + k + 4, top, color0);
            }
            if ((s & cBit3) != 0)
            {
                ::lcdDrawPixel(dev, left + k + 5, top, color0);
            }
            if ((s & cBit2) != 0)
            {
                ::lcdDrawPixel(dev, left + k + 6, top, color0);
            }
            if ((s & cBit1) != 0)
            {
                ::lcdDrawPixel(dev, left + k + 7, top, color0);
            }
            if ((s & cBit0) != 0)
            {
                ::lcdDrawPixel(dev, left + k + 8, top, color0);
            }
        }
        ++top;
    }
}

}
*/
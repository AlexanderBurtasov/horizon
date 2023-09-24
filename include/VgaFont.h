#pragma once

#include <vector>

#include "IFont.h"

struct TFT_t;

class VgaFont: public IFont
{
public:
  VgaFont(TFT_t *tft): m_tft{tft}
  {}

  virtual void DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color, uint16_t backColor) const = 0;
  virtual void DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color) const = 0;
  virtual uint8_t GetWidth() const = 0;

protected:
  TFT_t *m_tft = nullptr;
  const uint8_t *m_pBytes = nullptr;
};

class Vga8x8: public VgaFont
{
public:
  Vga8x8(TFT_t *tft);
  void DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color, uint16_t backColor) const override;
  virtual void DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color) const override;

  uint8_t GetWidth() const override
  {
    return 8;
  }
};

class Vga8x16: public VgaFont
{
public:
  Vga8x16(TFT_t *tft);
  void DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color, uint16_t backColor) const override;
  virtual void DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color) const;

  uint8_t GetWidth() const override
  {
    return 8;
  }
};

class Vga16x32: public VgaFont
{
public:
  Vga16x32(TFT_t *tft);
  void DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color, uint16_t backColor) const override;
  virtual void DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color) const;

  void DisplayText(const char *text, uint16_t left, uint16_t top, uint16_t color, uint16_t backColor) const;

  uint8_t GetWidth() const override
  {
    return 16;
  }
};

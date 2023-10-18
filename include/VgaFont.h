#pragma once
// std
#include <cstdint>
#include <tuple>

class VgaFont
{
public:
  const uint16_t *BuildPixmap(char symbol, uint16_t fontColor, uint16_t backColor) const;
  std::tuple<uint8_t, uint8_t> GetDim() const;

protected:
  VgaFont(uint8_t width, uint8_t height, const uint8_t *pMask);

private:
  uint8_t m_width;
  uint8_t m_height;
  const uint8_t *m_pMask = nullptr;

  static uint16_t m_buffer[32*32];
};

class VgaFont8x8: public VgaFont
{
public:
  VgaFont8x8();
};

class VgaFont8x16: public VgaFont
{
public:
  VgaFont8x16();
};

class VgaFont16x32: public VgaFont
{
public:
  VgaFont16x32();
};

/*
class VgaFont: public IFont
{
public:
  VgaFont(TFT_t *tft): m_tft{tft}
  {}

  virtual void DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color, uint16_t backColor) const = 0;
  virtual void DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color) const = 0;
  virtual uint8_t GetWidth() const = 0;

  const uint8_t *const GetBytes() const
  {
    return m_pBytes;
  }

protected:
  TFT_t *m_tft = nullptr;
  const uint8_t *m_pBytes = nullptr;
  static uint8_t m_symbolBuffer[32 * 32];
};

class Vga8x8: public VgaFont
{
public:
  Vga8x8(TFT_t *tft);
  void DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color, uint16_t backColor) const override;
  void DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color) const override;

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
  void DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color) const override;

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
  void DisplayChar(uint8_t symbol, uint16_t left, uint16_t top, uint16_t color) const override;

  void DisplayText(const char *text, uint16_t left, uint16_t top, uint16_t color, uint16_t backColor) const;

  uint8_t GetWidth() const override
  {
    return 16;
  }
};
*/

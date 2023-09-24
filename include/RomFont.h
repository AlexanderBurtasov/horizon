#pragma once

class IRomFont
{
public:
  virtual ~IFont() {};
  virtual uint16_t Width() const = 0;
  virtual uint16_t Height() const = 0;
  virtual uint16_t First() const = 0;
  virtual uint16_t Last() const = 0;
};

class Vga18x16 : public IRomFont
{
public:
  uint16_t Width() const override;
  uint16_t Height() const override;
  uint16_t First() const override;
  uint16_t Last() const override;
};

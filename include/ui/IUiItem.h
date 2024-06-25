#pragma once

class TftSpi;

class IUiItem
{
public:
  IUiItem(TftSpi &rTftSpi): m_rTftSpi(rTftSpi)
  {}

  virtual ~IUiItem()
  {}

  virtual void Draw() const = 0;

protected:
  TftSpi &m_rTftSpi;
};

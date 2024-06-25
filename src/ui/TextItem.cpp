#include "ui/TextItem.h"
#include "VgaFont.h"
#include "TftSpi.h"

#include <iostream>

//-----------------------------------------------------------------------------------------------
//                                    class TextItem
//-----------------------------------------------------------------------------------------------
TextItem::TextItem(uint16_t leftX, uint16_t leftY, VgaFont *pFont, uint16_t bufSize
  , uint16_t fontColor, uint16_t backColor, TftSpi &rTftSpi)
  : IUiItem{rTftSpi}
  , m_leftX{leftX}, m_leftY{leftY}
  , m_pFont{pFont}
  , m_bufSize{bufSize}
  , m_fontColor{fontColor}, m_backColor{backColor}
  , m_bytes{new char[bufSize]}, m_lastBytes{new char[bufSize]}
{
  for (size_t i = 0; i < bufSize; ++i)
  {
    m_bytes[i] = '\0';
    m_lastBytes[i] = '\0';
  }
}

void TextItem::Draw() const
{
  uint16_t tmpLeft = m_leftX;
  const auto &[fontWidth, _] = m_pFont->GetDim();
  for (uint16_t i = 0; i < m_bufSize; ++i)
  {
    if (m_bytes[i] != m_lastBytes[i])
    {
      m_rTftSpi.DrawChar(tmpLeft, m_leftY, m_bytes[i], *m_pFont, m_fontColor, m_backColor);
      m_lastBytes[i] = m_bytes[i];
    }
    tmpLeft += fontWidth;
  }
}

void TextItem::Update(const char *const data, uint16_t bufSize)
{
  const uint16_t copySize = bufSize > m_bufSize ? m_bufSize : bufSize;

  for (uint16_t i = 0; i < copySize; ++i)
  {
    m_bytes[i] = data[i];
  }
}
//-----------------------------------------------------------------------------------------------
//                                    class IntItem
//-----------------------------------------------------------------------------------------------
IntItem::IntItem(uint16_t leftX, uint16_t leftY
  , VgaFont *pFont, uint16_t width, const char *const fmtStr
  , uint16_t fontColor, uint16_t backColor, TftSpi &rTftSpi)
  : TextItem{leftX, leftY, pFont, width, fontColor, backColor, rTftSpi}
  , m_fmtStr{fmtStr}
{
}

void IntItem::Draw() const
{
  if (!m_changed)
  {
    return;
  }

  // std::cout << "Draw: " << std::endl;//<< m_bytes.get() << "; " << m_lastBytes.get() << std::endl;
  // for (size_t i = 0; i < m_bufSize; ++i)
  // {
  //   std::cout << m_lastBytes[i];
  // }
  // std::cout << std::endl;
  // for (size_t i = 0; i < m_bufSize; ++i)
  // {
  //   std::cout << m_bytes[i];
  // }
  // std::cout << std::endl;

  TextItem::Draw();

  m_changed = false;
}

void IntItem::SetValue(int value)
{
  if (m_value != value)
  {
    m_value = value;
    ::sprintf(m_bytes.get(), m_fmtStr, m_value);
    m_changed = true;
  }
}

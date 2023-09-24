#include "ui/TextItem.h"
#include "VgaFont.h"

namespace
{
static char gStaticBuffer[64] = {};
}
//-----------------------------------------------------------------------------------------------
//                                    class TextItem
//-----------------------------------------------------------------------------------------------
TextItem::TextItem(uint16_t leftX, uint16_t leftY, VgaFont *pFont, uint16_t bufSize, uint16_t fontColor, uint16_t backColor)
  : IUiItem{}
  , m_leftX{leftX}, m_leftY{leftY}
  , m_pFont{pFont}
  , m_bufSize{bufSize}
  , m_fontColor{fontColor}, m_backColor{backColor}
  , m_bytes{new char[bufSize]}, m_lastBytes{new char[bufSize]}
{
}

void TextItem::Draw() const
{
  uint16_t tmpLeft = m_leftX;
  const auto fontWidth = m_pFont->GetWidth();
  for (uint16_t i = 0; i < m_bufSize; ++i)
  {
    if (m_bytes[i] != m_lastBytes[i])
    {
      m_pFont->DisplayChar(m_bytes[i], tmpLeft, m_leftY, m_fontColor, m_backColor);
      m_lastBytes[i] = m_bytes[i];
    }
    tmpLeft += fontWidth;
  }
}

void TextItem::Update(const char *data, uint16_t bufSize)
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
  , uint16_t fontColor, uint16_t backColor)
  : IUiItem{}
  , m_leftX{leftX}, m_leftY{leftY}
  , m_pFont{pFont}
  , m_width{width}
  , m_fmtStr{fmtStr}
  , m_fontColor{fontColor}, m_backColor{backColor}
  , m_bytes{new char[m_width]{'\0'}}, m_lastBytes{new char[m_width]{'\0'}}
{
}

void IntItem::Draw() const
{
  if (!m_changed)
  {
    return;
  }

  uint16_t tmpLeft = m_leftX;
  const auto fontWidth = m_pFont->GetWidth();
  for (uint16_t i = 0; i < m_width; ++i)
  {
    if (m_bytes[i] != m_lastBytes[i])
    {
      m_pFont->DisplayChar(m_bytes[i], tmpLeft, m_leftY, m_fontColor, m_backColor);
      m_lastBytes[i] = m_bytes[i];
    }
    tmpLeft += fontWidth;
  }

  m_changed = false;
}

void IntItem::UpdateForce(int value)
{
  ::sprintf(gStaticBuffer, m_fmtStr, value);
  for (uint16_t i = 0; i < m_width; ++i)
  {
    if (gStaticBuffer[i] != '\0')
    {
      m_bytes[i] = gStaticBuffer[i];
    }
    else
    {
      m_bytes[i] = ' ';
    }
  }
}

void IntItem::Update(int value)
{
  if (m_value != value)
  {
    UpdateForce(value);

    m_value = value;
    m_changed = true;
  }
}

#include "ui/IUiItem.h"
// std
#include <memory>
#include <stdint.h>

class VgaFont;

class TextItem: public IUiItem
{
public:
  TextItem(uint16_t leftX, uint16_t leftY, VgaFont *pFont, uint16_t bufSize, uint16_t fontColor, uint16_t backColor, TftSpi &rTftSpi);

  void Draw() const override;
  void Update(const char *const data, uint16_t bufSize);

protected:
  uint16_t m_leftX;
  uint16_t m_leftY;
  VgaFont *m_pFont = nullptr;

  uint16_t m_bufSize = 0;

  uint16_t m_fontColor;
  uint16_t m_backColor;

  std::unique_ptr<char[]> m_bytes;
  std::unique_ptr<char[]> m_lastBytes;
};

class IntItem: public TextItem
{
public:
  IntItem(uint16_t leftX, uint16_t leftY, VgaFont *pFont, uint16_t width, const char *const fmtStr
    , uint16_t fontColor, uint16_t backColor, TftSpi &rTftSpi);

  void Draw() const override;
  void SetValue(int value);

private:
  VgaFont *m_pFont = nullptr;
  const char *const m_fmtStr = nullptr;

  // todo!!! fix me
  int m_value = 0xffff;
  mutable bool m_changed = true;
};

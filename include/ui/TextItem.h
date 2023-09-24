#include "ui/IUiItem.h"
// std
#include <memory>
#include <stdint.h>

class VgaFont;

class TextItem: public IUiItem
{
public:
  TextItem(uint16_t leftX, uint16_t leftY, VgaFont *pFont, uint16_t bufSize, uint16_t fontColor, uint16_t backColor);

  void Draw() const override;
  void Update(const char *data, uint16_t bufSize);

private:
  uint16_t m_leftX;
  uint16_t m_leftY;
  VgaFont *m_pFont = nullptr;

  uint16_t m_bufSize = 0;

  uint16_t m_fontColor;
  uint16_t m_backColor;

  std::unique_ptr<char[]> m_bytes;
  std::unique_ptr<char[]> m_lastBytes;
};

class IntItem: public IUiItem
{
public:
  IntItem(uint16_t leftX, uint16_t leftY, VgaFont *pFont, uint16_t width, const char *const fmtStr
    , uint16_t fontColor, uint16_t backColor);

  void Draw() const override;
  void Update(int value);
  void UpdateForce(int value);

private:
  uint16_t m_leftX;
  uint16_t m_leftY;
  VgaFont *m_pFont = nullptr;
  uint16_t m_width;
  const char *const m_fmtStr = nullptr;

  uint16_t m_fontColor;
  uint16_t m_backColor;

  int m_value = 0;
  std::unique_ptr<char[]> m_bytes;
  std::unique_ptr<char[]> m_lastBytes;
  mutable bool m_changed = true;
};

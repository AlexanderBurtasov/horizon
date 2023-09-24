#pragma once

class IUiItem
{
public:
  virtual ~IUiItem()
  {}

  virtual void Draw() const = 0;
};

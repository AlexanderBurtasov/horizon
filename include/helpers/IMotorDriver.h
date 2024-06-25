#pragma once

#include <cstdint>

class IMotorDriver
{
public:
  IMotorDriver(uint8_t span): m_minLevel{static_cast<int8_t>((-1) * span)}, m_maxLevel{static_cast<int8_t>(span)}
  {}

  virtual ~IMotorDriver() {}
  virtual void WriteLevel(int8_t level) = 0;

  IMotorDriver(const IMotorDriver &) = delete;
  IMotorDriver &operator = (const IMotorDriver &) = delete;
  IMotorDriver(IMotorDriver &&) = delete;
  IMotorDriver &operator = (IMotorDriver &&) = delete;

protected:
  int8_t m_minLevel;
  int8_t m_maxLevel;
};

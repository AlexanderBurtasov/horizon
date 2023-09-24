#pragma once

#include <stdint.h>
// fwd
class I2cMasterHelper;

class I2cJoystick
{
public:
  I2cJoystick(int8_t deviceAddress, I2cMasterHelper &i2cMaster, uint8_t maxRange);
  uint8_t Update();
  
  int8_t GetX() const;
  int8_t GetY() const;
  
private:
  int8_t m_deviceAddress;
  I2cMasterHelper &m_rI2cMaster;

  int8_t m_x = 0;
  int8_t m_y = 0;
  uint8_t m_maxRange;

  int8_t m_buf[2];
};

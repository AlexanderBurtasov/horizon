#pragma once

#include <stdint.h>
#include <stddef.h>

class I2cMasterHelper
{
public:
  enum class I2cPort: int
  {
    cNumber_0 = 0
    , cNumber_1
  };

  I2cMasterHelper(I2cPort portNum, int sdaNum, int sclNum, uint32_t clockSpeed);
  bool ReadBytes(int deviceAddress, int regAddress, uint8_t *dest, size_t dataLength);

private:
  I2cPort m_portNum;

private:
  // syntax sugar: forbit copy / move operations
  I2cMasterHelper(const I2cMasterHelper &) = delete;
  I2cMasterHelper &operator = (const I2cMasterHelper &) = delete;
  I2cMasterHelper(I2cMasterHelper &&) = delete;
  I2cMasterHelper &operator = (I2cMasterHelper &&) = delete;
};

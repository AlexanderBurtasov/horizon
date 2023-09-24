#include "I2cJoystick.h"
// prj
#include "I2cMasterHelper.h"


auto convertValue = [](int8_t value, uint8_t maxRange)
{
  const int cMaxValue = 100;
  return static_cast<int16_t>(value) * maxRange / cMaxValue;
};

I2cJoystick::I2cJoystick(int8_t deviceAddress, I2cMasterHelper &i2cMaster, uint8_t maxRange)
  : m_deviceAddress{deviceAddress}
  , m_rI2cMaster(i2cMaster)
  , m_maxRange{maxRange}
{}

uint8_t I2cJoystick::Update()
{
  m_rI2cMaster.ReadBytes(m_deviceAddress, 0x12, reinterpret_cast<uint8_t *>(m_buf), 1);
  m_rI2cMaster.ReadBytes(m_deviceAddress, 0x13, reinterpret_cast<uint8_t *>(m_buf+1), 1);

  uint8_t result = 0;
  if (m_buf[0] != m_x)
  {
    m_x = m_buf[0];
    result |= 0x1;
  }
  if (m_buf[1] != m_y)
  {
    m_y = m_buf[1];
    result |= 0x1 << 1;
  }

  return result;
}
  
int8_t I2cJoystick::GetX() const
{
  return convertValue(m_x, m_maxRange);
}

int8_t I2cJoystick::GetY() const
{
  return convertValue(m_y, m_maxRange);
}

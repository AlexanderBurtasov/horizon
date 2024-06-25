#pragma once
// std
#include <tuple>
// esp
#include "driver/adc.h"
#include "driver/gpio.h"
// prj
#include "Axis.h"

class AdcJoystick
{
public:
  using AxisDef = std::tuple<gpio_num_t, int, int, int, uint16_t>;
  using Result = std::tuple<int, int>;

public:
  AdcJoystick(const AxisDef &axis0, const AxisDef &axis1);
  Result ReadValues() const;

private:
  Axis m_axis0;
  Axis m_axis1;

  adc1_channel_t m_channel0;
  adc1_channel_t m_channel1;
};

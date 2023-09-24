#pragma once
// espidf
#include "driver/adc.h"

#include "Axis.h"

class AdcAxis
{
public:
  AdcAxis(gpio_num_t pinNum, int lower, int middle, int upper, uint16_t maxValue);
  int GetValue() const;
  
protected:
  Axis m_axis;
  adc1_channel_t m_chanNum;
};

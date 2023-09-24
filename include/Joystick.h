#pragma once

#include "Axis.h"

class Joystick
{
public:
  Joystick();
  std::pair<int, int> Update();  

private:
  Axis m_xAxis;
  Axis m_yAxis;
};

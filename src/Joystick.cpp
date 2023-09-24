#include "Joystick.h"
// std
#include <math.h>
// driver
#include "driver/adc.h"

using namespace std;

Joystick::Joystick()
  : m_xAxis{ 200, 1850, 4040, 10 }
  , m_yAxis{ 4000, 2050, 200, 35 }

{
  gpio_num_t aPin, bPin;

  auto r = adc2_pad_get_io_num(ADC2_CHANNEL_5, &aPin); // gpio_12
  assert(r == ESP_OK);

  r = adc2_config_channel_atten(ADC2_CHANNEL_5, ADC_ATTEN_DB_11);
  assert(r == ESP_OK);

  r = adc2_pad_get_io_num(ADC2_CHANNEL_6, &bPin);      // gpio 14
  assert(r == ESP_OK);

  r = adc2_config_channel_atten(ADC2_CHANNEL_6, ADC_ATTEN_DB_11);
  assert(r == ESP_OK);                                                                                                                                                                                                                                                                                                                                                                                                                               

  //m_yLevels = BuildThresholdsArray(200, 1850, 4096, 5);
  //m_xLevels = BuildThresholdsArray(100, 2050, 4096, 5);
}

pair<int, int> Joystick::Update()
{
  int vX, vY;
  ::adc2_get_raw(ADC2_CHANNEL_5, ADC_WIDTH_BIT_12, &vX);
  ::adc2_get_raw(ADC2_CHANNEL_6, ADC_WIDTH_BIT_12, &vY);

  return { m_xAxis.getValue(vX), m_yAxis.getValue(vY) };
}

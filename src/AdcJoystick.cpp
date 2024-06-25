#include "helpers/AdcJoystick.h"
// espidf
#include "driver/adc.h"

using std::map;

namespace
{
adc1_channel_t GetChannelNum(gpio_num_t pinNum);
}

AdcJoystick::AdcJoystick(const AdcJoystick::AxisDef &axis0, const AdcJoystick::AxisDef &axis1)
  : m_axis0{std::get<1>(axis0), std::get<2>(axis0), std::get<3>(axis0), std::get<4>(axis0)}
  , m_axis1{std::get<1>(axis1), std::get<2>(axis1), std::get<3>(axis1), std::get<4>(axis1)}
{
  m_channel0 = GetChannelNum(std::get<0>(axis0));
  m_channel1 = GetChannelNum(std::get<0>(axis1));

  ::adc1_config_channel_atten(m_channel0, ADC_ATTEN_DB_11);
  ::adc1_config_channel_atten(m_channel1, ADC_ATTEN_DB_11);
}

AdcJoystick::Result AdcJoystick::ReadValues() const
{
  const int value0 = ::adc1_get_raw(m_channel0);
  const int value1 = ::adc1_get_raw(m_channel1);
  
  return {m_axis0.getValue(value0), m_axis1.getValue(value1)};
}

namespace
{
adc1_channel_t GetChannelNum(gpio_num_t pinNum)
{
  static const map<gpio_num_t, adc1_channel_t> gConvMap =
  {
    { GPIO_NUM_36, ADC1_CHANNEL_0 }, /*!< ADC1 channel 0 is GPIO36 */
    { GPIO_NUM_37, ADC1_CHANNEL_1 }, /*!< ADC1 channel 1 is GPIO37 */
    { GPIO_NUM_38, ADC1_CHANNEL_2 }, /*!< ADC1 channel 2 is GPIO38 */
    { GPIO_NUM_39, ADC1_CHANNEL_3 }, /*!< ADC1 channel 3 is GPIO39 */
    { GPIO_NUM_32, ADC1_CHANNEL_4 }, /*!< ADC1 channel 4 is GPIO32 */
    { GPIO_NUM_33, ADC1_CHANNEL_5 }, /*!< ADC1 channel 5 is GPIO33 */
    { GPIO_NUM_34, ADC1_CHANNEL_6 }, /*!< ADC1 channel 6 is GPIO34 */
    { GPIO_NUM_35, ADC1_CHANNEL_7 }  /*!< ADC1 channel 7 is GPIO35 */
  };

  const auto it = gConvMap.find(pinNum);
  if (gConvMap.end() == it)
  {
    return ADC1_CHANNEL_MAX;
  }

  return it->second;
}
}

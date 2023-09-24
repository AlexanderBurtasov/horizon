#include "AdcAxis.h"

using std::map;

namespace
{
adc1_channel_t GetChannelNum(gpio_num_t pinNum)
{
    static const map<gpio_num_t, adc1_channel_t> gConvMap =
    {
        { GPIO_NUM_36, ADC1_CHANNEL_0 }, /*!< ADC1 channel 0 is GPIO36 */
        { GPIO_NUM_37, ADC1_CHANNEL_1 },     /*!< ADC1 channel 1 is GPIO37 */
        { GPIO_NUM_38, ADC1_CHANNEL_2 },     /*!< ADC1 channel 2 is GPIO38 */
        { GPIO_NUM_39, ADC1_CHANNEL_3 },     /*!< ADC1 channel 3 is GPIO39 */
        { GPIO_NUM_32, ADC1_CHANNEL_4 },     /*!< ADC1 channel 4 is GPIO32 */
        { GPIO_NUM_33, ADC1_CHANNEL_5 },     /*!< ADC1 channel 5 is GPIO33 */
        { GPIO_NUM_34, ADC1_CHANNEL_6 },     /*!< ADC1 channel 6 is GPIO34 */
        { GPIO_NUM_35, ADC1_CHANNEL_7 }     /*!< ADC1 channel 7 is GPIO35 */
    };

    const auto it = gConvMap.find(pinNum);
    if (gConvMap.end() == it)
    {
        return ADC1_CHANNEL_MAX;
    }

    return it->second;
}
}

AdcAxis::AdcAxis(gpio_num_t pinNum, int lower, int middle, int upper, uint16_t maxValue)
    : m_axis{lower, middle, upper, maxValue}
{
    m_chanNum = GetChannelNum(pinNum);
    adc1_config_channel_atten(m_chanNum, ADC_ATTEN_DB_11);
}


int AdcAxis::GetValue() const
{
    const int value = ::adc1_get_raw(m_chanNum);
    return m_axis.getValue(value);
}

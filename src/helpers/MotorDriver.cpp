#include "helpers/MotorDriver.h"
// std
#include <iostream>
#include <thread>
// esp32
#include "rom/gpio.h"
#include "driver/ledc.h"
#include "driver/pcnt.h"
#include "soc/ledc_struct.h"

namespace
{
template <typename T>
T GetAbs(T value)
{
  if (value < 1)
  {
    return -value;
  }

  return value;
}

template <typename T>
bool IsPositive(T value)
{
  return value >= 0;
}

template <typename T>
bool ZeroCrossing(T from, T to)
{
  if (0 == to)
  {
    return false;
  }
  if (0 == from)
  {
    return true;
  }

  return IsPositive(from) != IsPositive(to);
}
} // namespace
//-----------------------------------------------------------------------------------------------
//                                class MotorDriver50Hz
//-----------------------------------------------------------------------------------------------
MotorDriver50Hz::MotorDriver50Hz(gpio_num_t pinNum, ledc_channel_t channel, uint8_t span)
  : Servo{pinNum, channel}
  , IMotorDriver{span}
  , m_currentLevel(static_cast<int8_t>(-1 * span) -1)
{
  // arm
  //WriteLevel(m_minValue);
  Servo::Write(0.f);
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  //WriteLevel(m_maxValue);

  Servo::Write(180.f);
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
}

void MotorDriver50Hz::WriteLevel(int8_t level)
{
  if (level == m_currentLevel)
  {
    return;
  }

  if (ZeroCrossing(m_currentLevel, level))
  {
    std::cout << "zero crossing from: " << static_cast<int>(m_currentLevel) << " to: " << static_cast<int>(level) << std::endl;
    OnZeroCrossing(level);
  }

  float angleToWrite = 0;
  if (level < m_minLevel)
  {
    angleToWrite = 0;
  }
  else if (level > m_maxLevel)
  {
    angleToWrite = 180;
  }
  if (level < 0)
  {
    angleToWrite = 90.f - static_cast<float>(level) * 90.f / m_minLevel;
  }
  else
  {
    angleToWrite = 90.f + static_cast<float>(level) * 90.f / m_maxLevel;
  }

  if (GetAbs(m_lastAngle - angleToWrite) > 1.f)
  {
    std::cout << "write angle: " << angleToWrite << std::endl;
    Servo::Write(angleToWrite);
    m_lastAngle = angleToWrite;
    m_currentLevel = level;
  }
}

void MotorDriver50Hz::OnZeroCrossing(int levelNew)
{
  // do nothing
}
//-----------------------------------------------------------------------------------------------
//                            class MotorDriver50HzZeroBreak
//-----------------------------------------------------------------------------------------------
MotorDriver50HzZeroBreak::MotorDriver50HzZeroBreak(gpio_num_t pinNum, ledc_channel_t channel, uint8_t span)
  : MotorDriver50Hz{pinNum, channel, span}
{
}

void MotorDriver50HzZeroBreak::OnZeroCrossing(int levelNew)
{
  Servo::Write(90);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  Servo::Write(80);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  Servo::Write(90);
  /*
  if (levelNew < 0)
  {
    Servo::Write(60);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }
  else
  {
    Servo::Write(110);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }*/
}
//-----------------------------------------------------------------------------------------------
//                            class MotorDriver50HzZeroBreak
//-----------------------------------------------------------------------------------------------
namespace
{
const ledc_channel_t cLedcChannelDefault = LEDC_CHANNEL_0;
const ledc_mode_t cSpeedModeDefault = LEDC_HIGH_SPEED_MODE;
const ledc_timer_t cTimerIndexDefault = LEDC_TIMER_0;

//#define SERVO_LEDC_INIT_BITS LEDC_TIMER_15_BIT
//#define SERVO_LEDC_INIT_FREQ (50)
ledc_timer_bit_t cTimerBitsDefault = ledc_timer_bit_t::LEDC_TIMER_10_BIT;
const int cInitFreq = 50;
}
//-----------------------------------------------------------------------------------------------
MotorDriverPwm::MotorDriverPwm(gpio_num_t pinANum, gpio_num_t pinBNum, ledc_channel_t channelA, ledc_channel_t channelB, uint8_t span)
  : IMotorDriver{span}
{
  m_channelA = channelA;
  m_mode = cSpeedModeDefault;
  m_timer = cTimerIndexDefault;
  {
//    const int fullDuty = (1 << cTimerBitsDefault) - 1;

    ledc_timer_config_t ledc_timer
    {
      .speed_mode = m_mode              // timer mode
      , .duty_resolution = cTimerBitsDefault // resolution of PWM duty
      , .timer_num = m_timer            // timer index
      , .freq_hz = cInitFreq            // frequency of PWM signal
      , .clk_cfg = LEDC_AUTO_CLK
    };

    {
      const auto result = ::ledc_timer_config(&ledc_timer);
      std::cout << "ledc_timer_config A: " << result << std::endl;
    }

    ledc_channel_config_t ledcChannel
    {
        .gpio_num = pinANum
        , .speed_mode = m_mode
        , .channel = m_channelA
        , .intr_type = LEDC_INTR_DISABLE
        , .timer_sel = m_timer
        , .duty = 0
    };
    {
      const auto result = ::ledc_channel_config(&ledcChannel);
      std::cout << "ledc_channel_config A: " << result << std::endl;
    }
  }

  {
    m_channelB = channelB;
    m_mode = cSpeedModeDefault;
    m_timer = cTimerIndexDefault;
//    const int fullDuty = (1 << cTimerBitsDefault) - 1;
/*
    ledc_timer_config_t ledc_timer
    {
      .speed_mode = m_mode              // timer mode
      , .duty_resolution = cTimerBitsDefault // resolution of PWM duty
      , .timer_num = m_timer            // timer index
      , .freq_hz = cInitFreq            // frequency of PWM signal
      , .clk_cfg = LEDC_AUTO_CLK
    };

    {
      const auto result = ::ledc_timer_config(&ledc_timer);
      std::cout << "ledc_timer_config: " << result << std::endl;
    }
*/
    ledc_channel_config_t ledcChannel
    {
        .gpio_num = pinBNum
        , .speed_mode = m_mode
        , .channel = m_channelB
        , .intr_type = LEDC_INTR_DISABLE
        , .timer_sel = m_timer
        , .duty = 0
    };
    {
      const auto result = ::ledc_channel_config(&ledcChannel);
      std::cout << "ledc_channel_config B: " << result << std::endl;
    }
  }

  // ::gpio_reset_pin(pinEnaNum);
  // ::gpio_set_direction(pinEnaNum, GPIO_MODE_OUTPUT);
  // ::gpio_set_level(pinEnaNum, 1);  
}

void MotorDriverPwm::WriteLevel(int8_t level)
{
  ::ledc_set_duty(m_mode, m_channelA, 255);
  ::ledc_update_duty(m_mode, m_channelA);

  ::ledc_set_duty(m_mode, m_channelB, 0);
  ::ledc_update_duty(m_mode, m_channelA);
}
//-----------------------------------------------------------------------------------------------
//                            class MotorDriver50HzZeroBreak
//-----------------------------------------------------------------------------------------------
MotorDriverLn298N::MotorDriverLn298N(gpio_num_t pinEnaNum, ledc_channel_t channelA
  , gpio_num_t pinIn1, gpio_num_t pinIn2
  , uint8_t span)
  : IMotorDriver{span}
  , m_pinIn1(pinIn1)
  , m_pinIn2(pinIn2)
{
  m_channelEna = channelA;
  m_timer = cTimerIndexDefault;
  {
    m_fullDuty = (1 << cTimerBitsDefault) - 1;

    ledc_timer_config_t ledc_timer
    {
      .speed_mode = cSpeedModeDefault         // timer mode
      , .duty_resolution = cTimerBitsDefault // resolution of PWM duty
      , .timer_num = m_timer            // timer index
      , .freq_hz = 100                  // frequency of PWM signal
      , .clk_cfg = LEDC_AUTO_CLK
    };

    {
      const auto result = ::ledc_timer_config(&ledc_timer);
      std::cout << "ledc_timer_config Ena: " << result << std::endl;
    }

    ledc_channel_config_t ledcChannel
    {
        .gpio_num = pinEnaNum
        , .speed_mode = cSpeedModeDefault
        , .channel = m_channelEna
        , .intr_type = LEDC_INTR_DISABLE
        , .timer_sel = m_timer
        , .duty = 0
        , .hpoint = 0
        , .flags = 0
    };

    {
      const auto result = ::ledc_channel_config(&ledcChannel);
      std::cout << "ledc_channel_config Ena: " << result << std::endl;
    }
  }

  ::gpio_reset_pin(m_pinIn1);
  ::gpio_set_direction(m_pinIn1, GPIO_MODE_OUTPUT);
  ::gpio_set_level(m_pinIn1, 0);

  ::gpio_reset_pin(m_pinIn2);
  ::gpio_set_direction(m_pinIn2, GPIO_MODE_OUTPUT);
  ::gpio_set_level(m_pinIn2, 0);
}

void MotorDriverLn298N::WriteLevel(int8_t level)
{
  const uint32_t duty = ::abs(level) * m_fullDuty / m_maxLevel;
  const uint32_t levelIn1 = level <= 0 ? 0 : 1;
  const uint32_t levelIn2 = level >= 0 ? 0 : 1;
  
  ::gpio_set_level(m_pinIn1, levelIn1);
  ::gpio_set_level(m_pinIn2, levelIn2);

  ::ledc_set_duty(cSpeedModeDefault, m_channelEna, duty);
  ::ledc_update_duty(cSpeedModeDefault, m_channelEna);

  std::cout << "level: " << static_cast<int16_t>(level)
    << "; duty: " << duty << "; in1: " << levelIn1 << "; in2: " << levelIn2 << std::endl;
}

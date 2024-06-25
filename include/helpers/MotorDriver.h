#include "helpers/IMotorDriver.h"
#include "helpers/Servo.h"

class MotorDriver50Hz: protected Servo, public IMotorDriver
{
public:
  MotorDriver50Hz(gpio_num_t pinNum, ledc_channel_t channel, uint8_t span);
  void WriteLevel(int8_t level) override;

protected:
  virtual void OnZeroCrossing(int levelNew);

protected:
  float m_lastAngle = -100.f;
  int8_t m_currentLevel = 0;
};

class MotorDriver50HzZeroBreak: public MotorDriver50Hz
{
public:
  MotorDriver50HzZeroBreak(gpio_num_t pinNum, ledc_channel_t channel, uint8_t span);

protected:
  void OnZeroCrossing(int levelNew) override;
};

class MotorDriverPwm: public IMotorDriver
{
public:
  MotorDriverPwm(gpio_num_t pinANum, gpio_num_t pinBNum, ledc_channel_t channelA, ledc_channel_t channelB, uint8_t span);
  void WriteLevel(int8_t level) override;

protected:
  ledc_channel_t m_channelA;
  ledc_channel_t m_channelB;
  ledc_mode_t m_mode;
  ledc_timer_t m_timer;
};

class MotorDriverLn298N: public IMotorDriver
{
public:
  MotorDriverLn298N(gpio_num_t pinEnaNum, ledc_channel_t channelA, gpio_num_t pinIn1, gpio_num_t pinIn2
    , uint8_t span);

  void WriteLevel(int8_t level) override;

protected:
  ledc_channel_t m_channelEna;
  ledc_timer_t m_timer;

  gpio_num_t m_pinIn1;
  gpio_num_t m_pinIn2;

  uint32_t m_fullDuty = 1;
};

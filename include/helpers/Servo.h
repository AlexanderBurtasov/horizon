#pragma once

#include "esp_err.h"
#include "esp_log.h"
#include "driver/ledc.h"

class Servo
{
private:
  ledc_channel_t m_channel;
  ledc_mode_t m_mode;
  ledc_timer_t m_timer;
  int m_fullDuty;
  int m_freq;
  float m_angle;
  uint32_t m_maxAngle;
  uint32_t m_minUs;
  uint32_t m_maxUs;

  /**
   * prevent copy constructing
   */
  Servo(const Servo &) = delete;
  Servo(Servo &&) = delete;
  Servo &operator = (const Servo&) = delete;
  Servo &operator = (Servo &&) = delete;
public:
  /**
  * @brief Constructor for CServo class
  * @param servoIo GPIO incdex for servo signal
  * @param maxAngle Max angle of servo motor, positive value.
  * @param minWidthUs Minimum pulse width, corresponding to 0 position.
  * @param maxwidthUs Maximum pulse width, corresponding to max_angle position.
  * @param channel LEDC hardware channel
  * @param speedMode Speed mode for LEDC hardware
  * @param timerIdx Timer index for LEDC hardware
  */
  Servo(gpio_num_t pinNum, ledc_channel_t channel);
  Servo(gpio_num_t pinNum, ledc_channel_t channel, int maxAngle, int minWidthUs, int maxWidthUs
    , ledc_mode_t speedMode, ledc_timer_t timerIdx);

  void Write(float angle);
};

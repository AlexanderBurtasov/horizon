#include "helpers/Servo.h"

#include <iostream>

#include <esp_types.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_err.h"
#include "rom/gpio.h"
#include "driver/ledc.h"
#include "driver/pcnt.h"
#include "soc/ledc_struct.h"

static const char* SERVO_TAG = "servo";
#define SERVO_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(SERVO_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }

#define SERVO_LEDC_INIT_BITS LEDC_TIMER_15_BIT
#define SERVO_LEDC_INIT_FREQ (50)
#define SERVO_DUTY_MIN_US    (500)
#define SERVO_DUTY_MAX_US    (2500)
#define SERVO_SEC_TO_US      (1000000)

const int cMaxAngleDefault = 180;
const int cMinWidthUsDefault = 500;
const int cMaxWidthUsDefault = 2500;
const ledc_channel_t cLedcChannelDefault = LEDC_CHANNEL_0;
const ledc_mode_t cSpeedModeDefault = LEDC_HIGH_SPEED_MODE;
const ledc_timer_t cTimerIndexDefault = LEDC_TIMER_0;

Servo::Servo(gpio_num_t pinNum, ledc_channel_t channel)
  : Servo{pinNum, channel, cMaxAngleDefault, cMinWidthUsDefault, cMaxWidthUsDefault, cSpeedModeDefault, cTimerIndexDefault}
{
}

Servo::Servo(gpio_num_t pinNum, ledc_channel_t channel, int maxAngle, int minWidthUs, int maxWidthUs, ledc_mode_t speedMode, ledc_timer_t timerIndex)
{
  m_channel = channel;
  m_mode = speedMode;
  m_timer = timerIndex;
  m_fullDuty = (1 << SERVO_LEDC_INIT_BITS) - 1;
  m_freq = SERVO_LEDC_INIT_FREQ;
  m_angle = 0;
  m_maxAngle = maxAngle;
  m_minUs = minWidthUs;
  m_maxUs = maxWidthUs;

  ledc_timer_config_t ledc_timer
  {
    .speed_mode = m_mode              // timer mode
    , .duty_resolution = SERVO_LEDC_INIT_BITS // resolution of PWM duty
    , .timer_num = m_timer            // timer index
    , .freq_hz = SERVO_LEDC_INIT_FREQ // frequency of PWM signal
    , .clk_cfg = LEDC_AUTO_CLK
  };

  {
    const auto result = ::ledc_timer_config(&ledc_timer);
    if (ESP_OK != result)
    {
      std::cout << "ledc_timer_config failed. error: " << result << std::endl;
    }
  }

  ledc_channel_config_t ledcChannel
  {
      .gpio_num = pinNum
      , .speed_mode = m_mode
      , .channel = m_channel
      , .intr_type = LEDC_INTR_DISABLE
      , .timer_sel = m_timer
      , .duty = 0
      , .hpoint = 0
  };
  {
    const auto result = ::ledc_channel_config(&ledcChannel);
    if (ESP_OK != result)
    {
      std::cout << "ledc_channel_config failed. error: " << result << std::endl;
    }
  }
}

void Servo::Write(float angle)
{
  m_angle = angle;
  const float angleUs = angle / m_maxAngle * (m_maxUs - m_minUs) + m_minUs;
  const uint32_t duty = m_fullDuty * ((int)angleUs) / (SERVO_SEC_TO_US / m_freq);

  ::ledc_set_duty(m_mode, m_channel, duty);
  ::ledc_update_duty(m_mode, m_channel);
}

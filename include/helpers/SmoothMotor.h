#pragma once
// std
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
// prj
#include "helpers/IMotorDriver.h"
#include "helpers/Servo.h"

class SmoothMotor: public IMotorDriver, protected Servo
{
public:
  SmoothMotor(gpio_num_t pinNum, ledc_channel_t channel, uint8_t span);
  ~SmoothMotor();
  void WriteLevel(int8_t level) override;

//protected:
  void Arm();
  float LevelToAngle(int8_t level) const;

private:
  std::thread m_thread;
  std::atomic<bool> m_isFinished = false;
  std::mutex m_mutex;

  int8_t m_currentLevel = 0;
  int8_t m_targetLevel = 0;
  int8_t m_multiplier = 1;

  std::chrono::system_clock::time_point m_lastPoint;
};

#include "helpers/SmoothMotor.h"
// std
#include <iostream>
#include <tuple>
#include <vector>

namespace ch = std::chrono;
namespace th = std::this_thread;

using std::lock_guard;
using std::tuple;

namespace
{
template<typename T>
bool HasTransition(T from, T to)
{
  return from >= 0 && to < 0;
  // if (from == to)
  // {
  //   return false;
  // }

  // if (0 == true)
  // {
  //   return true;
  // }

  // return from * to < 0;
}

tuple<ch::system_clock::time_point, int8_t> CalculateNextStep(int8_t currentValue, uint8_t speedMax, int8_t multipiler, ch::system_clock::time_point lastTick);
}

SmoothMotor::SmoothMotor(gpio_num_t pinNum, ledc_channel_t channel, uint8_t span)
  : IMotorDriver{span}
  , Servo{pinNum, channel}
{
  auto runFunc = [this]() -> void
  {
    while (!m_isFinished.load())
    {
      {
        lock_guard lock{m_mutex};
        if (m_currentLevel != m_targetLevel)
        {
          const auto [currentTick, calculatedLevel] = CalculateNextStep(m_currentLevel, 60, m_multiplier, m_lastPoint);
          int valueToWrite = 0;
          if (m_multiplier > 0)
          {
            valueToWrite = calculatedLevel <= m_targetLevel ? calculatedLevel : m_targetLevel;
          }
          else
          {
            valueToWrite = calculatedLevel > m_targetLevel ? calculatedLevel : m_targetLevel;
          }

          const auto angleToWrite = LevelToAngle(valueToWrite);
          std::cout << "value: " << valueToWrite << "; angle: " << angleToWrite << std::endl;
          Servo::Write(angleToWrite);

          m_currentLevel = static_cast<int8_t>(valueToWrite);
          m_lastPoint = currentTick;
        }
      }

      th::sleep_for(ch::milliseconds(100));
    }
  };

//  Arm();

  m_lastPoint = ch::system_clock::now();
  m_thread = std::thread{runFunc};
}

SmoothMotor::~SmoothMotor()
{
  m_isFinished.store(true);
  m_thread.join();
}

void SmoothMotor::Arm()
{
  const std::vector<float> cArmingSeq =
  {
    90, 45, 135, 90
  };

  for (auto it : cArmingSeq)
  {
    Servo::Write(it);
    th::sleep_for(ch::milliseconds(200));
  }
  // int angle = 90;
  // for (; angle >= 0; angle -= 10)
  // {
  //   std::cout << "angle: " << angle << std::endl;
  //   Servo::Write(angle);
  //   th::sleep_for(ch::milliseconds(50));
  // }
  // std::cout << "arm0 done" << std::endl;

  // for (angle = 0; angle <= 180; angle += 10)
  // {
  //   std::cout << "angle: " << angle << std::endl;
  //   Servo::Write(angle);
  //   th::sleep_for(ch::milliseconds(50));
  // }
  // std::cout << "arm1 done" << std::endl;

  // for (angle = 180; angle <= 90; angle -= 10)
  // {
  //   std::cout << "angle: " << angle << std::endl;
  //   Servo::Write(angle);
  //   th::sleep_for(ch::milliseconds(50));
  // }

  // std::cout << "arm done" << std::endl;
}

void SmoothMotor::WriteLevel(int8_t level)
{
  lock_guard lock{m_mutex};
  if (level == m_targetLevel)
  {
    return;
  }
  std::cout << std::endl << "new level: " << static_cast<int16_t>(level) 
    << "; target level: " << static_cast<int16_t>(m_targetLevel)
    << "; current level: " << static_cast<int16_t>(m_currentLevel)
    << std::endl;

  m_targetLevel = level;

  if (HasTransition(m_currentLevel, level))
  {
    std::cout << "Zero" << std::endl;
    Servo::Write(90);
    th::sleep_for(ch::milliseconds(100));
    Servo::Write(85);
    th::sleep_for(ch::milliseconds(100));
    Servo::Write(90);
    th::sleep_for(ch::milliseconds(100));

    m_currentLevel = -1;
    const auto angleToWrite = LevelToAngle(m_currentLevel);
    Servo::Write(angleToWrite);

    m_multiplier = m_targetLevel > m_currentLevel ? 1 : -1;
    m_lastPoint = ch::system_clock::now();

    return;
  }

  if (m_currentLevel > 0 && level >= 0)
  {
    if (level < m_currentLevel)
    {
      m_currentLevel = level;
      const auto angleToWrite = LevelToAngle(m_currentLevel);
      std::cout << "90  <- 180: " << static_cast<int16_t>(m_currentLevel)
        << "; angle: " << static_cast<uint16_t>(angleToWrite) << std::endl;
      m_lastPoint = ch::system_clock::now();
      Servo::Write(angleToWrite);
      return;
    }
  }
  else if (m_currentLevel < 0 && level <= 0)
  {
    if (level > m_currentLevel)
    {
      m_currentLevel = level;
      const auto angleToWrite = LevelToAngle(m_currentLevel);
      std::cout << "0  ->  90: " << static_cast<int16_t>(m_currentLevel)
        << "; angle: " << static_cast<uint16_t>(angleToWrite) << std::endl;
      m_lastPoint = ch::system_clock::now();
      Servo::Write(angleToWrite);
      return;
    }
  }
  else // force smooth increase
  {
    m_multiplier = m_targetLevel > m_currentLevel ? 1 : -1;
    m_lastPoint = ch::system_clock::now();
  }
}

float SmoothMotor::LevelToAngle(int8_t level) const
{
  return level * 4.5f + 90.f;
}

namespace
{

tuple<ch::system_clock::time_point, int8_t> CalculateNextStep(int8_t currentValue, uint8_t speedMax, int8_t multipiler, ch::system_clock::time_point lastTick)
{
  const auto currentTick = ch::system_clock::now();
  const auto timeDelta = ch::duration_cast<std::chrono::milliseconds>(currentTick - lastTick).count();
  const auto delta = speedMax * timeDelta / 1000.f;
  return {currentTick, static_cast<int8_t>(currentValue + delta * multipiler)};
}

}

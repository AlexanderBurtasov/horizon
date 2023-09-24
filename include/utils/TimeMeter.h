#pragma once

#include <chrono>
#include <iostream>

class TimeMeter
{
public:
  TimeMeter(const char *const title): m_title{title}, m_startPoint{std::chrono::system_clock::now()}
  {}

  ~TimeMeter()
  {
      const auto endPoint = std::chrono::system_clock::now();
      std::cout << m_title << std::chrono::duration_cast<std::chrono::milliseconds>(endPoint - m_startPoint).count()
          << std::endl;
  }

private:
  std::string m_title;
  std::chrono::system_clock::time_point m_startPoint;
};

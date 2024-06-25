#include "helpers/Servo.h"
// std
#include <thread>

namespace ch = std::chrono;
///namespace th = std::thread;

void DoServoTest()
{
  Servo servo(GPIO_NUM_17, LEDC_CHANNEL_0);
  servo.Write(120);

  for (;;)
  {
    std::this_thread::sleep_for(ch::milliseconds(1000));
  }
}

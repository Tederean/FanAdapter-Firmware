#include <Arduino.h>
#include <application/common/FanRegulator.h>
#include <application/common/FastPID.h>

using namespace std;

namespace FanRegulator
{

  void Initialize(float tickFrequency);

  uint8_t Step(uint8_t targetPower, uint8_t currentPower);

  FastPID PID(0.0f, 0.1f, 0.0f, TickFrequency, 8);

  void Initialize(float tickFrequency)
  {
  }

  uint8_t Step(uint8_t targetPower, uint8_t currentPower)
  {
    return targetPower;
  }

} // namespace FanRegulator
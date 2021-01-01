#ifndef _FanRegulator_
#define _FanRegulator_

#include <Arduino.h>

using namespace std;

namespace FanRegulator
{

  void Initialize(float tickFrequency);

  uint8_t Step(uint8_t targetPower, uint8_t currentPower);

} // namespace FanRegulator

#endif
#ifndef _TickService_
#define _TickService_

#include <Arduino.h>

using namespace std;

namespace Services
{
  namespace Tick
  {

    void Initialize();

  } // namespace Tick
} // namespace Services

#endif


//#include <FastPID.h>

//uint16_t PowerToRPM(uint8_t power);

//const float TickFrequency = 1000000.0f / TickInterval;

//FastPID PID(0.4f, 0.4f, 0.05f, TickFrequency, 8);

//uint16_t PowerToRPM(uint8_t power)
//{
//  float ratio = power / 255.0f;
//
//  return ratio * 1500;
//}
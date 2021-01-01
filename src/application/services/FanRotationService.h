#ifndef _FanRotationService_
#define _FanRotationService_

#include <Arduino.h>
#include <framework/common/Event.h>

using namespace std;

namespace Services
{
  namespace FanRotation
  {

    void Initialize();

    uint16_t MeasureRPM();

  } // namespace FanRotation
} // namespace Services

#endif

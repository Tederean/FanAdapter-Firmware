#ifndef _PCService_
#define _PCService_

#include <Arduino.h>
#include <framework/common/Event.h>

using namespace std;

namespace Services
{
  namespace PC
  {

    void Initialize();

    float MeasureTargetPowerRatio();

  } // namespace PC
} // namespace Services

#endif

#ifndef _FanPowerService_
#define _FanPowerService_

#include <Arduino.h>

using namespace std;

namespace Services
{
  namespace FanPower
  {

    void Initialize();

    uint8_t GetFanPower();

    void SetFanPower(uint8_t power);

  } // namespace FanPower
} // namespace Services

#endif

#include <Arduino.h>
#include <application/services/FanPowerService.h>

using namespace std;

namespace Services
{
  namespace FanPower
  {

    uint8_t CurrentFanPower;

    void Initialize();

    uint8_t GetFanPower();

    void SetFanPower(uint8_t power);

    void Initialize()
    {
      pinMode(FAN_PWM, OUTPUT);
      pinMode(FAN_VCC, OUTPUT);

#if (FAN_PWM == 9) || (FAN_PWM == 10)
      TCCR1B = TCCR1B & B11111000 | B00000001; // Adjust PWM frequency, affects pin 9 and 10.
#endif

      SetFanPower(0xFF);
    }

    uint8_t GetFanPower()
    {
      return CurrentFanPower;
    }

    void SetFanPower(uint8_t power)
    {
      CurrentFanPower = power;

      digitalWrite(FAN_VCC, power > 0);
      analogWrite(FAN_PWM, power);
    }

  } // namespace FanPower
} // namespace Services
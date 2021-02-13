#include <Arduino.h>
#include <application/services/PCService.h>
#include <framework/services/SystemService.h>
#include <framework/utils/Math.h>

using namespace std;

namespace Services
{
  namespace PC
  {

    void Initialize();

    uint8_t MeasureTargetPower();

    const uint32_t MeasurementTimeout = 200UL;

    void Initialize()
    {
#ifdef PC_SIMULATION
      analogReadResolution(8);
#endif

      pinMode(PC_PWM, INPUT);
    }

    uint8_t MeasureTargetPower()
    {
#ifdef PC_SIMULATION
      return analogRead(PC_PWM);
#else
      uint32_t highTime_us = 0;

      noInterrupts();

      highTime_us = pulseIn(PC_PWM, HIGH, MeasurementTimeout);

      interrupts();

      if (highTime_us > 0)
      {
        uint32_t lowTime_us = 0;
        
        noInterrupts();

        lowTime_us = pulseIn(PC_PWM, LOW, MeasurementTimeout);

        interrupts();

        if (lowTime_us > 0)
        {
          uint32_t powerValue = (highTime_us * 0xFF) / (lowTime_us + highTime_us);

          return Math::Clamp<uint32_t>(powerValue, 0x00, 0xFF);
        }
      }

      return digitalRead(PC_PWM) == HIGH ? 0xFF : 0x00;
#endif
    }

  } // namespace PC
} // namespace Services
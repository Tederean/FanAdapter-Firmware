#include <Arduino.h>
#include <util/atomic.h>
#include <application/services/PCService.h>
#include <framework/services/SystemService.h>

using namespace std;

namespace Services
{
  namespace PC
  {

    void Initialize();

    float MeasureTargetPowerRatio();

    const uint32_t MeasurementTimeout = 200UL;

    void Initialize()
    {
      pinMode(PC_PWM, INPUT);
    }

    float MeasureTargetPowerRatio()
    {
      float highTime_us = 0.0f;

      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
      {
        highTime_us = pulseIn(PC_PWM, HIGH, MeasurementTimeout);
      }

      if (highTime_us > 0.0f)
      {
        float lowTime_us = 0.0f;
        
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
          lowTime_us = pulseIn(PC_PWM, LOW, MeasurementTimeout);
        }

        if (lowTime_us > 0.0f)
        {
          return highTime_us / (lowTime_us + highTime_us);
        }
      }

      return digitalRead(PC_PWM) == HIGH ? 1.0f : 0.0f;
    }

  } // namespace PC
} // namespace Services
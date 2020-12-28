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

    //void OnTimeToMeasure(void *args);

    float MeasureDutyCycle();

    //const timespan_t MeasurementInterval = 250UL * 1000UL;

    const uint32_t MeasurementTimeout = 200UL;

    //Event<float> ControlValueMeasuredEvent;

    //Event<void> TimeToMeasureEvent;

    void Initialize()
    {
      pinMode(PC_PWM, INPUT);

      //TimeToMeasureEvent.Subscribe(OnTimeToMeasure);
      //Services::System::InvokeLater(&TimeToMeasureEvent, MeasurementInterval, true);
    }

    //void OnTimeToMeasure(void *args)
    //{
    //  float dutyCycle = MeasureDutyCycle();
    //
    //  ControlValueMeasuredEvent.Invoke(&dutyCycle);
    //}

    float MeasureDutyCycle()
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

      return 1.0f;
    }

  } // namespace PC
} // namespace Services
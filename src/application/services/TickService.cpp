#include <Arduino.h>
#include <framework/services/SystemService.h>
#include <application/services/FanPowerService.h>
#include <application/services/FanRotationService.h>
#include <application/services/PCService.h>
#include <application/services/TickService.h>
#include <FastPID.h>

using namespace std;

namespace Services
{
  namespace Tick
  {

    void Initialize();

    void OnTickEvent(void *args);

    const timespan_t TickInterval = 100UL * 1000UL;

    const float Kp = 0.4f, Ki = 0.4f, Kd = 0.05f;

    FastPID PID(Kp, Ki, Kd, (1000000.0f / TickInterval), 8);

    Event<void> TickEvent;

    void Initialize()
    {
      TickEvent.Subscribe(OnTickEvent);
      Services::System::InvokeLater(&TickEvent, TickInterval, true);
    }

    void OnTickEvent(void *args)
    {
      float currentRPM = Services::FanRotation::GetCurrentRPM();
      float targetDutyCycle = Services::PC::MeasureDutyCycle();
      float targetRPM = 1500.0f;

      uint8_t outputPower = PID.step(targetRPM, currentRPM);

      Services::FanPower::SetFanPower(outputPower);

      //Serial.print("RPM=");
      //Serial.print(currentRPM, 0);
      //Serial.print(" | PC=");
      //Serial.print(targetDutyCycle *100.0f, 0);
      //Serial.print("% | PWM=");
      //Serial.print(outputPower / 2.55f, 0);
      //Serial.print("%\n");
    }

  } // namespace Tick
} // namespace Services
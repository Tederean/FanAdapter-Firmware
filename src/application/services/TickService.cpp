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

    float PowerRatioToRPM(float ratio);

    float GetFanPowerRatio(float targetPowerRatio);

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
      float currentRPM = Services::FanRotation::MeasureRPM();
      float targetPowerRatio = Services::PC::MeasureTargetPowerRatio();

      float fanPowerRatio = GetFanPowerRatio(targetPowerRatio);
      float targetRPM = PowerRatioToRPM(fanPowerRatio);

      uint8_t outputPower = PID.step(targetRPM, currentRPM);
      Services::FanPower::SetFanPower(outputPower);

      Serial.print("RPM=");
      Serial.print(currentRPM, 0);
      Serial.print(" | PC=");
      Serial.print(targetPowerRatio *100.0f, 0);
      Serial.print("% | PWM=");
      Serial.print(outputPower / 2.55f, 0);
      Serial.print("%\n");
    }

    float GetFanPowerRatio(float targetPowerRatio)
    {
      //  0% - 30% => Zero RPM
      // 31% - 50% => Slow proportional controller
      // Wait 2 Minutes
      // 51% - 70% => Slow proportional controller

      if (targetPowerRatio <= 0.3f)
      {
        return 0.0f;
      }

      return targetPowerRatio;
    }

    float PowerRatioToRPM(float ratio)
    {
      return ratio * 1500.0f;
    }

  } // namespace Tick
} // namespace Services
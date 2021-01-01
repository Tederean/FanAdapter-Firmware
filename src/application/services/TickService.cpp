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

    uint16_t PowerToRPM(uint8_t power);

    const timespan_t TickInterval = 100UL * 1000UL;

    const float TickFrequency = 1000000.0f / TickInterval;

    FastPID PID(0.4f, 0.4f, 0.05f, TickFrequency, 8);

    FastPID PReg(0.0f, 0.0f, 0.0f, TickFrequency, 8);

    Event<void> TickEvent;

    void Initialize()
    {
      TickEvent.Subscribe(OnTickEvent);
      Services::System::InvokeLater(&TickEvent, TickInterval, true);
    }

    void OnTickEvent(void *args)
    {
      uint16_t currentRPM = Services::FanRotation::MeasureRPM();
      uint8_t targetPower = Services::PC::MeasureTargetPower();

      uint8_t fanPower = targetPower; // PReg.step(178, targetPower);
      uint16_t targetRPM = PowerToRPM(fanPower);

      uint8_t outputPower = PID.step(targetRPM, currentRPM);
      Services::FanPower::SetFanPower(outputPower);


#ifdef SERIAL_DEBUG
      Serial.print("RPM=");
      Serial.print(currentRPM);
      Serial.print(" | PC=");
      Serial.print(targetPower / 2.55f, 0);
      Serial.print("% | TRPM=");
      Serial.print(targetRPM);
      Serial.print(" | PWM=");
      Serial.print(outputPower / 2.55f, 0);
      Serial.print("%\n");
#endif

    }

    uint16_t PowerToRPM(uint8_t power)
    {
      float ratio = power / 255.0f;

      return ratio * 1500;
    }

  } // namespace Tick
} // namespace Services
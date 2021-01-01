#include <Arduino.h>
#include <framework/services/SystemService.h>
#include <application/services/FanPowerService.h>
#include <application/services/FanRotationService.h>
#include <application/services/PCService.h>
#include <application/services/TickService.h>
#include <application/utils/FastPID.h>

using namespace std;

namespace Services
{
  namespace Tick
  {

    void Initialize();

    void OnTickEvent(void *args);

    uint8_t Step(uint8_t targetPower, uint8_t currentPower);

    const timespan_t TickInterval = 100UL * 1000UL;

    const float TickFrequency = 1000000.0f / TickInterval;

    FastPID PID(0.0f, 0.1f, 0.0f, TickFrequency, 8);

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

      uint8_t currentPower = Services::FanPower::GetFanPower();
      uint8_t outputPower = PID.step(targetPower, currentPower);

      Services::FanPower::SetFanPower(outputPower);


      Debug("RPM=");
      Debug(currentRPM);

      Debug(" | ");

      Debug("target=");
      Debug(targetPower);

      Debug(" | ");

      Debug("output=");
      Debug(outputPower);

      Debug("\n");
    }

  } // namespace Tick
} // namespace Services
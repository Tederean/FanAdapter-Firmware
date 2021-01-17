#include <Arduino.h>
#include <vector>
#include <framework/services/SystemService.h>
#include <application/services/FanPowerService.h>
#include <application/services/FanRotationService.h>
#include <application/services/PCService.h>
#include <application/services/TickService.h>
#include <application/common/FanRegulator.h>

using namespace std;

namespace Services
{
  namespace Tick
  {

    void Initialize();

    void OnTickEvent(void *args);

    const timespan_t TickInterval = 100UL * 1000UL;

    const float TickFrequency = 1000000.0f / TickInterval;

    vector<Restpoint> Restpoints =
    {
      { 10, 3000000/TickInterval, ValueChange::Rising },
      { 100, 3000000/TickInterval, ValueChange::Rising },
      { 200, 3000000/TickInterval, ValueChange::Rising },

      { 10, 3000000/TickInterval, ValueChange::Falling },
      { 100, 3000000/TickInterval, ValueChange::Falling },
      { 200, 3000000/TickInterval, ValueChange::Falling },
    };

    FanRegulator Regulator(0.0f, 0.1f, 0.0f, TickFrequency, 0, 255, &Restpoints);

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
      uint8_t outputPower = Regulator.Step(targetPower, currentPower);

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
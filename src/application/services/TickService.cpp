#include <Arduino.h>

#if defined(__SAM3X8E__) || defined(__SAMD21G18A__) || defined(ARDUINO_ARCH_RP2040)
#undef min
#undef max
#include <algorithm>
#endif

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
      { 48, 5000000/TickInterval, ValueChange::Rising },
      { 127, 30000000/TickInterval, ValueChange::Rising },

      { 52, 15000000/TickInterval, ValueChange::Falling },
    };

    FanRegulator Regulator(0.0f, 0.1f, 0.0f, TickFrequency, 40, 178, &Restpoints);

    Event<void> TickEvent;

    void Initialize()
    {
      TickEvent.Subscribe(OnTickEvent);
      Services::System::InvokeLater(&TickEvent, TickInterval, SchedulingBehaviour::FixedPeriodSkipTicks);
    }

    void OnTickEvent(void *args)
    {
#ifndef PC_SIMULATION
      uint16_t currentRPM = Services::FanRotation::MeasureRPM();
#endif
      uint8_t targetPower = Services::PC::MeasureTargetPower();

      uint8_t currentPower = Services::FanPower::GetFanPower();
      uint8_t outputPower = Regulator.Step(targetPower, currentPower);

      Services::FanPower::SetFanPower(outputPower);


#ifndef PC_SIMULATION
      Debug("RPM=");
      Debug(currentRPM);

      Debug(" | ");
#endif

      Debug("target=");
      Debug(targetPower);

      Debug(" | ");

      Debug("output=");
      Debug(outputPower);

      Debug("\n");
    }

  } // namespace Tick
} // namespace Services
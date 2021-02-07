#include <Arduino.h>
#include <vector>
#include <framework/services/SystemService.h>

#ifdef ENV_MICRO
#include <application/services/FanPowerService.h>
#include <application/services/FanRotationService.h>
#include <application/services/PCService.h>
#endif

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

#if defined(ENV_UNO) || defined(ENV_MICRO_SIM)
    uint8_t OutputPower;
#endif

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
      Services::System::InvokeLater(&TickEvent, TickInterval, true);

#ifdef ENV_UNO
      pinMode(A0, INPUT);
#endif

#if defined(ENV_UNO) || defined(ENV_MICRO_SIM)
    OutputPower = 0;
#endif
    }

#if defined(ENV_UNO)

    void OnTickEvent(void *args)
    {
      uint8_t targetPower = (analogRead(A0) >> 2);

      uint8_t currentPower = OutputPower;
      uint8_t outputPower = Regulator.Step(targetPower, currentPower);

      Debug(targetPower);
      Debug('\t');
      Debug(outputPower);
      Debug('\n');

      OutputPower = outputPower;
    }

#elif defined(ENV_MICRO_SIM)

    void OnTickEvent(void *args)
    {
      uint8_t targetPower = 255;

      uint8_t currentPower = OutputPower;
      uint8_t outputPower = Regulator.Step(targetPower, currentPower);

      OutputPower = outputPower;
    }

#elif defined(ENV_MICRO)

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
#endif

  } // namespace Tick
} // namespace Services
#include <Arduino.h>
#include <util/atomic.h>
#include <application/services/FanRotationService.h>
#include <framework/services/SystemService.h>

using namespace std;

namespace Services
{
  namespace FanRotation
  {

    void Initialize();

    void OnEnableInterrupt(void *args);

    float MeasureRPM();

    void OnRisingEdgeInterrupt();

    volatile timespan_t LastEdge;

    volatile timespan_t CurrentEdge;

    void Initialize()
    {
      LastEdge = 0L;
      CurrentEdge = 0L;

      pinMode(FAN_RPM, INPUT);

      Services::System::LoopEvent.Subscribe(OnEnableInterrupt);
    }

    void OnEnableInterrupt(void *args)
    {
      Services::System::LoopEvent.Unsubscribe(OnEnableInterrupt);

      attachInterrupt(digitalPinToInterrupt(FAN_RPM), OnRisingEdgeInterrupt, RISING);
    }

    float MeasureRPM()
    {
      timespan_t copyOfCurrentEdge;
      timespan_t copyOfLastEdge;

      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
      {
        copyOfCurrentEdge = CurrentEdge;
        copyOfLastEdge = LastEdge;

        CurrentEdge = 0;
        LastEdge = 0;
      }

      timespan_t periodDuration = abs(copyOfCurrentEdge - copyOfLastEdge);

      if (periodDuration > 0L && periodDuration < 100000L)
      {
        return 30000000.0f / (float)periodDuration;
      }

      return 0.0f;
    }

    void OnRisingEdgeInterrupt()
    {
      LastEdge = CurrentEdge;
      CurrentEdge = Services::System::GetUptime_us();
    }

  } // namespace FanRotation
} // namespace Services
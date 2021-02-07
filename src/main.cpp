#include <Arduino.h>
#include <framework/services/SystemService.h>

#ifdef ENV_MICRO
#include <application/services/PCService.h>
#include <application/services/FanPowerService.h>
#include <application/services/FanRotationService.h>
#endif

#include <application/services/TickService.h>

void loop()
{
  Services::System::LoopEvent.Invoke(nullptr);
}

void setup()
{
  Services::System::Initialize();
  
#ifdef ENV_MICRO
  Services::PC::Initialize();
  Services::FanPower::Initialize();
  Services::FanRotation::Initialize();
#endif

  Services::Tick::Initialize();
}
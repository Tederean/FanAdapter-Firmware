#include <Arduino.h>
#include <framework/services/SystemService.h>
#include <application/services/FanPowerService.h>
#include <application/services/FanRotationService.h>
#include <application/services/PCService.h>
#include <application/services/TickService.h>

void loop()
{
  Services::System::LoopEvent.Invoke(nullptr);
}

void setup()
{
  Services::System::Initialize();
  
  Services::PC::Initialize();
  Services::FanPower::Initialize();
  Services::FanRotation::Initialize();
  
  Services::Tick::Initialize();
}
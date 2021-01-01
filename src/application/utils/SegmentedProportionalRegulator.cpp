#include <Arduino.h>
#include <application/utils/SegmentedProportionalRegulator.h>

using namespace std;

SegmentedProportionalRegulator::SegmentedProportionalRegulator()
{
  CurrentPowerRatio = 0;
}

float SegmentedProportionalRegulator::Step(float targetPowerRatio)
{
  if (targetPowerRatio <= 0.3f)
  {
    CurrentPowerRatio = 0;
    return CurrentPowerRatio;
  }



  //  0% - 30% => Zero RPM
  // 31% - 50% => Slow proportional controller
  // Wait 2 Minutes
  // 51% - 70% => Slow proportional controller


  return CurrentPowerRatio;
}
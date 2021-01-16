#include <Arduino.h>
#include <framework/services/SystemService.h>
#include <application/common/FanRegulator.h>
#include <application/common/FastPID.h>

using namespace std;

FanRegulator::FanRegulator(float proportionalValue, float integralValue, float differentialValue, float tickFrequency, uint8_t minValue, uint8_t maxValue, Restpoint *risingRestpoints, Restpoint *fallingRestpoints)
{
  PID = new FastPID(proportionalValue, integralValue, differentialValue, tickFrequency, 8);
  PID->SetOutputRange(minValue, maxValue);

  RisingRestpoints = risingRestpoints;
  FallingRestpoints = fallingRestpoints;
}

uint8_t FanRegulator::Step(uint8_t targetPower, uint8_t currentPower)
{
  return targetPower;
}
#include <Arduino.h>
#include <framework/services/SystemService.h>
#include <application/common/FanRegulator.h>
#include <application/common/FastPID.h>

using namespace std;

void FanRegulator::ResetRemainingTime(Restpoint *restpoint)
{
  restpoint->RemainingTime = restpoint->Restperiod;
}

void FanRegulator::ResetAllRemainingTimes(Restpoint *restpoint)
{
  uint32_t structLength = sizeof(Restpoint);
  uint8_t arrayLength = sizeof(restpoint) / structLength;
  
  for (uint8_t counter = 0; counter < arrayLength; counter++)
  {
    ResetRemainingTime(restpoint + (counter * structLength));
  }
}

FanRegulator::FanRegulator(float proportionalValue, float integralValue, float differentialValue, float tickFrequency, uint8_t minValue, uint8_t maxValue, Restpoint *risingRestpoints, Restpoint *fallingRestpoints)
{
  PID = FastPID(proportionalValue, integralValue, differentialValue, tickFrequency, 8);
  PID.SetOutputRange(minValue, maxValue);

  ResetAllRemainingTimes(risingRestpoints);
  ResetAllRemainingTimes(fallingRestpoints);
  
  RisingRestpoints = risingRestpoints;
  FallingRestpoints = fallingRestpoints;
}

uint8_t FanRegulator::Step(uint8_t targetPower, uint8_t currentPower)
{
  return targetPower;
}
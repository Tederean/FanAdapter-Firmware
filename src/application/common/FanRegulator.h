#ifndef _FanRegulator_
#define _FanRegulator_

#include <Arduino.h>
#include <framework/services/SystemService.h>
#include <application/common/FastPID.h>

using namespace std;

typedef struct
{
  uint8_t Target;
  timespan_t Restperiod;
  timespan_t RemainingTime; 
} Restpoint;

class FanRegulator
{

private:

  FastPID *PID;

  Restpoint *RisingRestpoints;

  Restpoint *FallingRestpoints;

public:

  FanRegulator(float proportionalValue, float integralValue, float differentialValue, float tickFrequency, uint8_t minValue, uint8_t maxValue, Restpoint *risingRestpoints, Restpoint *fallingRestpoints);

  uint8_t Step(uint8_t targetPower, uint8_t currentPower);

};

#endif
#ifndef _FanRegulator_
#define _FanRegulator_

#include <Arduino.h>
#include <vector>
#include <framework/services/SystemService.h>
#include <application/common/FastPID.h>

using namespace std;

enum class ValueChange
{
  Stay,
  Rising,
  Falling,
};

typedef struct
{
  uint8_t Target;
  uint16_t PausedTicks;
  ValueChange Direction;
} Restpoint;

class FanRegulator
{

private:

  FastPID PID;

  vector<Restpoint> *AvailableRestpoints;


  uint16_t TickCounter;

  uint8_t LastOutputValue;


  bool HasCurrentRestpoint;

  Restpoint CurrentRestpoint;


  Restpoint FindNextRestpoint(uint8_t value, ValueChange direction);

  ValueChange GetDirection(uint8_t oldValue, uint8_t newValue);

public:

  FanRegulator(float proportionalValue, float integralValue, float differentialValue, float tickFrequency, uint8_t minValue, uint8_t maxValue, vector<Restpoint> *restpoints);

  uint8_t Step(uint8_t targetValue, uint8_t currentValue);

};

#endif
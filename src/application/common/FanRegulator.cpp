#include <Arduino.h>
#include <framework/services/SystemService.h>
#include <application/common/FanRegulator.h>
#include <application/common/FastPID.h>

using namespace std;

FanRegulator::FanRegulator(float proportionalValue, float integralValue, float differentialValue, float tickFrequency, uint8_t minValue, uint8_t maxValue, vector<Restpoint> *restpoints)
{
  PID = FastPID(proportionalValue, integralValue, differentialValue, tickFrequency, 8);
  PID.SetOutputRange(minValue, maxValue);

  AvailableRestpoints = restpoints;

  TickCounter = 0;
  LastOutputValue = 0;

  HasCurrentRestpoint = false;
  LastElapedRestpointValue = 0;
}

Restpoint FanRegulator::FindNextRestpoint(uint8_t value, ValueChange direction, vector<Restpoint> *restpoints, uint8_t lastElapedRestpointValue)
{
  Restpoint fallback = { 255, 0, direction };
  
  {
    uint16_t fallbackDifference = 255;

    for (auto restpoint : *restpoints)
    {
      if (restpoint.Direction != direction)
        continue;

      if ((direction == ValueChange::Rising && restpoint.Target > value) || (direction == ValueChange::Falling && restpoint.Target < value))
      {
        uint8_t difference = abs(restpoint.Target - value);

        if (difference < fallbackDifference && restpoint.Target != lastElapedRestpointValue)
        {
          fallback = restpoint;
          fallbackDifference = difference;
        }
      }
    }
  }

  return fallback;
}

ValueChange FanRegulator::GetDirection(uint8_t oldValue, uint8_t newValue)
{
  if (newValue > oldValue)
  {
    return ValueChange::Rising;
  }

  if (newValue < oldValue)
  {
    return ValueChange::Falling;
  }
  
  return ValueChange::Stay;
}

uint8_t FanRegulator::Step(uint8_t targetValue, uint8_t currentValue)
{
  uint8_t outputValue = PID.Step(targetValue, currentValue);


  if (!HasCurrentRestpoint)
  {
    auto direction = GetDirection(LastOutputValue, outputValue);
    auto nextRestpoint = FindNextRestpoint(LastOutputValue, direction, AvailableRestpoints, LastElapedRestpointValue);

    if (nextRestpoint.PausedTicks > 0 && outputValue >= nextRestpoint.Target)
    {
      HasCurrentRestpoint = true;
      CurrentRestpoint = nextRestpoint;

      TickCounter = nextRestpoint.PausedTicks;
      LastOutputValue = nextRestpoint.Target;

      return nextRestpoint.Target;
    }
  }


  if (HasCurrentRestpoint)
  {
    outputValue = CurrentRestpoint.Target;
    TickCounter--;

    PID.Clear();

    if (TickCounter == 0)
    {
      LastElapedRestpointValue = CurrentRestpoint.Target;
      HasCurrentRestpoint = false;
    }
  }


  LastOutputValue = outputValue;
  return outputValue;
}
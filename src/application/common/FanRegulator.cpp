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

  CurrentRestpoint.PausedTicks = 0;
}

bool FanRegulator::TryFindNextRisingRestpoint(uint8_t lastOutputValue, uint8_t nextOutputValue, vector<Restpoint> *restpoints, Restpoint *foundRestpoint)
{
  foundRestpoint->PausedTicks = 0;
  foundRestpoint->Direction = ValueChange::Rising;
  foundRestpoint->Target = 0;

  {
    uint16_t nextDifference = 255;

    for (auto restpoint : *restpoints)
    {
      if (restpoint.Direction != ValueChange::Rising)
        continue;

      if (restpoint.Target > lastOutputValue && restpoint.Target <= nextOutputValue)
      {
        uint8_t difference = restpoint.Target - lastOutputValue;

        if (difference < nextDifference)
        {
          memcpy(&restpoint, foundRestpoint, sizeof(Restpoint));
          nextDifference = difference;
        }
      }
    }
  }

  return foundRestpoint->PausedTicks > 0;
}

bool FanRegulator::TryFindNextFallingRestpoint(uint8_t lastOutputValue, uint8_t nextOutputValue, vector<Restpoint> *restpoints, Restpoint *foundRestpoint)
{
  foundRestpoint->PausedTicks = 0;
  foundRestpoint->Direction = ValueChange::Falling;
  foundRestpoint->Target = 255;

  {
    uint16_t nextDifference = 255;

    for (auto restpoint : *restpoints)
    {
      if (restpoint.Direction != ValueChange::Falling)
        continue;

      if (restpoint.Target < lastOutputValue && restpoint.Target >= nextOutputValue)
      {
        uint8_t difference = lastOutputValue - restpoint.Target;

        if (difference < nextDifference)
        {
          memcpy(&restpoint, foundRestpoint, sizeof(Restpoint));
          nextDifference = difference;
        }
      }
    }
  }

  return foundRestpoint->PausedTicks > 0;
}

bool FanRegulator::TryFindNextRestpoint(uint8_t lastOutputValue, uint8_t nextOutputValue, vector<Restpoint> *restpoints, Restpoint *foundRestpoint)
{
  if (lastOutputValue == nextOutputValue)
    return false;

  if (nextOutputValue > lastOutputValue)
  {
    return TryFindNextRisingRestpoint(lastOutputValue, nextOutputValue, restpoints, foundRestpoint);
  }
  else
  {
    return TryFindNextFallingRestpoint(lastOutputValue, nextOutputValue, restpoints, foundRestpoint);
  }
}

bool FanRegulator::HasCurrentRestpoint()
{
  return CurrentRestpoint.PausedTicks > 0;
}

bool FanRegulator::DirectionChanged(uint8_t LastOutputValue, uint8_t nextOutputValue, Restpoint *activeRestpoint)
{
  if (nextOutputValue == LastOutputValue)
    return false;

  ValueChange newDirection = nextOutputValue > LastOutputValue ? ValueChange::Rising : ValueChange::Falling;

  return newDirection != activeRestpoint->Direction;
}

uint8_t FanRegulator::Step(uint8_t targetValue, uint8_t currentValue)
{
  uint8_t nextOutputValue = PID.Step(targetValue, currentValue);


  if (HasCurrentRestpoint() && DirectionChanged(LastOutputValue, nextOutputValue, &CurrentRestpoint))
  {
    CurrentRestpoint.PausedTicks = 0;
  }


  if (!HasCurrentRestpoint())
  {
    Restpoint nextRestpoint;

    if (TryFindNextRestpoint(LastOutputValue, nextOutputValue, AvailableRestpoints, &nextRestpoint))
    {
      CurrentRestpoint = nextRestpoint;

      TickCounter = nextRestpoint.PausedTicks;
      LastOutputValue = nextRestpoint.Target;

      return nextRestpoint.Target;
    }
  }


  if (HasCurrentRestpoint())
  {
    nextOutputValue = CurrentRestpoint.Target;
    TickCounter--;
    PID.Clear();

    if (TickCounter == 0)
    {
      CurrentRestpoint.PausedTicks = 0;

      if (CurrentRestpoint.Direction == ValueChange::Rising && nextOutputValue < 255)
      {
        nextOutputValue++;
      }
      else if (CurrentRestpoint.Direction == ValueChange::Falling && nextOutputValue > 0)
      {
        nextOutputValue--;
      }
    }
  }


  LastOutputValue = nextOutputValue;
  return nextOutputValue;
}
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
          foundRestpoint->PausedTicks = restpoint.PausedTicks;
          foundRestpoint->Target = restpoint.Target;
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
          foundRestpoint->PausedTicks = restpoint.PausedTicks;
          foundRestpoint->Target = restpoint.Target;
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

void FanRegulator::DeleteCurrentRestpoint()
{
  CurrentRestpoint.PausedTicks = 0;
}

bool FanRegulator::DirectionChanged(uint8_t lastValue, uint8_t nextValue, Restpoint *activeRestpoint)
{
  if (nextValue == lastValue)
    return false;

  ValueChange newDirection = nextValue > lastValue ? ValueChange::Rising : ValueChange::Falling;

  return newDirection != activeRestpoint->Direction;
}

uint8_t FanRegulator::Step(uint8_t targetValue, uint8_t currentValue)
{
  if (HasCurrentRestpoint() && DirectionChanged(LastOutputValue, targetValue, &CurrentRestpoint))
  {
    DeleteCurrentRestpoint();
  }

  if (HasCurrentRestpoint())
  {
    StepCurrentRestpoint(targetValue, currentValue);
  }
  else
  {
    StepWithoutRestpoint(targetValue, currentValue);
  }

  return LastOutputValue;
}

void FanRegulator::StepCurrentRestpoint(uint8_t targetValue, uint8_t currentValue)
{
  TickCounter--;

  if (TickCounter > 0) return;

  DeleteCurrentRestpoint();

  if (CurrentRestpoint.Direction == ValueChange::Rising && LastOutputValue < 255)
  {
    LastOutputValue++;
  }
  else if (CurrentRestpoint.Direction == ValueChange::Falling && LastOutputValue > 0)
  {
    LastOutputValue--;
  }
}

void FanRegulator::StepWithoutRestpoint(uint8_t targetValue, uint8_t currentValue)
{
  uint8_t nextOutputValue = PID.Step(targetValue, currentValue);
  Restpoint nextRestpoint;

  if (TryFindNextRestpoint(LastOutputValue, nextOutputValue, AvailableRestpoints, &nextRestpoint))
  {
    CurrentRestpoint = nextRestpoint;

    TickCounter = nextRestpoint.PausedTicks;
    LastOutputValue = nextRestpoint.Target;
  }
  else
  {
    LastOutputValue = nextOutputValue;
  }
}
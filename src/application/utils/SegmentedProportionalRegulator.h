#ifndef _SegmentedProportionalRegulator_
#define _SegmentedProportionalRegulator_

#include <Arduino.h>

using namespace std;

class SegmentedProportionalRegulator
{

private:

  float CurrentPowerRatio;

public:

  SegmentedProportionalRegulator();

  float Step(float targetPowerRatio);

};

#endif

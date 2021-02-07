#include "FastPID.h"

#include <Arduino.h>

FastPID::~FastPID() {
}

void FastPID::Clear() {
  LastSetpoint = 0; 
  Sum = 0; 
  LastError = 0;
}

bool FastPID::SetCoefficients(float kp, float ki, float kd, float hz) {
  ProportionalValue = FloatToParam(kp);
  IntegralValue = FloatToParam(ki / hz);
  DifferentialValue = FloatToParam(kd * hz);
  return true;
}

bool FastPID::SetOutputConfig(int bits, bool sign) {
  // Set output bits
  if (bits <= 16 && bits >= 1) {
    if (bits == 16) {
      Outmax = (0xFFFFULL >> (17 - bits)) * PARAM_MULT;
    }
    else{
      Outmax = (0xFFFFULL >> (16 - bits)) * PARAM_MULT;
    }
    if (sign) {
      Outmin = -((0xFFFFULL >> (17 - bits)) + 1) * PARAM_MULT;
    }
    else {
      Outmin = 0;
    }
  }
  return true;
}

bool FastPID::SetOutputRange(int16_t min, int16_t max)
{
  if (min >= max) {
    return true;
  }
  Outmin = int64_t(min) * PARAM_MULT;
  Outmax = int64_t(max) * PARAM_MULT;
  return true;
}

bool FastPID::Configure(float kp, float ki, float kd, float hz, int bits, bool sign) {
  Clear();
  SetCoefficients(kp, ki, kd, hz);
  SetOutputConfig(bits, sign);
  return true; 
}

uint32_t FastPID::FloatToParam(float in) {
  if (in > PARAM_MAX || in < 0) {
    return 0;
  }

  uint32_t param = in * PARAM_MULT;

  if (in != 0 && param == 0) {
    return 0;
  }
  
  return param;
}

int16_t FastPID::Step(int16_t setpoint, int16_t feedback) {

  // int16 + int16 = int17
  int32_t err = int32_t(setpoint) - int32_t(feedback);
  int32_t P = 0, I = 0;
  int32_t D = 0;

  if (ProportionalValue) {
    // uint16 * int16 = int32
    P = int32_t(ProportionalValue) * int32_t(err);
  }

  if (IntegralValue) {
    // int17 * int16 = int33
    Sum += int64_t(err) * int64_t(IntegralValue);

    // Limit sum to 32-bit signed value so that it saturates, never overflows.
    if (Sum > INTEG_MAX)
      Sum = INTEG_MAX;
    else if (Sum < INTEG_MIN)
      Sum = INTEG_MIN;

    // int32
    I = Sum;
  }

  if (DifferentialValue) {
    // (int17 - int16) - (int16 - int16) = int19
    int32_t deriv = (err - LastError) - int32_t(setpoint - LastSetpoint);
    LastSetpoint = setpoint; 
    LastError = err; 

    // Limit the derivative to 16-bit signed value.
    if (deriv > DERIV_MAX)
      deriv = DERIV_MAX;
    else if (deriv < DERIV_MIN)
      deriv = DERIV_MIN;

    // int16 * int16 = int32
    D = int32_t(DifferentialValue) * int32_t(deriv);
  }

  // int32 (P) + int32 (I) + int32 (D) = int34
  int64_t out = int64_t(P) + int64_t(I) + int64_t(D);

  // Make the output saturate
  if (out > Outmax) 
    out = Outmax;
  else if (out < Outmin) 
    out = Outmin;

  // Remove the integer scaling factor. 
  int16_t rval = out >> PARAM_SHIFT;

  // Fair rounding.
  if (out & (0x1ULL << (PARAM_SHIFT - 1))) {
    rval++;
  }

  return rval;
}
